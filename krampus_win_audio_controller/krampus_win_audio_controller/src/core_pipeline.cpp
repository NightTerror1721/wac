#include "core_pipeline.h"

#define CHECK_NULL_POINTER(_Ptr, _Status) if(!(_Ptr)) return ( *(_Status) = E_POINTER, nullptr )

BEGIN_KPAC_NAMESPACE

HRESULT initialize()
{
    return CoInitializeEx(NULL, COINIT::COINIT_MULTITHREADED);
}

void uninitialize()
{
    CoUninitialize();
}


static IAudioSessionManager2* _open_session_manager(ResourceReference<IMMDevice> device, HRESULT* resultPtr)
{
    CHECK_NULL_POINTER(device, resultPtr);
    IAudioSessionManager2* session;
    *resultPtr = device->Activate(__uuidof(IAudioSessionManager2), CLSCTX_INPROC_SERVER, NULL, (void**)&session);
    if (FAILED(*resultPtr))
        return nullptr;
    return session;
}

static IAudioSessionControl* _open_session_control(ResourceReference<IAudioSessionManager2> manager, HRESULT* resultPtr)
{
    CHECK_NULL_POINTER(manager, resultPtr);
    IAudioSessionControl* control;
    *resultPtr = manager->GetAudioSessionControl(NULL, 0, &control);
    if (FAILED(*resultPtr))
        return nullptr;
    return control;
}

static IAudioSessionControl2* _open_session_control2(ResourceReference<IAudioSessionControl> control, HRESULT* resultPtr)
{
    CHECK_NULL_POINTER(control, resultPtr);
    IAudioSessionControl2* control2;
    *resultPtr = control->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&control2);
    if (FAILED(*resultPtr))
        return nullptr;
    return control2;
}

static ISimpleAudioVolume* _open_audio_volume(ResourceReference<IAudioSessionManager2> manager, HRESULT* resultPtr)
{
    CHECK_NULL_POINTER(manager, resultPtr);
    ISimpleAudioVolume* volume;
    *resultPtr = manager->GetSimpleAudioVolume(NULL, 0, &volume);
    if (FAILED(*resultPtr))
        return nullptr;
    return volume;
}

static IAudioSessionEnumerator* _open_session_enumerator(ResourceReference<IAudioSessionManager2> manager, HRESULT* resultPtr)
{
    CHECK_NULL_POINTER(manager, resultPtr);
    IAudioSessionEnumerator* enumerator;
    *resultPtr = manager->GetSessionEnumerator(&enumerator);
    if (FAILED(*resultPtr))
        return nullptr;
    return enumerator;
}

static IMMDeviceEnumerator* _open_device_enumerator(HRESULT* resultPtr)
{
    IMMDeviceEnumerator* enumerator;
    *resultPtr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator),
        NULL,
        CLSCTX_INPROC_SERVER,
        __uuidof(IMMDeviceEnumerator),
        (void**)&enumerator
    );
    if (FAILED(*resultPtr))
        return nullptr;
    return enumerator;
}

static IMMDeviceCollection* _open_device_collection(ResourceReference<IMMDeviceEnumerator> deviceEnum, HRESULT* resultPtr)
{
    CHECK_NULL_POINTER(deviceEnum, resultPtr);
    IMMDeviceCollection* col;
    deviceEnum->EnumAudioEndpoints(EDataFlow::eRender, DEVICE_STATE_ACTIVE | DEVICE_STATE_DISABLED, &col);
    if (FAILED(*resultPtr))
        return nullptr;
    return col;
}




ResourceController::ResourceController() :
    _status{}
{}
ResourceController::~ResourceController() {}

HRESULT ResourceController::status() const { return _status; }
bool ResourceController::hasErrors() const { return FAILED(_status); }




Device::Device(IMMDevice* nativeDevice) :
    ResourceController{},
    _device{ nativeDevice }
{}
Device::~Device() {}

SessionManager Device::getManager()
{
    return { &_device };
}




DeviceManager::DeviceManager() :
    ResourceController{},
    _enum{ _open_device_enumerator(&_status) },
    _col{ _open_device_collection(&_enum, &_status) }
{}
DeviceManager::~DeviceManager() {}

unsigned int DeviceManager::getDeviceCount()
{
    UINT count;
    _col->GetCount(&count);
    return count;
}

Device DeviceManager::getDefaultDevice()
{
    IMMDevice* device;
    if (FAILED(_enum->GetDefaultAudioEndpoint(EDataFlow::eRender, ERole::eConsole, &device)))
        return { nullptr };
    return { device };
}




SessionControl::SessionControl(ResourceReference<IAudioSessionManager2> session) :
    ResourceController{},
    _baseControl{ _open_session_control(session, &_status) },
    _control{ _open_session_control2(&_baseControl, &_status) }
{}
SessionControl::SessionControl(Resource<IAudioSessionControl>&& nativeControl) :
    ResourceController{},
    _baseControl{ std::move(nativeControl) },
    _control{ _open_session_control2(&_baseControl, &_status) }
{}
SessionControl::~SessionControl() {}

DWORD SessionControl::getProcessId()
{
    DWORD pid = 0;
    _control->GetProcessId(&pid);
    return pid;
}

bool SessionControl::isSystemSoundSession()
{
    return _control->IsSystemSoundsSession() == S_OK;
}

std::wstring SessionControl::getRawSessionIdentifier()
{
    LPWSTR w_str;
    if (_control->GetSessionIdentifier(&w_str) != S_OK)
    {
        CoTaskMemFree(w_str);
        return L"";
    }

    std::wstring str{ w_str };
    CoTaskMemFree(w_str);
    return std::move(str);
}




AudioVolume::AudioVolume(ResourceReference<IAudioSessionManager2> manager) :
    ResourceController{},
    _volume{ _open_audio_volume(manager, &_status) }
{}
AudioVolume::AudioVolume(Resource<ISimpleAudioVolume>&& nativeVolume) :
    ResourceController{},
    _volume{ std::move(nativeVolume) }
{}
AudioVolume::~AudioVolume() {}

bool AudioVolume::isMute()
{
    BOOL flag;
    _volume->GetMute(&flag);
    return flag;
}
void AudioVolume::setMute(bool flag)
{
    _volume->SetMute(flag, NULL);
}

float AudioVolume::getVolume()
{
    float vol;
    _volume->GetMasterVolume(&vol);
    return vol;
}
void AudioVolume::setVolume(float volume)
{
    _volume->SetMasterVolume(volume, NULL);
}










SessionManager::SessionManager(ResourceReference<IMMDevice> device) :
    ResourceController{},
    _session{ _open_session_manager(device, &_status) }
{}
SessionManager::~SessionManager()
{}

SessionControl SessionManager::getMasterSession()
{
    return { &_session };
}

AudioVolume SessionManager::getMasterVolume()
{
    return { &_session };
}

SessionCollection SessionManager::getSessionCollection()
{
    return { &_session };
}





SessionCollection::SessionCollection(ResourceReference<IAudioSessionManager2> manager) :
    ResourceController{},
    _col{ _open_session_enumerator(manager, &_status) }
{}
SessionCollection::~SessionCollection() {}

int SessionCollection::getSessionCount()
{
    int count;
    _col->GetCount(&count);
    return count;
}

SessionControl SessionCollection::getSession(int index)
{
    IAudioSessionControl* control;
    if (FAILED(_col->GetSession(index, &control)))
        return { ResourceReference<IAudioSessionManager2>{ nullptr } };

    return { std::move(Resource<IAudioSessionControl>{ control }) };
}

AudioVolume SessionCollection::getVolume(int index)
{
    ISimpleAudioVolume* volume;
    IAudioSessionControl* control;
    if (FAILED(_col->GetSession(index, &control)))
        return { ResourceReference<IAudioSessionManager2>{ nullptr } };

    if (FAILED(control->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&volume)))
        return { ResourceReference<IAudioSessionManager2>{ nullptr } };

    return { std::move(Resource<ISimpleAudioVolume>{ volume }) };
}


END_KPAC_NAMESPACE

#include "core_pipeline.h"

#define CHECK_NULL_POINTER(_Ptr, _Status) if(!(_Ptr)) return ( *(_Status) = E_POINTER, nullptr )

BEGIN_KPAC_NAMESPACE

HRESULT COM_Initialize()
{
    return CoInitializeEx(NULL, COINIT::COINIT_MULTITHREADED);
}

void COM_Uninitialize()
{
    CoUninitialize();
}

namespace core
{

    static COM_Object<IAudioSessionManager2> _open_session_manager(COM_Object<IMMDevice> device, HRESULT* resultPtr)
    {
        CHECK_NULL_POINTER(device, resultPtr);
        IAudioSessionManager2* session;
        *resultPtr = device->Activate(__uuidof(IAudioSessionManager2), CLSCTX_INPROC_SERVER, NULL, (void**)&session);
        if (FAILED(*resultPtr))
            return nullptr;
        return session;
    }

    static COM_Object<IAudioSessionControl> _open_session_control(COM_Object<IAudioSessionManager2> manager, HRESULT* resultPtr)
    {
        CHECK_NULL_POINTER(manager, resultPtr);
        IAudioSessionControl* control;
        *resultPtr = manager->GetAudioSessionControl(NULL, 0, &control);
        if (FAILED(*resultPtr))
            return nullptr;
        return control;
    }

    static COM_Object<IAudioSessionControl2> _open_session_control2(COM_Object<IAudioSessionControl> control, HRESULT* resultPtr)
    {
        CHECK_NULL_POINTER(control, resultPtr);
        IAudioSessionControl2* control2;
        *resultPtr = control->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&control2);
        if (FAILED(*resultPtr))
            return nullptr;
        return control2;
    }

    static COM_Object<ISimpleAudioVolume> _open_audio_volume(COM_Object<IAudioSessionManager2> manager, HRESULT* resultPtr)
    {
        CHECK_NULL_POINTER(manager, resultPtr);
        ISimpleAudioVolume* volume;
        *resultPtr = manager->GetSimpleAudioVolume(NULL, 0, &volume);
        if (FAILED(*resultPtr))
            return nullptr;
        return volume;
    }

    static COM_Object<IAudioSessionEnumerator> _open_session_enumerator(COM_Object<IAudioSessionManager2> manager, HRESULT* resultPtr)
    {
        CHECK_NULL_POINTER(manager, resultPtr);
        IAudioSessionEnumerator* enumerator;
        *resultPtr = manager->GetSessionEnumerator(&enumerator);
        if (FAILED(*resultPtr))
            return nullptr;
        return enumerator;
    }

    static COM_Object<IMMDeviceEnumerator> _open_device_enumerator(HRESULT* resultPtr)
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

    static COM_Object<IMMDeviceCollection> _open_device_collection(COM_Object<IMMDeviceEnumerator> deviceEnum, HRESULT* resultPtr)
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
    ResourceController::ResourceController(HRESULT status) :
        _status{ status }
    {}
    ResourceController::~ResourceController() {}

    HRESULT ResourceController::status() const { return _status; }
    bool ResourceController::hasErrors() const { return FAILED(_status); }

    ResourceController::operator bool() const { return !FAILED(_status); }




    Device::Device(IMMDevice* nativeDevice) :
        ResourceController{},
        _device{ nativeDevice }
    {}
    Device::~Device() {}

    SessionManager Device::getManager()
    {
        if (!_device)
            return {};
        return { _device };
    }




    DeviceManager::DeviceManager() :
        ResourceController{},
        _enum{ _open_device_enumerator(&_status) },
        _col{ _open_device_collection(_enum, &_status) }
    {}
    DeviceManager::~DeviceManager() {}

    unsigned int DeviceManager::getDeviceCount()
    {
        if (!_col)
            return 0;
        UINT count;
        _col->GetCount(&count);
        return count;
    }

    Device DeviceManager::getDevice(unsigned int index)
    {
        if (!_col)
            return {};
        UINT idx = index;
        IMMDevice* device;
        if(FAILED(_col->Item(idx, &device)))
            return {};
        return { device };
    }

    Device DeviceManager::getDefaultDevice()
    {
        if (!_enum)
            return {};
        IMMDevice* device;
        if (FAILED(_enum->GetDefaultAudioEndpoint(EDataFlow::eRender, ERole::eConsole, &device)))
            return {};
        return { device };
    }




    SessionControl::SessionControl(COM_Object<IAudioSessionManager2> session) :
        ResourceController{},
        _baseControl{ _open_session_control(session, &_status) },
        _control{ _open_session_control2(_baseControl, &_status) }
    {}
    SessionControl::SessionControl(COM_Object<IAudioSessionControl>&& nativeControl) :
        ResourceController{},
        _baseControl{ std::move(nativeControl) },
        _control{ _open_session_control2(_baseControl, &_status) }
    {}
    SessionControl::~SessionControl() {}

    DWORD SessionControl::getProcessId()
    {
        if (!_control)
            return 0;
        DWORD pid = 0;
        _control->GetProcessId(&pid);
        return pid;
    }

    bool SessionControl::isSystemSoundSession()
    {
        return _control && _control->IsSystemSoundsSession() == S_OK;
    }

    std::wstring SessionControl::getRawSessionIdentifier()
    {
        if (!_control)
            return L"";

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




    AudioVolume::AudioVolume(COM_Object<IAudioSessionManager2> manager) :
        ResourceController{},
        _volume{ _open_audio_volume(manager, &_status) }
    {}
    AudioVolume::AudioVolume(COM_Object<ISimpleAudioVolume>&& nativeVolume) :
        ResourceController{},
        _volume{ std::move(nativeVolume) }
    {}
    AudioVolume::~AudioVolume() {}

    bool AudioVolume::isMute()
    {
        if (!_volume)
            return true;
        BOOL flag;
        _volume->GetMute(&flag);
        return flag;
    }
    void AudioVolume::setMute(bool flag)
    {
        if(_volume)
            _volume->SetMute(flag, NULL);
    }

    float AudioVolume::getVolume()
    {
        if (!_volume)
            return -1.f;
        float vol;
        _volume->GetMasterVolume(&vol);
        return vol;
    }
    void AudioVolume::setVolume(float volume)
    {
        if(_volume)
            _volume->SetMasterVolume(volume, NULL);
    }










    SessionManager::SessionManager(COM_Object<IMMDevice> device) :
        ResourceController{},
        _session{ _open_session_manager(device, &_status) }
    {}
    SessionManager::~SessionManager()
    {}

    SessionControl SessionManager::getMasterSession()
    {
        if (!_session)
            return {};
        return { _session };
    }

    AudioVolume SessionManager::getMasterVolume()
    {
        if (!_session)
            return {};
        return { _session };
    }

    SessionCollection SessionManager::getSessionCollection()
    {
        if (!_session)
            return {};
        return { _session };
    }





    SessionCollection::SessionCollection(COM_Object<IAudioSessionManager2> manager) :
        ResourceController{},
        _col{ _open_session_enumerator(manager, &_status) }
    {}
    SessionCollection::~SessionCollection() {}

    int SessionCollection::getSessionCount()
    {
        if (!_col)
            return 0;
        int count;
        _col->GetCount(&count);
        return count;
    }

    SessionControl SessionCollection::getSession(int index)
    {
        if (!_col)
            return {};

        IAudioSessionControl* control;
        if (FAILED(_col->GetSession(index, &control)))
            return {};

        return { std::move(COM_Object<IAudioSessionControl>{ control }) };
    }

    AudioVolume SessionCollection::getVolume(int index)
    {
        if (!_col)
            return {};

        ISimpleAudioVolume* volume;
        IAudioSessionControl* control;
        if (FAILED(_col->GetSession(index, &control)))
            return {};

        if (FAILED(control->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&volume)))
            return {};

        return { std::move(COM_Object<ISimpleAudioVolume>{ volume }) };
    }

}

END_KPAC_NAMESPACE

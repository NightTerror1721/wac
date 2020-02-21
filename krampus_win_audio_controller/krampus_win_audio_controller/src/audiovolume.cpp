#include "kpaudiocontr.h"

#include <string>
#include <iostream>

#define EXIT_ON_ERROR(hr)  \
              if (FAILED(hr)) { goto Exit; }
#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

BEGIN_KPAC_NAMESPACE

/*AudioVolumeEvents::AudioVolumeEvents(EDataFlow flow, ERole role, IAudioSessionEvents* session)
{
    IMMDeviceEnumerator* pEnumerator = NULL;
    IMMDevice* pDevice = NULL;
    IMMDeviceCollection* pCol = NULL;
    UINT count = 0;

    _hrStatus = S_OK;
    _pManager = NULL;
    _pControl = NULL;
    _pAudioEvents = session;

    if (_pAudioEvents == NULL)
    {
        _hrStatus = E_POINTER;
        return;
    }

    _pAudioEvents->AddRef();

    // Get the enumerator for the audio endpoint devices
    // on this system.
    _hrStatus = CoCreateInstance(__uuidof(MMDeviceEnumerator),
        NULL, CLSCTX_INPROC_SERVER,
        __uuidof(IMMDeviceEnumerator),
        (void**)&pEnumerator);
    EXIT_ON_ERROR(_hrStatus)


    _hrStatus = pEnumerator->EnumAudioEndpoints(flow, DEVICE_STATE_ACTIVE | DEVICE_STATE_DISABLED, &pCol);
    _hrStatus = pCol->GetCount(&count);

    // Get the audio endpoint device with the specified data-flow
    // direction (eRender or eCapture) and device role.
    _hrStatus = pEnumerator->GetDefaultAudioEndpoint(flow, role,
        &pDevice);
    EXIT_ON_ERROR(_hrStatus)

    // Get the session manager for the endpoint device.
    _hrStatus = pDevice->Activate(__uuidof(IAudioSessionManager2),
        CLSCTX_INPROC_SERVER, NULL,
        (void**)&_pManager2);
    EXIT_ON_ERROR(_hrStatus)

    _pManager = _pManager2;

    // Get the control interface for the process-specific audio
    // session with session GUID = GUID_NULL. This is the session
    // that an audio stream for a DirectSound, DirectShow, waveOut,
    // or PlaySound application stream belongs to by default.
    _hrStatus = _pManager2->GetAudioSessionControl(NULL, 0, &_pControl);
    EXIT_ON_ERROR(_hrStatus)

    _hrStatus = _pControl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&_pControl2);
    EXIT_ON_ERROR(_hrStatus)

    _hrStatus = _pControl->RegisterAudioSessionNotification(_pAudioEvents);
    EXIT_ON_ERROR(_hrStatus)


    test_printDevice();
    enumSessions();


Exit:
    SAFE_RELEASE(pEnumerator)
    SAFE_RELEASE(pDevice)
}
AudioVolumeEvents::~AudioVolumeEvents()
{
    if (_pControl != NULL)
    {
        _pControl->UnregisterAudioSessionNotification(_pAudioEvents);
    }
    SAFE_RELEASE(_pManager)
    SAFE_RELEASE(_pControl)
    SAFE_RELEASE(_pAudioEvents)
}

void AudioVolumeEvents::test_printDevice()
{
    LPWSTR str;
    HRESULT res = _pControl->GetIconPath(&str);
    std::wcout << to_wstring(str) << std::endl;
    CoTaskMemFree(str);
}

HRESULT AudioVolumeEvents::enumSessions()
{
    if (!_pManager2)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;

    int cbSessionCount = 0;
    LPWSTR pswSession = NULL;

    IAudioSessionEnumerator* pSessionList = NULL;
    IAudioSessionControl* pSessionControl = NULL;
    IAudioSessionControl2* pSessionControl2 = NULL;

    // Get the current list of sessions.
    hr = _pManager2->GetSessionEnumerator(&pSessionList);
    if (FAILED(hr))
        goto done;

    // Get the session count.
    hr = pSessionList->GetCount(&cbSessionCount);
    if (FAILED(hr))
        goto done;

    for (int index = 0; index < cbSessionCount; index++)
    {
        CoTaskMemFree(pswSession);
        SAFE_RELEASE(pSessionControl);

        // Get the <n>th session.
        hr = pSessionList->GetSession(index, &pSessionControl);
        if (FAILED(hr))
            goto done;

        hr = pSessionControl->GetDisplayName(&pswSession);
        if (FAILED(hr))
            goto done;

        //wprintf_s(L"Session Name: %s\n", pswSession);

        DWORD sessionPid;
        pSessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&pSessionControl2);
        if (FAILED(hr))
            goto done;

        pSessionControl2->GetProcessId(&sessionPid);
        pSessionControl2->GetSessionIdentifier(&pswSession);

        wprintf_s(L"Session Pid: %d; Name: %s\n", sessionPid, pswSession);
    }

done:
    CoTaskMemFree(pswSession);
    SAFE_RELEASE(pSessionControl);
    SAFE_RELEASE(pSessionList);

    return hr;

}*/

END_KPAC_NAMESPACE

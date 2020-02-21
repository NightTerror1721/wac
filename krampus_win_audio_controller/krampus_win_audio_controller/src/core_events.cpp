#include "core_events.h"

#include <cstdio>



BEGIN_KPAC_NAMESPACE

AudioSessionEvents::AudioSessionEvents() :
    _cRef(1)
{}

AudioSessionEvents::~AudioSessionEvents()
{}

ULONG STDMETHODCALLTYPE AudioSessionEvents::AddRef()
{
    return InterlockedIncrement(&_cRef);
}

ULONG STDMETHODCALLTYPE AudioSessionEvents::Release()
{
    ULONG ulRef = InterlockedDecrement(&_cRef);
    if (0 == ulRef)
    {
        delete this;
    }
    return ulRef;
}

HRESULT STDMETHODCALLTYPE AudioSessionEvents::QueryInterface(REFIID  riid, VOID** ppvInterface)
{
    if (IID_IUnknown == riid)
    {
        AddRef();
        *ppvInterface = (IUnknown*)this;
    }
    else if (__uuidof(IAudioSessionEvents) == riid)
    {
        AddRef();
        *ppvInterface = (IAudioSessionEvents*)this;
    }
    else
    {
        *ppvInterface = NULL;
        return E_NOINTERFACE;
    }
    return S_OK;
}

// Notification methods for audio session events

HRESULT STDMETHODCALLTYPE AudioSessionEvents::OnDisplayNameChanged(LPCWSTR NewDisplayName, LPCGUID EventContext)
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE AudioSessionEvents::OnIconPathChanged(LPCWSTR NewIconPath, LPCGUID EventContext)
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE AudioSessionEvents::OnSimpleVolumeChanged(float NewVolume, BOOL NewMute, LPCGUID EventContext)
{
    if (NewMute)
    {
        printf("MUTE\n");
    }
    else
    {
        printf("Volume = %d percent\n",
            (UINT32)(100 * NewVolume + 0.5));
    }

    return S_OK;
}

HRESULT STDMETHODCALLTYPE AudioSessionEvents::OnChannelVolumeChanged(
    DWORD ChannelCount,
    float NewChannelVolumeArray[],
    DWORD ChangedChannel,
    LPCGUID EventContext
) {
    return S_OK;
}

HRESULT STDMETHODCALLTYPE AudioSessionEvents::OnGroupingParamChanged(LPCGUID NewGroupingParam, LPCGUID EventContext)
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE AudioSessionEvents::OnStateChanged(AudioSessionState NewState)
{
    const char* pszState = "?????";

    switch (NewState)
    {
    case AudioSessionStateActive:
        pszState = "active";
        break;
    case AudioSessionStateInactive:
        pszState = "inactive";
        break;
    }
    printf("New session state = %s\n", pszState);

    return S_OK;
}

HRESULT STDMETHODCALLTYPE AudioSessionEvents::OnSessionDisconnected(AudioSessionDisconnectReason DisconnectReason)
{
    const char* pszReason = "?????";

    switch (DisconnectReason)
    {
    case DisconnectReasonDeviceRemoval:
        pszReason = "device removed";
        break;
    case DisconnectReasonServerShutdown:
        pszReason = "server shut down";
        break;
    case DisconnectReasonFormatChanged:
        pszReason = "format changed";
        break;
    case DisconnectReasonSessionLogoff:
        pszReason = "user logged off";
        break;
    case DisconnectReasonSessionDisconnected:
        pszReason = "session disconnected";
        break;
    case DisconnectReasonExclusiveModeOverride:
        pszReason = "exclusive-mode override";
        break;
    }
    printf("Audio session disconnected (reason: %s)\n",
        pszReason);

    return S_OK;
}


END_KPAC_NAMESPACE

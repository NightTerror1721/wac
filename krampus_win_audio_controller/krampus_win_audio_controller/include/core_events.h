#pragma once
#include <audioclient.h>
#include <audiopolicy.h>
#include <mmdeviceapi.h>

#include "utils.h"


BEGIN_KPAC_NAMESPACE


class AudioSessionEvents : public IAudioSessionEvents
{
private:
    LONG _cRef;

public:
    AudioSessionEvents();
    ~AudioSessionEvents();

    ULONG STDMETHODCALLTYPE AddRef() override;
    ULONG STDMETHODCALLTYPE Release() override;
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID  riid, VOID** ppvInterface) override;

    // Notification methods for audio session events
    HRESULT STDMETHODCALLTYPE OnDisplayNameChanged(LPCWSTR NewDisplayName, LPCGUID EventContext) override;
    HRESULT STDMETHODCALLTYPE OnIconPathChanged(LPCWSTR NewIconPath, LPCGUID EventContext) override;
    HRESULT STDMETHODCALLTYPE OnSimpleVolumeChanged(float NewVolume, BOOL NewMute, LPCGUID EventContext) override;
    HRESULT STDMETHODCALLTYPE OnChannelVolumeChanged(
        DWORD ChannelCount,
        float NewChannelVolumeArray[],
        DWORD ChangedChannel,
        LPCGUID EventContext
    ) override;
    HRESULT STDMETHODCALLTYPE OnGroupingParamChanged(LPCGUID NewGroupingParam, LPCGUID EventContext) override;
    HRESULT STDMETHODCALLTYPE OnStateChanged(AudioSessionState NewState) override;
    HRESULT STDMETHODCALLTYPE OnSessionDisconnected(AudioSessionDisconnectReason DisconnectReason) override;
};


END_KPAC_NAMESPACE

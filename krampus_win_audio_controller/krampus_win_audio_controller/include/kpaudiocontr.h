#pragma once

#include <audioclient.h>
#include <audiopolicy.h>
#include <mmdeviceapi.h>

#include <string>

#define KPAC_NAMESPACE kpac
#define BEGIN_KPAC_NAMESPACE namespace KPAC_NAMESPACE {
#define END_KPAC_NAMESPACE }
#define USE_KPAC using namespace KPAC_NAMESPACE


/* AudioVolume file */
BEGIN_KPAC_NAMESPACE
class AudioVolumeEvents
{
private:
    HRESULT _hrStatus;
    IAudioSessionManager* _pManager;
    IAudioSessionManager2* _pManager2;
    IAudioSessionControl* _pControl;
    IAudioSessionControl2* _pControl2;
    IAudioSessionEvents* _pAudioEvents;

public:
    AudioVolumeEvents(EDataFlow flow, ERole role, IAudioSessionEvents* session);
    ~AudioVolumeEvents();

    inline HRESULT GetStatus() { return _hrStatus; };
    HRESULT enumSessions();
private:
    void test_printDevice();


};
END_KPAC_NAMESPACE
/************************************************************************************************************/

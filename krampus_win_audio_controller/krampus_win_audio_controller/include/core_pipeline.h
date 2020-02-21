#pragma once

#include <audioclient.h>
#include <audiopolicy.h>
#include <mmdeviceapi.h>

#include "utils.h"

BEGIN_KPAC_NAMESPACE


HRESULT initialize();
void uninitialize();



class ResourceController
{
protected:
	HRESULT _status;

public:
	ResourceController();
	virtual ~ResourceController();

	HRESULT status() const;
	bool hasErrors() const;

private:
	ResourceController(const ResourceController&);
	
	ResourceController& operator= (const ResourceController&);
};



class SessionControl : public ResourceController
{
private:
	Resource<IAudioSessionControl> _baseControl;
	Resource<IAudioSessionControl2> _control;

public:
	SessionControl(ResourceReference<IAudioSessionManager2> manager);
	SessionControl(Resource<IAudioSessionControl>&& nativeControl);
	~SessionControl();

	DWORD getProcessId();

	bool isSystemSoundSession();

	std::wstring getRawSessionIdentifier();
};


class AudioVolume : public ResourceController
{
private:
	Resource<ISimpleAudioVolume> _volume;

public:
	AudioVolume(ResourceReference<IAudioSessionManager2> manager);
	AudioVolume(Resource<ISimpleAudioVolume>&& nativeVolume);
	~AudioVolume();

	bool isMute();
	void setMute(bool flag);

	float getVolume();
	void setVolume(float volume);
};


class SessionCollection : public ResourceController
{
private:
	Resource<IAudioSessionEnumerator> _col;

public:
	SessionCollection(ResourceReference<IAudioSessionManager2> manager);
	~SessionCollection();

	int getSessionCount();

	SessionControl getSession(int index);

	AudioVolume getVolume(int index);
};



class SessionManager : public ResourceController
{
private:
	Resource<IAudioSessionManager2> _session;

public:
	SessionManager(ResourceReference<IMMDevice> device);
	~SessionManager();

	SessionControl getMasterSession();

	AudioVolume getMasterVolume();

	SessionCollection getSessionCollection();
};



class Device : public ResourceController
{
private:
	Resource<IMMDevice> _device;

public:
	Device(IMMDevice* nativeDevice);
	~Device();

	SessionManager getManager();
};



class DeviceManager : public ResourceController
{
private:
	Resource<IMMDeviceEnumerator> _enum;
	Resource<IMMDeviceCollection> _col;

public:
	DeviceManager();
	~DeviceManager();

	unsigned int getDeviceCount();

	Device getDefaultDevice();
};



END_KPAC_NAMESPACE


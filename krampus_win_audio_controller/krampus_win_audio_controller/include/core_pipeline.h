#pragma once

#include <audioclient.h>
#include <audiopolicy.h>
#include <mmdeviceapi.h>

#include "utils.h"

BEGIN_KPAC_NAMESPACE


HRESULT COM_Initialize();
void COM_Uninitialize();


namespace core
{

	class ResourceController
	{
	protected:
		HRESULT _status;

	public:
		ResourceController();
		ResourceController(HRESULT status);
		ResourceController(const ResourceController&) = default;
		ResourceController(ResourceController&&) noexcept = default;
		virtual ~ResourceController();

		ResourceController& operator= (const ResourceController&) = default;
		ResourceController& operator= (ResourceController&&) noexcept = default;

		HRESULT status() const;
		bool hasErrors() const;

		operator bool() const;
	};



	class SessionControl : public ResourceController
	{
	private:
		COM_Object<IAudioSessionControl> _baseControl;
		COM_Object<IAudioSessionControl2> _control;

	public:
		inline SessionControl() : ResourceController{ E_POINTER }, _baseControl { nullptr }, _control{ nullptr } {}
		SessionControl(COM_Object<IAudioSessionManager2> manager);
		SessionControl(COM_Object<IAudioSessionControl>&& nativeControl);
		~SessionControl();

		DWORD getProcessId();

		bool isSystemSoundSession();

		std::wstring getRawSessionIdentifier();
	};


	class AudioVolume : public ResourceController
	{
	private:
		COM_Object<ISimpleAudioVolume> _volume;

	public:
		inline AudioVolume() : ResourceController{ E_POINTER }, _volume{ nullptr } {}
		AudioVolume(COM_Object<IAudioSessionManager2> manager);
		AudioVolume(COM_Object<ISimpleAudioVolume>&& nativeVolume);
		~AudioVolume();

		bool isMute();
		void setMute(bool flag);

		float getVolume();
		void setVolume(float volume);
	};


	class SessionCollection : public ResourceController
	{
	private:
		COM_Object<IAudioSessionEnumerator> _col;

	public:
		inline SessionCollection() : ResourceController{ E_POINTER }, _col{ nullptr } {}
		SessionCollection(COM_Object<IAudioSessionManager2> manager);
		~SessionCollection();

		int getSessionCount();

		SessionControl getSession(int index);

		AudioVolume getVolume(int index);
	};



	class SessionManager : public ResourceController
	{
	private:
		COM_Object<IAudioSessionManager2> _session;

	public:
		inline SessionManager() : ResourceController{ E_POINTER }, _session{ nullptr } {}
		SessionManager(COM_Object<IMMDevice> device);
		~SessionManager();

		SessionControl getMasterSession();

		AudioVolume getMasterVolume();

		SessionCollection getSessionCollection();
	};



	class Device : public ResourceController
	{
	private:
		COM_Object<IMMDevice> _device;

	public:
		inline Device() : ResourceController{ E_POINTER }, _device{ nullptr } {}
		Device(IMMDevice* nativeDevice);
		~Device();

		SessionManager getManager();
	};



	class DeviceManager : public ResourceController
	{
	private:
		COM_Object<IMMDeviceEnumerator> _enum;
		COM_Object<IMMDeviceCollection> _col;

	public:
		DeviceManager();
		~DeviceManager();

		unsigned int getDeviceCount();
		Device getDevice(unsigned int index);

		Device getDefaultDevice();
	};

}

END_KPAC_NAMESPACE


bool operator! (const KPAC core::ResourceController& rc);

#pragma once

#include <iostream>
#include <vector>

#include "core_pipeline.h"

BEGIN_KPAC_NAMESPACE


class AudioSessionInfo
{
private:
	DWORD _pid;
	std::wstring _path;
	std::wstring _name;

public:
	AudioSessionInfo(DWORD pid);
	~AudioSessionInfo();

	inline DWORD getProcessId() const { return _pid; }
	inline const std::wstring& getPath() const { return _path; }
	inline const std::wstring& getName() const { return _name; }
};


class AudioEndpointController
{
private:
	core::SessionManager _masterSession;

public:
	AudioEndpointController();
	AudioEndpointController(core::Device* device);
	AudioEndpointController(const AudioEndpointController& c);
	AudioEndpointController(AudioEndpointController&& c) noexcept;
	~AudioEndpointController();

	float getMasterVolume();
	void setMasterVolume(const float volume);
	void setMasterVolume(const Percentage& volume);

	bool isMasterMute();
	void setMasterMute(bool flag);

	size_t getSessionCount();
	std::vector<AudioSessionInfo> getAllSessionsInfo();
	AudioSessionInfo getSessionInfo(const size_t index);

	float getSessionVolume(const AudioSessionInfo& session);
	void setSessionVolume(const AudioSessionInfo& session, const float volume);
	void setSessionVolume(const AudioSessionInfo& session, const Percentage& volume);

	bool isSessionMute(const AudioSessionInfo& session);
	void setSessionMute(const AudioSessionInfo& session, bool flag);


	static AudioEndpointController defaultEndpoint();
	static std::vector<AudioEndpointController> endpoints();

private:
	core::AudioVolume _getSessionVolumeController(const AudioSessionInfo& session);
};



END_KPAC_NAMESPACE


std::wostream& operator<< (std::wostream& os, const KPAC AudioSessionInfo& info);

#include "audio_controller.h"

#include <filesystem>

namespace fs = std::filesystem;

BEGIN_KPAC_NAMESPACE

static std::wstring _extract_name(const std::wstring& spath)
{
	if (spath.empty())
		return L"";

	fs::path path{ spath };
	return path.filename().wstring();
}


AudioSessionInfo::AudioSessionInfo(DWORD pid) :
	_pid{ pid },
	_path{ get_process_name(pid) },
	_name{ _extract_name(_path) }
{}
AudioSessionInfo::~AudioSessionInfo() {}






AudioEndpointController::AudioEndpointController() :
	_masterSession{}
{}
AudioEndpointController::AudioEndpointController(core::Device* device) :
	_masterSession{ device->getManager() }
{}
AudioEndpointController::AudioEndpointController(const AudioEndpointController& c) :
	_masterSession{ c._masterSession }
{}
AudioEndpointController::AudioEndpointController(AudioEndpointController&& c) noexcept :
	_masterSession{ std::move(c._masterSession) }
{
	c._masterSession.~SessionManager();
}
AudioEndpointController::~AudioEndpointController()
{
}

float AudioEndpointController::getMasterVolume() { return _masterSession.getMasterVolume().getVolume(); }
void AudioEndpointController::setMasterVolume(const float volume) { _masterSession.getMasterVolume().setVolume(volume); }
void AudioEndpointController::setMasterVolume(const Percentage& volume) { _masterSession.getMasterVolume().setVolume(volume); }

bool AudioEndpointController::isMasterMute() { return _masterSession.getMasterVolume().isMute(); }
void AudioEndpointController::setMasterMute(bool flag) { _masterSession.getMasterVolume().setMute(flag); }

size_t AudioEndpointController::getSessionCount() { return static_cast<size_t>(_masterSession.getSessionCollection().getSessionCount()); }
std::vector<AudioSessionInfo> AudioEndpointController::getAllSessionsInfo()
{
	core::SessionCollection col = _masterSession.getSessionCollection();
	int len = col.getSessionCount();
	std::vector<AudioSessionInfo> v{ static_cast<size_t>(len) };
	for (int i = 0; i < len; ++i)
		v.push_back({ col.getSession(i).getProcessId() });

	return std::move(v);
}
AudioSessionInfo AudioEndpointController::getSessionInfo(const size_t index)
{
	return { _masterSession.getSessionCollection().getSession(index).getProcessId() };
}

float AudioEndpointController::getSessionVolume(const AudioSessionInfo& session)
{
	core::AudioVolume vol = _getSessionVolumeController(session);
	if (vol.hasErrors())
		return -1;
	return vol.getVolume();
}
void AudioEndpointController::setSessionVolume(const AudioSessionInfo& session, const float volume)
{
	core::AudioVolume vol = _getSessionVolumeController(session);
	if (!vol.hasErrors())
		vol.setVolume(volume);
}
void AudioEndpointController::setSessionVolume(const AudioSessionInfo& session, const Percentage& volume)
{
	core::AudioVolume vol = _getSessionVolumeController(session);
	if (!vol.hasErrors())
		vol.setVolume(volume);
}

bool AudioEndpointController::isSessionMute(const AudioSessionInfo& session)
{
	core::AudioVolume vol = _getSessionVolumeController(session);
	if (vol.hasErrors())
		return true;
	return vol.isMute();
}
void AudioEndpointController::setSessionMute(const AudioSessionInfo& session, bool flag)
{
	core::AudioVolume vol = _getSessionVolumeController(session);
	if (!vol.hasErrors())
		vol.setMute(flag);
}


AudioEndpointController AudioEndpointController::defaultEndpoint()
{
	core::DeviceManager man;
	return { &man.getDefaultDevice() };
}
std::vector<AudioEndpointController> AudioEndpointController::endpoints()
{
	core::DeviceManager man;
	unsigned int len = man.getDeviceCount();
	std::vector<AudioEndpointController> v{ len };
	for (unsigned int i = 0; i < len; ++i)
		v.emplace_back(&man.getDevice(i));
	return v;
}

core::AudioVolume AudioEndpointController::_getSessionVolumeController(const AudioSessionInfo& session)
{
	core::SessionCollection col = _masterSession.getSessionCollection();
	int len = col.getSessionCount();
	for (int i = 0; i < len; ++i)
		if (col.getSession(i).getProcessId() == session.getProcessId())
			return col.getVolume(i);
	return { COM_Object<IAudioSessionManager2>{ nullptr } };
}






END_KPAC_NAMESPACE

std::wostream& operator<< (std::wostream& os, const KPAC AudioSessionInfo& info)
{
	return os << L"[" << info.getProcessId() << L"] " << info.getName();
}

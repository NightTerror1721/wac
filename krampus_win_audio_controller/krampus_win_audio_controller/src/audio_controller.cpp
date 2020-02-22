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
	_path{ !pid ? L"" : get_process_name(pid) },
	_name{ !pid ? L"" : _extract_name(_path) }
{}
AudioSessionInfo::~AudioSessionInfo() {}


bool operator! (const KPAC core::ResourceController& rc) { return rc.hasErrors(); }






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

bool AudioEndpointController::isInvalid() const { return !_masterSession; }
AudioEndpointController::operator bool() const { return _masterSession; }

float AudioEndpointController::getMasterVolume()
{
	if (!_masterSession)
		return -1;
	return _masterSession.getMasterVolume().getVolume();
}
void AudioEndpointController::setMasterVolume(const float volume)
{
	if(_masterSession)
		_masterSession.getMasterVolume().setVolume(volume);
}
void AudioEndpointController::setMasterVolume(const Percentage& volume)
{
	if (_masterSession)
		_masterSession.getMasterVolume().setVolume(volume);
}

bool AudioEndpointController::isMasterMute()
{
	if (!_masterSession)
		return true;
	return _masterSession.getMasterVolume().isMute();
}
void AudioEndpointController::setMasterMute(bool flag)
{
	if (_masterSession)
		_masterSession.getMasterVolume().setMute(flag);
}

size_t AudioEndpointController::getSessionCount()
{
	if (!_masterSession)
		return 0;
	return static_cast<size_t>(_masterSession.getSessionCollection().getSessionCount());
}
std::vector<AudioSessionInfo> AudioEndpointController::getAllSessionsInfo()
{
	if (!_masterSession)
		return {};
	core::SessionCollection col = _masterSession.getSessionCollection();
	int len = col.getSessionCount();
	std::vector<AudioSessionInfo> v{ static_cast<size_t>(len) };
	for (int i = 0; i < len; ++i)
		v.push_back({ col.getSession(i).getProcessId() });

	return std::move(v);
}
AudioSessionInfo AudioEndpointController::getSessionInfo(const size_t index)
{
	if (!_masterSession)
		return AudioSessionInfo::invalid();
	return { _masterSession.getSessionCollection().getSession(index).getProcessId() };
}
AudioSessionInfo AudioEndpointController::getSessionInfoByPid(DWORD pid)
{
	if (!_masterSession)
		return AudioSessionInfo::invalid();
	core::SessionCollection col = _masterSession.getSessionCollection();
	int len = col.getSessionCount();
	for (int i = 0; i < len; ++i)
	{
		DWORD s_pid = col.getSession(i).getProcessId();
		if (s_pid == pid)
			return { s_pid };
	}
	return AudioSessionInfo::invalid();
}

float AudioEndpointController::getSessionVolume(const AudioSessionInfo& session)
{
	core::AudioVolume vol = _getSessionVolumeController(session);
	if (!vol)
		return -1;
	return vol.getVolume();
}
void AudioEndpointController::setSessionVolume(const AudioSessionInfo& session, const float volume)
{
	core::AudioVolume vol = _getSessionVolumeController(session);
	if (!vol)
		vol.setVolume(volume);
}
void AudioEndpointController::setSessionVolume(const AudioSessionInfo& session, const Percentage& volume)
{
	core::AudioVolume vol = _getSessionVolumeController(session);
	if (vol)
		vol.setVolume(volume);
}

bool AudioEndpointController::isSessionMute(const AudioSessionInfo& session)
{
	core::AudioVolume vol = _getSessionVolumeController(session);
	if (!vol)
		return true;
	return vol.isMute();
}
void AudioEndpointController::setSessionMute(const AudioSessionInfo& session, bool flag)
{
	core::AudioVolume vol = _getSessionVolumeController(session);
	if (vol)
		vol.setMute(flag);
}


AudioEndpointController AudioEndpointController::defaultEndpoint()
{
	core::DeviceManager man;
	if (!man)
		return {};
	return { &man.getDefaultDevice() };
}
std::vector<AudioEndpointController> AudioEndpointController::endpoints()
{
	core::DeviceManager man;
	if (!man)
		return {};
	unsigned int len = man.getDeviceCount();
	std::vector<AudioEndpointController> v{ len };
	for (unsigned int i = 0; i < len; ++i)
		v.emplace_back(&man.getDevice(i));
	return v;
}

core::AudioVolume AudioEndpointController::_getSessionVolumeController(const AudioSessionInfo& session)
{
	if (!_masterSession)
		return {};
	core::SessionCollection col = _masterSession.getSessionCollection();
	int len = col.getSessionCount();
	for (int i = 0; i < len; ++i)
		if (col.getSession(i).getProcessId() == session.getProcessId())
			return col.getVolume(i);
	return {};
}






END_KPAC_NAMESPACE

bool operator! (const KPAC AudioSessionInfo& s) { return !s.operator bool(); }
bool operator! (const KPAC AudioEndpointController& c) { return c.isInvalid(); }

std::wostream& operator<< (std::wostream& os, const KPAC AudioSessionInfo& info)
{
	return os << L"[" << info.getProcessId() << L"] " << info.getName();
}

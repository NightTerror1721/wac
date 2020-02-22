#pragma once

#include "audio_controller.h"

#include <functional>
#include <map>


BEGIN_KPAC_NAMESPACE

class TerminalApplication;

class TerminalCommand
{
public:
	typedef std::function<void(TerminalApplication*, AudioEndpointController*, const std::vector<std::string>&)> action_t;

private:
	std::string _name;
	std::string _usage;
	action_t _action;

public:
	TerminalCommand();
	TerminalCommand(const std::string& name, action_t action, const std::string& usage = "");
	~TerminalCommand();

	void operator() (TerminalApplication* app, AudioEndpointController* controller, const std::vector<std::string>& args);

	inline const std::string name() const { return _name; }
	inline const std::string usage() const { return _usage; }
	inline action_t action() const { return _action; }
};


class TerminalApplication
{
private:
	AudioEndpointController _controller;
	std::map<std::string, TerminalCommand> _cmds;
	bool _started;

public:
	TerminalApplication();
	~TerminalApplication();

	bool isInvalid() const;
	operator bool() const;

	const AudioEndpointController& controller() const;

	std::map<std::string, TerminalCommand> commands() const;

	void start();
	void stop();
};


END_KPAC_NAMESPACE


bool operator! (const KPAC TerminalApplication& app);

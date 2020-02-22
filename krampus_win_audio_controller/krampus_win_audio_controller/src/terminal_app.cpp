#include "terminal_app.h"

#include <iostream>
#include <sstream>

BEGIN_KPAC_NAMESPACE


TerminalCommand::TerminalCommand() :
	_name{},
	_usage{},
	_action{}
{}
TerminalCommand::TerminalCommand(const std::string& name, action_t action, const std::string& usage) :
	_name{ name },
	_usage{ usage },
	_action{ action }
{}
TerminalCommand::~TerminalCommand() {}

void TerminalCommand::operator() (TerminalApplication* app, AudioEndpointController* controller, const std::vector<std::string>& args)
{
	_action(app, controller, args);
}



std::vector<TerminalCommand> default_commands();


TerminalApplication::TerminalApplication() :
	_controller{ AudioEndpointController::defaultEndpoint() },
	_cmds{},
	_started{ false }
{
	if (_controller)
		for (const auto& cmd : default_commands())
			_cmds[cmd.name()] = cmd;
}
TerminalApplication::~TerminalApplication() {}

bool TerminalApplication::isInvalid() const { return _controller.isInvalid(); }
TerminalApplication::operator bool() const { return _controller; }

const AudioEndpointController& TerminalApplication::controller() const { return _controller; }

std::map<std::string, TerminalCommand> TerminalApplication::commands() const { return _cmds; }




static void print(const std::string& text) { std::cout << text; }
static void println(const std::string& text) { std::cout << text << std::endl; }
static void newline() { std::cout << std::endl; }

static std::string read_line()
{
	char buff[256];
	std::cin.getline(buff, 256);
	if (std::cin.fail())
		return "";
	return { buff };
}

static std::vector<std::string> extract_args(const std::string& line)
{
	std::vector<std::string> args;
	size_t len = line.size();
	size_t from = 0;
	bool began = false;
	for (size_t i = 0; i < len; ++i)
	{
		char c = line[i];
		if (std::isspace(c))
		{
			if (began)
			{
				began = false;
				args.emplace_back(line.substr(from, i - from));
				from = i + 1;
			}
		}
		else
		{
			if (!began)
			{
				began = true;
				from = i;
			}
		}
	}

	if(began)
		args.emplace_back(line.substr(from));

	return args;
}

static void do_help(const std::map<std::string, TerminalCommand>& cmds)
{
	std::cout << (cmds.size() + 1) << " commands available:" << std::endl;
	println("- help");
	for (const auto& p : cmds)
	{
		println("- " + p.first + " " + p.second.usage());
	}
}

void TerminalApplication::start()
{
	if (_started || isInvalid())
		return;

	_started = true;
	println("Terminal interactive to Krampus Audio Controller API");
	println("type \"help\" to show all commands");

	while (_started)
	{
		print("<kpac>: ");
		std::string line = read_line();
		if (line.empty())
			continue;

		std::vector<std::string> args = extract_args(line);
		if (args.empty())
			continue;

		std::string cmd_name = args[0];
		args.erase(args.begin());

		if (cmd_name == "help")
		{
			do_help(_cmds);
			continue;
		}

		auto it = _cmds.find(cmd_name);
		if (it == _cmds.end())
		{
			println("Unknown command: " + cmd_name);
			continue;
		}

		it->second(this, &_controller, args);
	}
}

void TerminalApplication::stop() { _started = false; }







#define decl_CMD TerminalCommand
#define CMD_Action(_T, _C, _A) [](TerminalApplication* _T, AudioEndpointController* _C, const std::vector<std::string>& _A) -> void 

std::vector<TerminalCommand> default_commands()
{
	return
	{
		decl_CMD {
			"exit",
			CMD_Action(t, c, args) {
				t->stop();
			},
			""
		},

		decl_CMD {
			"session",
			CMD_Action(t, c, args) {
				if (args.size() < 2)
					return;

				std::string arg0 = args[0];
				if (arg0 != "info")
					return;

				std::string arg1 = args[1];
				if (arg1 == "all")
				{
					auto all_sessions = c->getAllSessionsInfo();
					std::cout << all_sessions.size() << " session(s) enabled:" << std::endl;
					for (auto session : all_sessions)
						std::wcout << "- " << session << std::endl;
				}
				else
				{
					std::stringstream ss{ arg1 };
					DWORD pid;
					ss >> pid;
					if (ss.fail())
						println("Invalid session PID");
					else
					{
						auto info = c->getSessionInfoByPid(pid);
						if (!info)
							println("Invalid session PID");
						else std::wcout << info << std::endl;
					}
				}
			},
			"<info> <all|pid:integer>"
		},

		decl_CMD {
			"volume",
			CMD_Action(t, c, args) {
				if (args.size() < 2)
					return;

				std::string mode = args[0];
				std::string source = args[1];
				DWORD pid;
				bool is_master = source == "master";
				if (!is_master)
				{
					std::stringstream ss{ args[1] };
					ss >> pid;
					if (ss.fail())
					{
						println("Invalid session PID");
						return;
					}
				}

				if (mode == "get")
				{
					if (is_master)
					{
						float volume = c->getMasterVolume();
						if (volume < 0)
							println("The master volume is not available");
						else std::cout << "Master volume is " << Percentage::of(volume) << std::endl;
					}
					else
					{
						auto info = c->getSessionInfoByPid(pid);
						if (!info)
						{
							println("Invalid session PID");
							return;
						}
						float volume = c->getSessionVolume(info);
						if (volume < 0)
							std::wcout <<L"The volume of \"" << info << L"\" is not available";
						else std::wcout << L"The volume of \"" << info << L"\" is " << Percentage::of(volume) << std::endl;
					}
				}
				else if (mode == "set")
				{
					if (args.size() < 3)
						return;

					float volume;
					std::stringstream ss{ args[2] };
					ss >> volume;
					if (ss.fail())
					{
						println("Invalid volume value. Required a valid float between 0 and 1.");
						return;
					}

					Percentage pvol = Percentage::of(volume);
					if (is_master)
					{
						c->setMasterVolume(pvol);
						std::cout << "Master volume is set to " << pvol << std::endl;
					}
					else
					{
						auto info = c->getSessionInfoByPid(pid);
						if (!info)
						{
							println("Invalid session PID");
							return;
						}
						c->setSessionVolume(info, pvol);
						std::wcout << "\"" << info << "\" volume is set to " << pvol << std::endl;
					}
				}
				else
				{
					println("Invalid volume mode");
				}
			},
			"<get|set> <master|pid:integer> <?value:float[0-1]>"
		}
	};
}


END_KPAC_NAMESPACE


bool operator! (const KPAC TerminalApplication& app) { return app.isInvalid(); }

#include "audio_controller.h"
#include "terminal_app.h"

#include <iostream>

USE_KPAC;

int main(int argc, char** argv)
{
	COM_Initialize();

	/*AudioEndpointController controller = AudioEndpointController::defaultEndpoint();
	for (auto session : controller.getAllSessionsInfo())
	{
		std::wcout << session << L"; Volume: " << Percentage::of(controller.getSessionVolume(session)) << std::endl;
		//std::wcout << L"[" << control.getProcessId() << L"] " << control.getRawSessionIdentifier() << std::endl;
	}*/
	
	TerminalApplication app;
	app.start();



	COM_Uninitialize();

	return 0;
}

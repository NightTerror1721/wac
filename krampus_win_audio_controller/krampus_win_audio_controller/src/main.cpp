#include "audio_controller.h"

#include <iostream>

USE_KPAC;

int main(int argc, char** argv)
{
	initialize();

	AudioEndpointController controller = AudioEndpointController::defaultEndpoint();
	for (auto session : controller.getAllSessionsInfo())
	{
		std::wcout << session << L"; Volume: " << Percentage::of(controller.getSessionVolume(session)) << std::endl;
		//std::wcout << L"[" << control.getProcessId() << L"] " << control.getRawSessionIdentifier() << std::endl;
	}
	




	uninitialize();

	return 0;
}

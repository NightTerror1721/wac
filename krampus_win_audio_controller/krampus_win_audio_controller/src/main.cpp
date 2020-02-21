#include "core_pipeline.h"

#include <iostream>

USE_KPAC;

int main(int argc, char** argv)
{
	initialize();

	DeviceManager dman;
	Device defDev = dman.getDefaultDevice();
	SessionManager man = defDev.getManager();
	SessionCollection sessions = man.getSessionCollection();
	

	int scount = sessions.getSessionCount();
	for (int i = 0; i < scount; ++i)
	{
		SessionControl control = sessions.getSession(i);
		AudioVolume volume = sessions.getVolume(i);
		if (!control.hasErrors() && !volume.hasErrors())
		{
			std::wcout << L"[" << control.getProcessId() << L"] " << volume.getVolume() << std::endl;
			//std::wcout << L"[" << control.getProcessId() << L"] " << control.getRawSessionIdentifier() << std::endl;
		}
	}
	




	uninitialize();

	return 0;
}

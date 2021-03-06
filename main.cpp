#include <windows.h>
#include "ReadHook.h"

ReadHook app;

void Init()
{
  app.Initialize();
}

void Restore()
{
  app.Restore();
}
//BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
BOOL APIENTRY DllMain(HMODULE module, DWORD reason_for_call, LPVOID reserved)
{
	switch (reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&Init, NULL, 0, NULL);
		break;
	case DLL_PROCESS_DETACH:
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&Restore, NULL, 0, NULL);
		break;
	}
	return TRUE;
}

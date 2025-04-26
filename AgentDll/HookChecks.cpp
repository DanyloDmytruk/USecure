#include "HookChecks.h"

#include "MinHook/include/MinHook.h"


static DWORD(WINAPI* Real_CreateProcessInternal)
(
	DWORD                 unknown1,
	PWCHAR                lpApplicationName,
	PWCHAR                lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL                  bInheritHandles,
	DWORD                 dwCreationFlags,
	LPVOID                lpEnvironment,
	PWCHAR                lpCurrentDirectory,
	LPSTARTUPINFO         lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation,
	DWORD                 unknown2
	);

static DWORD WINAPI My_CreateProcessInternal(
	DWORD                 unknown1,
	PWCHAR                lpApplicationName,
	PWCHAR                lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL                  bInheritHandles,
	DWORD                 dwCreationFlags,
	LPVOID                lpEnvironment,
	PWCHAR                lpCurrentDirectory,
	LPSTARTUPINFO         lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation,
	DWORD                 unknown2)
{

	//MessageBoxW(0, lpApplicationName, lpCommandLine, 0); // For dbg


	// Continue exec..
	DWORD ret = Real_CreateProcessInternal(unknown1,
		lpApplicationName,
		lpCommandLine,
		lpProcessAttributes,
		lpThreadAttributes,
		bInheritHandles,
		dwCreationFlags,
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		lpProcessInformation,
		unknown2);

	return ret;
}

DWORD WINAPI HookingRoutine(LPVOID lpParam)
{
	//Here make hooks on executing processes and function will check it on VT
	MH_STATUS error = MH_Initialize();
	if (error != MH_OK)
	{
		return -1;
	}

	// Hook functions to create process
	MH_CreateHookApi(L"kernel32.dll", "CreateProcessInternalW", My_CreateProcessInternal, (LPVOID*)&Real_CreateProcessInternal);
	MH_CreateHookApi(L"kernelbase.dll", "CreateProcessInternalW", My_CreateProcessInternal, (LPVOID*)&Real_CreateProcessInternal);

	// Enable all hooks
	MH_EnableHook(MH_ALL_HOOKS);

	// Inf. loop
	for (;;)
	{
		Sleep(1000);
	}
		

	return 0;
}


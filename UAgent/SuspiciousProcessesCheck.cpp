#include "SuspiciousProcessesCheck.h"

#include <iostream>
#include <vector>
#include <winternl.h>
#include <tlhelp32.h>
#include <shlwapi.h>


LPCWSTR allowedProcesses[] = {
    L"csrss.exe",
    L"lsass",

    L"chrome.exe",
    L"chromium.exe",
    L"opera.exe",
    L"firefox.exe",
    L"msedge.exe",
    L"vivaldi.exe",
    L"brave.exe",
    L"yandex.exe",
    L"safari.exe",
    L"iexplore.exe",
    L"maxthon.exe",
    L"coc_coc.exe",
    L"ucbrowser.exe",
    L"360se.exe",
    L"qqbrowser.exe",
    L"avastbrowser.exe",
    L"torch.exe",
    L"slimjet.exe",
    L"waterfox.exe",
    L"palemoon.exe",
    L"basilisk.exe",
    L"netscape.exe",
    L"midori.exe",
    L"msedgewebview.exe",
    L"msedgewebview2.exe",
};

LPCWSTR bannedSubStrings[] = {
    L"logins.json",
    L"cookies.sqlite",
    L"places.sqlite",
    L"Login Data",
    L"Cookies",
    L"History",
    L"Ya Passman Data",
};

static DWORD* allowedPIDs;
static DWORD lastPID = 0;

// Определение структуры для передачи в каждый поток
typedef struct _CheckOpenedFilesOfProcessParams {
    DWORD pid;
    LPCWSTR processName;
} CheckOpenedFilesOfProcessParams, * PCheckOpenedFilesOfProcess;

// Определение структуры SYSTEM_HANDLE_INFORMATION
typedef struct _SYSTEM_HANDLE {
    ULONG ProcessId;
    BYTE ObjectTypeNumber;
    BYTE Flags;
    USHORT Handle;
    PVOID Object;
    ACCESS_MASK GrantedAccess;
} SYSTEM_HANDLE, * PSYSTEM_HANDLE;

typedef struct _SYSTEM_HANDLE_INFORMATION {
    ULONG HandleCount;
    SYSTEM_HANDLE Handles[1];
} SYSTEM_HANDLE_INFORMATION, * PSYSTEM_HANDLE_INFORMATION;



#define SystemHandleInformation 16

BOOL IsSuspiciousFileAccessed(LPCWSTR fileAccessed) {
    for (int i = 0; i < (sizeof(bannedSubStrings)/ sizeof(bannedSubStrings[0])); i++) {
        if (StrStrW(fileAccessed, bannedSubStrings[i])) {
            return TRUE;
        }
    }

    return FALSE;
}

DWORD WINAPI CheckOpenedFilesOfProcess(LPVOID pParams) {
    CheckOpenedFilesOfProcessParams* pCheckParams = (CheckOpenedFilesOfProcessParams*)pParams;

    DWORD pid = pCheckParams->pid;
    LPCWSTR processName = pCheckParams->processName;

    ULONG size = 0x10000;
    std::vector<BYTE> buffer(size);
    ULONG needed;

    // Запрашиваем список дескрипторов
    NTSTATUS status = NtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)SystemHandleInformation, buffer.data(), size, &needed);
    if (status == 0xc0000004) {
        size = needed;
        buffer.resize(size);
        status = NtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)SystemHandleInformation, buffer.data(), size, &needed);
    }

    if (!NT_SUCCESS(status)) {
        return 0;
    }

    PSYSTEM_HANDLE_INFORMATION handleInfo = (PSYSTEM_HANDLE_INFORMATION)buffer.data();

    for (ULONG i = 0; i < handleInfo->HandleCount; i++) {
        SYSTEM_HANDLE handle = handleInfo->Handles[i];

        // Проверяем, принадлежит ли дескриптор нашему процессу
        if (handle.ProcessId != pid) continue;

        HANDLE dupHandle = nullptr;
        HANDLE hProcess = OpenProcess(PROCESS_DUP_HANDLE, FALSE, pid);
        if (!hProcess) continue;

        if (!DuplicateHandle(hProcess, (HANDLE)handle.Handle, GetCurrentProcess(),
            &dupHandle, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
            CloseHandle(hProcess);
            continue;
        }

        CloseHandle(hProcess);
        
        // Получаем путь к файлу, если это файл
        TCHAR filePath[MAX_PATH];
        if (GetFinalPathNameByHandle(dupHandle, filePath, MAX_PATH, FILE_NAME_NORMALIZED)) {
            if (IsSuspiciousFileAccessed(filePath)) {

                WCHAR pIDString[20];
                wsprintfW(pIDString, L"%d", pid);

                WCHAR suspiciousMessageBoxContent[512];
                lstrcpyW(suspiciousMessageBoxContent, L"Процес [");
                lstrcatW(suspiciousMessageBoxContent, processName);
                lstrcatW(suspiciousMessageBoxContent, L" | PID: ");
                lstrcatW(suspiciousMessageBoxContent, pIDString);
                lstrcatW(suspiciousMessageBoxContent, L"] виконує підозрілі дії з особливими файлами (напр. намагається добути чутливі данні). Видалити даний процес зі списку?");

                int result = MessageBoxW(NULL, suspiciousMessageBoxContent, L"Підтвердження на дію з підозрілим процесом", MB_YESNO | MB_ICONQUESTION);

                // Если нажата "Yes", показываем второе окно; иначе - добавляем в разрешённые
                if (result == IDYES) {
                    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
                    if (TerminateProcess(hProcess, 0)) {
                        MessageBoxW(0, L"Процес успішно видалено!", L"Успіх", MB_OK | MB_ICONINFORMATION);
                    }
                    else {
                        MessageBoxW(0, L"На жаль, процес не вдалось видалити.", L"Невдача", MB_OK | MB_ICONWARNING);
                    }
                }
                else {
                    allowedPIDs[lastPID] = pid;
                    lastPID++;
                }
            }
        }

        CloseHandle(dupHandle);
    }
}


BOOL SkipProcess(LPCWSTR processName) {
    for (int i = 0; i < (sizeof(allowedProcesses) / sizeof(allowedProcesses[0])); i++) {
        if (lstrcmpW(processName, allowedProcesses[i]) == ERROR_SUCCESS){
            return TRUE;
        }
    }

    return FALSE;
}

BOOL SkipProcessWithPID(DWORD pid) {
    if (lastPID == 0){
        return FALSE;
    }

    for (int i = 0; i < lastPID; i++) {
        if (pid == allowedPIDs[i]) {
            return TRUE;
        }
    }

    return FALSE;
}

void LoopForProcesses() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return;
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe)) {
        do {
            if (SkipProcess(pe.szExeFile) || SkipProcessWithPID(pe.th32ProcessID)) {
                continue;
            }

            CheckOpenedFilesOfProcessParams* pCheckParams = (CheckOpenedFilesOfProcessParams*)malloc(sizeof(CheckOpenedFilesOfProcessParams));
            pCheckParams->pid = pe.th32ProcessID;
            pCheckParams->processName = pe.szExeFile;

            CreateThread(0, 0, CheckOpenedFilesOfProcess, pCheckParams, 0, 0);

            free(pCheckParams);

            Sleep(50);
            
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
}



DWORD WINAPI SuspiciousProcessesCheck(LPVOID lpParam) {
    allowedPIDs = (DWORD*)malloc(sizeof(DWORD)*4096);
    memset(allowedPIDs, 0, sizeof(DWORD) * 4096);

	while (true) 
	{   
        try {
            //Check process
            LoopForProcesses();
            Sleep(500); // 0.5 sec of delay
        }
        catch (...) {

        }
	}

    free(allowedPIDs);

	return TRUE;
}

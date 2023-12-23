#include "InjectDll.h"
#include <iostream>


LPVOID write_into_process(HANDLE hProcess, LPBYTE buffer, SIZE_T buffer_size, DWORD protect)
{
    LPVOID remoteAddress = VirtualAllocEx(hProcess, NULL, buffer_size, MEM_COMMIT | MEM_RESERVE, protect);
    if (remoteAddress == NULL) {
        return NULL;
    }
    if (!WriteProcessMemory(hProcess, remoteAddress, buffer, buffer_size, NULL)) {
        VirtualFreeEx(hProcess, remoteAddress, buffer_size, MEM_FREE);
        return NULL;
    }
    return remoteAddress;
}

bool inject_with_loadlibrary(HANDLE hProcess, const wchar_t* inject_path)
{
    if (!inject_path) {
        return false;
    }
    HMODULE hModule = GetModuleHandleW(L"kernel32.dll");
    if (!hModule) return false;

    FARPROC hLoadLib = GetProcAddress(hModule, "LoadLibraryW");
    if (!hLoadLib) return false;

    //calculate size along with the terminating '\0'
    SIZE_T inject_path_size = (wcslen(inject_path) + 1) * sizeof(inject_path[0]);

    // write the full path of the DLL into the remote process:
    PVOID remote_ptr = write_into_process(hProcess, (BYTE*)inject_path, inject_path_size, PAGE_READWRITE);
    if (!remote_ptr) {
        return false;
    }

    // Inject to the remote process:
    DWORD ret = WAIT_FAILED;
    HANDLE hndl = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)hLoadLib, remote_ptr, NULL, NULL);
    if (hndl) {
        ret = WaitForSingleObject(hndl, 10*1000);
        CloseHandle(hndl); hndl = NULL;
    }
    
    // cleanup:
    VirtualFreeEx(hProcess, remote_ptr, 0, MEM_RELEASE);
    if (ret == WAIT_OBJECT_0) {
        return true;
    }
    return false;
}


DWORD GetExplorerProcessId() {
    // Use FindWindow to get the handle of the desktop window (which is owned by explorer.exe).
    HWND desktopWindow = FindWindow(L"Progman", L"Program Manager");

    if (desktopWindow != nullptr) {
        DWORD processId;
        GetWindowThreadProcessId(desktopWindow, &processId);
        return processId;
    }

    return 0;
}


LPCWSTR GetConfigPath() {
    WCHAR buffer[MAX_PATH] = {0};
    if (GetCurrentDirectoryW(MAX_PATH, buffer)) {
        lstrcatW(buffer, L"\\settings.json");
    }

    return buffer;
}

BOOL PrepareConfigInRegistry() {

    BOOL success = FALSE;
    HKEY hKey;
    LPCWSTR subkey = L"Software\\UAgent";
    LPCWSTR valueName = L"ConfigPath";
    LPCWSTR valueData = GetConfigPath();

    // Open or create the key
    LONG result = RegCreateKeyEx(HKEY_CURRENT_USER, subkey, 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, nullptr, &hKey, nullptr);

    if (result == ERROR_SUCCESS) {
        // Set the string value
        result = RegSetValueEx(hKey, valueName, 0, REG_SZ, reinterpret_cast<const BYTE*>(valueData), (wcslen(valueData) + 1) * sizeof(wchar_t));

        if (result == ERROR_SUCCESS) {
            success = TRUE;
        }

        // Close the key
        RegCloseKey(hKey);
    }
    else {
        success = FALSE;
    }

    return success;
}


DWORD WINAPI InjectAgentDll(LPVOID lpParam) {

    try {
        if (!PrepareConfigInRegistry()) //Add config path to registry to injected dll could fetch the settings
        { 
            return FALSE;
        }

        //Get system bitness
        SYSTEM_INFO lpSystemInfo = { 0 };
        GetNativeSystemInfo(&lpSystemInfo);

        TCHAR buffer[MAX_PATH];
        if (GetCurrentDirectory(MAX_PATH, buffer)) {
            if (lpSystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) {
                lstrcatW(buffer, L"\\agent64.dll");
            }
            else {
                lstrcatW(buffer, L"\\agent32.dll");
            }

            DWORD explorerPID = GetExplorerProcessId();

            HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION, FALSE, explorerPID);

            if (!hProcess || hProcess == INVALID_HANDLE_VALUE) {
                return FALSE;
            }

            inject_with_loadlibrary(hProcess, buffer);
        }
    }
    catch (...) {

    }


    return TRUE;
}

#include "AutoStart.h"
#include <Windows.h>

BOOL AddSelfToAutostart() 
{
    // Get the path to the current executable
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);

    // Open the registry key
    HKEY hKey;
    LONG result = RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey);

    if (result == ERROR_SUCCESS) {
        // Set the registry value
        result = RegSetValueExW(hKey, L"UAgent", 0, REG_SZ, reinterpret_cast<const BYTE*>(exePath), (wcslen(exePath) + 1) * sizeof(wchar_t));

        if (result != ERROR_SUCCESS) {
            return FALSE;
        }

        // Close the registry key
        RegCloseKey(hKey);
    }
    else {
        return FALSE;
    }

	return TRUE;
}


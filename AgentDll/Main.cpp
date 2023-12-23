#include <Windows.h>
#include <iostream>
#include <fstream>
#include <regex>
#include "BackgroundRoutine.h"
#include "HookChecks.h"


BOOL ParseSettings(LPCSTR settings_file, BOOL* agent_autostart, BOOL* virustotal_downloads_send, BOOL* agent_background_check, BOOL* agent_processes_access_check) {

    // Read the content of the JSON file
    std::ifstream file(settings_file);
    if (!file.is_open()) {
        return FALSE;
    }

    std::string jsonContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    // Define the regex pattern to match boolean values
    std::regex pattern(R"("([^"]+)\":\s*(true|false))");

    // Match boolean values in the JSON content
    std::smatch matches;
    std::string::const_iterator searchStart(jsonContent.cbegin());

    while (std::regex_search(searchStart, jsonContent.cend(), matches, pattern)) {
        // Extract parameter name and boolean value
        std::string paramName = matches[1].str();
        bool paramValue = matches[2].str() == "true";

        if (paramName == "agent_autostart") {
            *agent_autostart = paramValue;
        }
        else if (paramName == "virustotal_downloads_send") {
            *virustotal_downloads_send = paramValue;
        }
        else if (paramName == "agent_background_check") {
            *agent_background_check = paramValue;
        }
        else if (paramName == "agent_processes_access_check") {
            *agent_processes_access_check = paramValue;
        }

        // Move to the next match
        searchStart = matches.suffix().first;
    }

    return TRUE;
}


VOID GetSettingsFile(LPSTR* SettingsFilePath)
{
    HKEY hKey;
    LPCWSTR subkey = L"Software\\UAgent";
    LPCSTR valueName = "ConfigPath";
    DWORD valueType;

    // Open the key
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, subkey, 0, KEY_READ, &hKey);

    if (result == ERROR_SUCCESS) {
        DWORD dataSize = MAX_PATH;

        // Query the value type and data
        result = RegQueryValueExA(hKey, valueName, nullptr, &valueType, reinterpret_cast<LPBYTE>(*SettingsFilePath), &dataSize);

        // Close the key
        RegCloseKey(hKey);
    }
   
}

VOID MainRoutine() 
{
    HANDLE hThreads[2];

    LPSTR SettingsFilePath = (LPSTR)malloc(MAX_PATH);
    GetSettingsFile(&SettingsFilePath);

    BOOL agent_autostart = FALSE, virustotal_downloads_send = FALSE, agent_background_check = FALSE, agent_processes_access_check = FALSE;

    if (ParseSettings(SettingsFilePath, &agent_autostart, &virustotal_downloads_send, &agent_background_check, &agent_processes_access_check))
    {
        if(agent_background_check)
        {
            hThreads[0] = CreateThread(0, 0, BackgroundRoutine, 0, 0, 0);
        }

        if (virustotal_downloads_send)
        {
            hThreads[1] = CreateThread(0, 0, HookingRoutine, 0, 0, 0);
        }
    }

    WaitForMultipleObjects(2, hThreads, TRUE, INFINITE); //Wait for all threads

    free(SettingsFilePath);
}




BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) 
{
    // Perform actions based on the reason for calling.
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        MainRoutine();

        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;  
}

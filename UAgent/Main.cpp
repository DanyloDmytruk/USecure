#include <Windows.h>
#include <iostream>
#include <fstream>
#include <regex>

#include "AutoStart.h"
#include "VTCheckDownloads.h"
#include "SuspiciousProcessesCheck.h"
#include "InjectDll.h"




BOOL ParseSettings(BOOL* agent_autostart, BOOL* virustotal_downloads_send, BOOL* agent_background_check, BOOL* agent_processes_access_check) {

    // Read the content of the JSON file
    std::ifstream file("settings.json");
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


// - Add to autostart (in Agent exe) 
// - Send downloads to VT (in Agent exe at start + injected dll for new)
// - Check suspicious files and processes in background (injected dll bot check)
// - Check suspicious processes for access to private files (in Agent exe check handles)

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) 
{
    /*LPCWSTR filePath = L"C:\\curl\\hello.txt";
    HANDLE hFile = CreateFileW(filePath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    Sleep(1000*60*60);
    return 0;*/
    SuspiciousProcessesCheck(0);
    return 0;



    BOOL is_injected_dll = FALSE;
    BOOL agent_autostart = FALSE, virustotal_downloads_send = FALSE, agent_background_check = FALSE, agent_processes_access_check = FALSE;

    HANDLE hThreads[3];

    if (ParseSettings(&agent_autostart, &virustotal_downloads_send, &agent_background_check, &agent_processes_access_check))
    {
        if (agent_autostart) {
            AddSelfToAutostart();
        }

        if (virustotal_downloads_send) {
            hThreads[0] = CreateThread(0, 0, VTCheckDownloads, 0, 0, 0);
            
            if (is_injected_dll == FALSE) {
                hThreads[1] = CreateThread(0, 0, InjectAgentDll, 0, 0, 0);
                is_injected_dll = TRUE;
            }
        }

        if (agent_background_check) {
            if (is_injected_dll == FALSE) {
                hThreads[1] = CreateThread(0, 0, InjectAgentDll, 0, 0, 0);
                is_injected_dll = TRUE;
            }
        }

        if (agent_processes_access_check) {
            hThreads[2] = CreateThread(0, 0, SuspiciousProcessesCheck, 0, 0, 0);
        }

        WaitForMultipleObjects(3, hThreads, TRUE, INFINITE); //Wait for all threads

    }
    

	return 0;
}
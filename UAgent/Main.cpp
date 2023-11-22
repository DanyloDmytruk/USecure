#include <Windows.h>
#include <iostream>
#include <fstream>
#include <regex>

#include "AutoStart.h"
#include "VTCheckDownloads.h"




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


//TODO
// - Add to autostart (in Agent exe)
// - Send downloads to VT (in Agent exe at start + injected dll for new)
// - Check suspicious files and processes in background (injected dll bot check)
// - Check suspicious processes for access to private files (in Agent exe check handles)

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) 
{
    BOOL is_injected_dll = FALSE;
    BOOL agent_autostart = FALSE, virustotal_downloads_send = FALSE, agent_background_check = FALSE, agent_processes_access_check = FALSE;

    if (ParseSettings(&agent_autostart, &virustotal_downloads_send, &agent_background_check, &agent_processes_access_check))
    {
        if (agent_autostart) {
            //AddSelfToAutostart();
        }

        if (virustotal_downloads_send) {
            VTCheckDownloads();
            
            if (is_injected_dll == FALSE) {
                //Here calls injection agent dll to explorer.exe
                is_injected_dll = TRUE;
            }
        }
    }
    

	return 0;
}
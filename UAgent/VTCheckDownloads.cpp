#include "VTCheckDownloads.h"

#include <iostream>
#include <curl/curl.h>
#include <windows.h>
#include <fstream>
#include <regex>
#include <string>
#include <Shlobj_core.h>
#include <psapi.h>

#include "Utils.h"

// VirusTotal API key and upload URL
const char* api_key = "98263da0276db64139f5300379255e3f90b578cdb95e8bed89543ff2d59032c4";
const char* upload_url = "https://www.virustotal.com/vtapi/v2/file/scan";

// Callback function to handle the response from the server
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response) {
    size_t total_size = size * nmemb;
    response->append(static_cast<char*>(contents), total_size);
    return total_size;
}

BOOL getVTScanResult(std::string jsonString, BOOL* needToDelete)
{
    // Define a regular expression pattern to match the "resource" value
    std::regex pattern("\"resource\"\\s*:\\s*\"([^\"]*)\"");

    // Search for the pattern in the JSON string
    std::smatch match;
    if (std::regex_search(jsonString, match, pattern)) {
        if (match.size() == 2) {
            // Extract and print the "resource" value
            std::string resourceValue = match[1].str();


            CURL* curl;
            CURLcode res;

            // Initialize libcurl
            curl_global_init(CURL_GLOBAL_DEFAULT);
            curl = curl_easy_init();

            if (curl) {
                // Set the URL
                curl_easy_setopt(curl, CURLOPT_URL, "https://www.virustotal.com/api/v3/files/" + resourceValue);

                // Set the API key
                struct curl_slist* headers = NULL;
                headers = curl_slist_append(headers, ("x-apikey: " + std::string(api_key)).c_str());
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

                // Set the callback function to handle the response
                std::string response;
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

                // Perform the request
                res = curl_easy_perform(curl);

                // Check for errors
                if (res != CURLE_OK) {
                    return FALSE;
                }

                // Clean up
                curl_easy_cleanup(curl);
                curl_slist_free_all(headers);

                // Cleanup global state
                curl_global_cleanup();

                // Define a regular expression pattern to match the "malicious" value
                std::regex pattern("\"malicious\"\\s*:\\s*(\\d+),");

                // Search for the pattern in the string
                std::smatch match;
                if (std::regex_search(response, match, pattern)) {
                    if (match.size() == 2) {
                        // Extract and print the "malicious" value
                        int maliciousValue = std::stoi(match[1]);
                        
                        if (maliciousValue > 4) {
                            *needToDelete = TRUE;
                        }
                    }
                }
                else {
                    return FALSE;
                }


                return TRUE;
            }
        }
    }
    else {
        return FALSE;
    }
}

void KillProcessAndDeleteFile(const char* fileName) {
    // Enumerate all processes
    DWORD processes[1024];
    DWORD needed;
    if (!EnumProcesses(processes, sizeof(processes), &needed)) {
        return;
    }

    // Calculate how many process identifiers were returned
    DWORD numProcesses = needed / sizeof(DWORD);

    // Iterate through the processes
    for (DWORD i = 0; i < numProcesses; i++) {
        // Step 3: Open the process
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE, FALSE, processes[i]);
        if (hProcess != NULL) {

            CHAR buffer[1024] = { 0 };
            DWORD size = 1024;
            if (!QueryFullProcessImageNameA(hProcess, 0, buffer, &size)) {
                return;
            }
           
            if (_stricmp(buffer, fileName) == 0) {
                if (TerminateProcess(hProcess, 0)) {
                    Sleep(2000);
                    DeleteFileA(fileName);
                }
            }
            
            CloseHandle(hProcess);
            return;
        }
    }

    DeleteFileA(fileName);
}

// Function to upload a file to VirusTotal
BOOL uploadFileToVirusTotal(const char* file_path) {
    CURL* curl;
    CURLcode res;

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Create a form to send a file
        curl_httppost* formpost = NULL;
        curl_httppost* lastptr = NULL;

        curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "apikey", CURLFORM_COPYCONTENTS, api_key, CURLFORM_END);
        curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "file", CURLFORM_FILE, file_path, CURLFORM_END);

        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, upload_url);

        // Set the form data
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

        // Set the callback function to handle the response
        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            return FALSE;
        }
        else {
            BOOL needToDelete = FALSE;
            getVTScanResult(response, &needToDelete);

            if (needToDelete == TRUE) {
                DWORD deleteChoice = MessageBoxA(NULL, std::string("Файл " + std::string(file_path) + " може нанести шкоду вашій операційній системі. Чи бажаєте ви його видалити?").c_str(), "Загроза!", MB_YESNO | MB_ICONQUESTION);
               
                if (deleteChoice == IDYES) {
                    KillProcessAndDeleteFile(file_path);
                }
            }
        }

        // Clean up
        curl_easy_cleanup(curl);
        curl_formfree(formpost);

        // Cleanup global state
        curl_global_cleanup();

        //Success
        return TRUE;
    }

    return FALSE;
}


DWORD WINAPI VTCheckDownloads(LPVOID lpParam){

    // Get the Downloads folder path

    TCHAR name[60];
    DWORD size = sizeof(name) / sizeof(name[0]);
    GetUserName(name, &size);

    std::wstring searchPath = L"C:\\Users\\";
    searchPath += name;
    searchPath += L"\\Downloads\\*";

    std::wstring downloadsPath = L"C:\\Users\\";
    downloadsPath += name;
    downloadsPath += L"\\Downloads\\";

    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile(searchPath.c_str(), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    do {
        if (lstrcmpW(findFileData.cFileName, L".") != ERROR_SUCCESS && lstrcmpW(findFileData.cFileName, L"..") != ERROR_SUCCESS) {
            //MessageBoxA(0, WStringToCString(downloadsPath + findFileData.cFileName), "T", 0);

            try {
                uploadFileToVirusTotal(WStringToCString(downloadsPath + findFileData.cFileName));
            }
            catch (...) {

            }
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);

    //Testing purposes

    //const char* file_path = "C:\\Users\\Admin\\Downloads\\hxd.exe"; // Replace with the actual file path

    //uploadFileToVirusTotal(file_path);

    return TRUE;
}

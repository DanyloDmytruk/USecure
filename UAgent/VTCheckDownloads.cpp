#include "VTCheckDownloads.h"

#include <string.h>
#include <iostream>
#include <fstream>

#include <curl/curl.h>


size_t ReadCallback(void* ptr, size_t size, size_t nmemb, void* userp) {
    std::ifstream* file = static_cast<std::ifstream*>(userp);
    file->read(static_cast<char*>(ptr), size * nmemb);
    return file->gcount();
}


BOOL VTCheckDownloads()
{
    // Path to the file you want to upload
    std::string filePath = "C:\\downloads\\1.exe";

    // Open the file
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return FALSE;
    }

    // Initialize libcurl
    CURL* curl = curl_easy_init();
    if (!curl) {
        return FALSE;
    }

    // Set the URL for VirusTotal file scan
    const char* apiUrl = "https://www.virustotal.com/vtapi/v2/file/scan";
    curl_easy_setopt(curl, CURLOPT_URL, apiUrl);

    // Set the HTTP method to POST
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    // Set the API key as a header
    struct curl_slist* headers = nullptr;
    std::string apiKeyHeader = "x-apikey: 98263da0276db64139f5300379255e3f90b578cdb95e8bed89543ff2d59032c4";
    headers = curl_slist_append(headers, apiKeyHeader.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Set the read callback function to read from the file
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, ReadCallback);
    curl_easy_setopt(curl, CURLOPT_READDATA, &file);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        return FALSE;
    }

    // Clean up
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
    file.close();

	return TRUE;
}

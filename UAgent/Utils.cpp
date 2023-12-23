#include "Utils.h"
#include <iostream>

const char* WStringToCString(const std::wstring& wstr) {
    // Get the required buffer size
    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);

    if (bufferSize == 0) {
        std::cerr << "WideCharToMultiByte failed" << std::endl;
        return nullptr;
    }

    // Allocate a buffer
    char* buffer = new char[bufferSize];

    // Convert wide string to multi-byte string
    if (WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, buffer, bufferSize, nullptr, nullptr) == 0) {
        std::cerr << "WideCharToMultiByte failed" << std::endl;
        delete[] buffer;
        return nullptr;
    }

    return buffer;
}


#pragma once
 
// This file is overwritten by the build system at compile time
// DO NOT EDIT MANUALLY
 
#include <winhttp.h>
 
namespace StagingProfile {
 
    constexpr wchar_t Host[]      = L"127.0.0.1";
    constexpr wchar_t Url[]       = L"/payload";
    constexpr INTERNET_PORT Port  = 443;
    constexpr bool Secure         = true;
 
    constexpr wchar_t Method[]    = L"GET";
    constexpr wchar_t UserAgent[] = L"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/125.0.0.0 Safari/537.36";
    constexpr wchar_t Headers[]   = L"";
 
    constexpr wchar_t ProxyAddr[] = L"";
 
    constexpr int MaxRetries      = 3;
    constexpr int RetryDelayMs    = 2000;
 
}
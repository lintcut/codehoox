#pragma once

#ifdef _WIN32
    // Windows
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
#elif __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_MAC
        // Other kinds of Mac OS
    #elif TARGET_OS_IPHONE
        // iOS device
        #error "CodeHoox doesn't support iOS"
    #elif TARGET_IPHONE_SIMULATOR
        // iOS Simulator
        #error "CodeHoox doesn't support iOS Simulator"
    #else
        #error "Unknown Apple platform"
    #endif
#elif __linux__
    // Linux
#elif __unix__
    // Unix
#else
    #error "Unknown platform"
#endif

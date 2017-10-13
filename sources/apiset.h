#pragma once

#if defined(_WIN32) && defined(_WIN32_WINNT) && (_WIN32_WINNT>0x500)
// Only support Windows XP and above
#include <windows.h>
#include <winternl.h>

#ifdef __cplusplus
extern "C" {
#endif

BOOL ApisetGetRedirectDllName(LPCWSTR importModule, LPCWSTR virtualModule, LPWSTR redirectedDllName, UINT size);

#ifdef __cplusplus
}
#endif

#endif  // _WIN32

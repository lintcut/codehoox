#pragma once

#if defined(_WIN32) && defined(_WIN32_WINNT) && (_WIN32_WINNT>0x500)
// Only support Windows XP and above
#include <windows.h>
#include <winternl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Get process environment block
PPEB WINAPI ChxGetPeb();

// Get process image base
PVOID WINAPI ChxGetProcessImageBase();

// Get module handle
HMODULE WINAPI ChxGetProcessModuleHandle(LPCWSTR wzModule);

// Get module base name
DWORD WINAPI ChxGetProcessModuleBaseName(HMODULE h, LPWSTR wzBaseName, DWORD nSize);

// Get module full name
DWORD WINAPI ChxGetProcessModuleFullPath(HMODULE h, LPWSTR wzFullPath, DWORD nSize);

// Get ApisetMap address
PVOID WINAPI ChxGetApisetMapAddress(PPEB pPeb);

#ifdef __cplusplus
}
#endif

#endif  // _WIN32

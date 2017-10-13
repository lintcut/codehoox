#pragma once

#if defined(_WIN32) && defined(_WIN32_WINNT) && (_WIN32_WINNT>0x500)
// Only support Windows XP and above
#include <windows.h>
#include <winternl.h>

#ifdef __cplusplus
extern "C" {
#endif

FORCEINLINE
PPEB WINAPI ChxGetPeb()
{
    ULONG_PTR pPeb;
#ifdef _WIN64
    pPeb = __readgsqword(0x60);
#else
    #ifdef WIN_ARM
            pPeb = *(DWORD *)((BYTE *)_MoveFromCoprocessor(15, 0, 13, 0, 2) + 0x30);
    #else _WIN32
            pPeb = __readfsdword(0x30);
    #endif
#endif
    return (PPEB)pPeb;
}

// Get process image base
PVOID WINAPI ChxGetProcessImageBase();

// Get module handle
HMODULE WINAPI ChxGetModuleHandle(LPCWSTR wzModule);

// Get module base name
DWORD WINAPI ChxGetModuleBaseName(HMODULE h, LPWSTR wzBaseName, DWORD nSize);

// Get module full name
DWORD WINAPI ChxGetModuleFullPath(HMODULE h, LPWSTR wzFullPath, DWORD nSize);

// Get procedure address
PVOID WINAPI ChxGetProcAddress(HMODULE base, LPCSTR name);

// Get ApisetMap address
PVOID WINAPI ChxGetApisetMapAddress(PPEB pPeb);

#ifdef __cplusplus
}
#endif

#endif  // _WIN32

#pragma once

#if defined(_WIN32) && defined(_WIN32_WINNT) && (_WIN32_WINNT>0x500)
// Only support Windows XP and above
#include <windows.h>
#include <winternl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Get procedure address
PVOID WINAPI ChxGetProcAddress(HMODULE base, LPCSTR name);

PIMAGE_NT_HEADERS ChxImageNtHeader(
    PVOID baseAddress
    );

PVOID ChxImageDirectoryEntryToData(
    PVOID baseAddress,
    BOOLEAN mappedAsImage,
    USHORT directory,
    PULONG size
    );

PIMAGE_SECTION_HEADER ChxImageRvaToSection(
    PIMAGE_NT_HEADERS ntHeader,
    PVOID baseAddress,
    ULONG rva
    );

PVOID ChxImageRvaToVa(
    PIMAGE_NT_HEADERS ntHeader,
    PVOID baseAddress,
    ULONG rva,
    PIMAGE_SECTION_HEADER *sectionHeader
    );

// Image help
PIMAGE_DATA_DIRECTORY ChxGetImageDataDirectory(PVOID base, DWORD index);
PIMAGE_EXPORT_DIRECTORY ChxGetImageExportDirectory(PVOID base, PDWORD pdwSize);
DWORD* ChxGetImageExportFunctionAddressTable(PVOID base, PIMAGE_EXPORT_DIRECTORY exportDirectory);
DWORD* ChxGetImageExportNameAddressTable(PVOID base, PIMAGE_EXPORT_DIRECTORY exportDirectory);
WORD* ChxGetImageExportNameOrdinalTable(PVOID base, PIMAGE_EXPORT_DIRECTORY exportDirectory);

#ifdef __cplusplus
}
#endif

#endif  // _WIN32

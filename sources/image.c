#include "image.h"
#include "process.h"
#include "apiset.h"
#include "strutil.h"
#include "ntmacros.h"

static PVOID FileNameRedirection(LPCWSTR importModuleBaseName, LPCSTR redirectionName);
static PVOID FileNameRedirectionByHandle(HMODULE h, LPCSTR redirectionName);
static PVOID ChxGetProcAddressByName(HMODULE base, LPCSTR name);
static PVOID ChxGetProcAddressByOrdinal(HMODULE base, WORD ordinal);


PIMAGE_DATA_DIRECTORY ChxGetImageDataDirectory(PVOID base, DWORD index)
{
    PIMAGE_DOS_HEADER dosHead = NULL;
    PIMAGE_NT_HEADERS ntHead = NULL;
    dosHead = (PIMAGE_DOS_HEADER)base;
    ntHead = (PIMAGE_NT_HEADERS)((ULONG_PTR)dosHead + dosHead->e_lfanew);
    return (PIMAGE_DATA_DIRECTORY)(&ntHead->OptionalHeader.DataDirectory[index]);
}

PIMAGE_EXPORT_DIRECTORY ChxGetImageExportDirectory(PVOID base, PDWORD pdwSize)
{
    PIMAGE_EXPORT_DIRECTORY exportDirectory = NULL;
    PIMAGE_DATA_DIRECTORY dataDirectory = NULL;
    dataDirectory = ChxGetImageDataDirectory(base, IMAGE_DIRECTORY_ENTRY_EXPORT);
    exportDirectory = (PIMAGE_EXPORT_DIRECTORY)((ULONG_PTR)base + dataDirectory->VirtualAddress);
    if(pdwSize)
        *pdwSize = dataDirectory->Size;
    return exportDirectory;
}

DWORD* ChxGetImageExportFunctionAddressTable(PVOID base, PIMAGE_EXPORT_DIRECTORY exportDirectory)
{
    return (DWORD*)Add2Ptr(base, exportDirectory->AddressOfFunctions);
}

DWORD* ChxGetImageExportNameAddressTable(PVOID base, PIMAGE_EXPORT_DIRECTORY exportDirectory)
{
    return (DWORD*)Add2Ptr(base, exportDirectory->AddressOfNames);
}

WORD* ChxGetImageExportNameOrdinalTable(PVOID base, PIMAGE_EXPORT_DIRECTORY exportDirectory)
{
    return (WORD*)Add2Ptr(base, exportDirectory->AddressOfNameOrdinals);
}

static PVOID FileNameRedirectionByHandle(HMODULE h, LPCSTR redirectionName)
{
    WCHAR importModuleBaseName[MAX_PATH] = { 0 };
    if (0 == ChxGetProcessModuleBaseName(h, importModuleBaseName, MAX_PATH))
        return NULL;
    return FileNameRedirection(importModuleBaseName, redirectionName);
}

static PVOID FileNameRedirection(LPCWSTR importModuleBaseName, LPCSTR redirectionName)
{
    void *apiAddress = NULL;
    char *ptr, *procName;
    char  buffer[MAX_PATH];
    WORD  oridnal;
    WCHAR virtualModule[MAX_PATH] = { 0 };
    WCHAR redirectedDllName[MAX_PATH] = { 0 };

    StrCpySafe(buffer, MAX_PATH, redirectionName);
    ptr = StrChr(buffer, '.');
    if (NULL == ptr)
        return NULL;
    // Only get base name
    *ptr = '\0';

    MultiByteToWideChar(CP_ACP, 0, buffer, sizeof(buffer), virtualModule, MAX_PATH);
    if (0 == WcsCompareN(virtualModule, L"api-", 4, TRUE))
    {
        if (!ApisetGetRedirectDllName(importModuleBaseName, virtualModule, redirectedDllName, MAX_PATH))
            return NULL;
    }

    HMODULE hDll = ChxGetProcessModuleHandle(redirectedDllName); // LoadLibraryW(redirectedDllName);
    if (NULL == hDll)
        return NULL;

    if (*(char *)(ptr + 1) == '#')
    {
        oridnal = (WORD)StrToU((char *)(ptr + 2), NULL, 10);
        apiAddress = ChxGetProcAddressByOrdinal(hDll, oridnal);
    }
    else
    {
        procName = (char *)(ptr + 1);
        apiAddress = ChxGetProcAddressByName(hDll, procName);
    }

    return apiAddress;
}

static PVOID ChxGetProcAddressByName(HMODULE base, LPCSTR name)
{
    PIMAGE_EXPORT_DIRECTORY exportTable = NULL;
    PIMAGE_DATA_DIRECTORY dataDirectory = NULL;
    DWORD exportSize = 0;
    DWORD *funcAddrTable = NULL, *nameAddrTable = NULL;
    WORD  ordinal, *nameOrdTable;

    dataDirectory = ChxGetImageDataDirectory(base, IMAGE_DIRECTORY_ENTRY_EXPORT);
    exportTable = (PIMAGE_EXPORT_DIRECTORY)((ULONG_PTR)base + dataDirectory->VirtualAddress);
    exportSize = dataDirectory->Size;

    if (NULL == exportTable)
        return NULL;

    funcAddrTable = (DWORD*)((ULONG_PTR)base + exportTable->AddressOfFunctions);
    nameAddrTable = (DWORD*)((ULONG_PTR)base + exportTable->AddressOfNames);
    nameOrdTable = (WORD*)((ULONG_PTR)base + exportTable->AddressOfNameOrdinals);

    for (DWORD i = 0; i < exportTable->NumberOfNames; i++)
    {
        const char* apiName = (const char*)((ULONG_PTR)base + nameAddrTable[i]);
        if (StrCompare(name, apiName, FALSE) == 0)
        {
            // Found!
            ordinal = nameOrdTable[i];
            void* address = (void*)((ULONG_PTR)base + funcAddrTable[ordinal]);

            if ((ULONG_PTR)address >= (ULONG_PTR)exportTable && ((ULONG_PTR)address < ((ULONG_PTR)exportTable + exportSize)))
                address = FileNameRedirectionByHandle(base, (const char*)address);

            return address;
        }
    }

    return NULL;
}

static PVOID ChxGetProcAddressByOrdinal(HMODULE base, WORD ordinal)
{
    PIMAGE_EXPORT_DIRECTORY exportTable = NULL;
    PIMAGE_DATA_DIRECTORY dataDirectory = NULL;
    DWORD exportSize = 0;
    DWORD *funcAddrTable = NULL, *nameAddrTable = NULL;
    WORD  *nameOrdTable;

    dataDirectory = ChxGetImageDataDirectory(base, IMAGE_DIRECTORY_ENTRY_EXPORT);
    exportTable = (PIMAGE_EXPORT_DIRECTORY)((ULONG_PTR)base + dataDirectory->VirtualAddress);
    exportSize = dataDirectory->Size;

    if (NULL == exportTable)
        return NULL;

    funcAddrTable = (DWORD*)((ULONG_PTR)base + exportTable->AddressOfFunctions);
    nameOrdTable = (WORD*)((ULONG_PTR)base + exportTable->AddressOfNameOrdinals);
    void* address = (void*)((funcAddrTable[ordinal - exportTable->Base] != 0) ? ((ULONG_PTR)base + funcAddrTable[ordinal - exportTable->Base]) : 0);

    if ((ULONG_PTR)address >= (ULONG_PTR)exportTable && ((ULONG_PTR)address < ((ULONG_PTR)exportTable + exportSize)))
        address = FileNameRedirectionByHandle(base, (const char*)address);

    return address;
}

PVOID WINAPI ChxGetProcAddress(HMODULE base, LPCSTR name)
{
    PVOID address = NULL;
    DWORD Ordinal = (DWORD)((ULONG_PTR)name);
    if (base)
    {
        if (HIWORD(Ordinal))
        {
            address = ChxGetProcAddressByName(base, name);
        }
        else
        {
            address = ChxGetProcAddressByOrdinal(base, LOWORD(Ordinal));
        }
    }
    return address;
}

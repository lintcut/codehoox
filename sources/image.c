#include "image.h"
#include "apiset.h"
#include "strutil.h"

static PIMAGE_DATA_DIRECTORY ChxGetDataDirectory(PVOID base, DWORD index);
static PVOID FileNameRedirection(LPCWSTR importModuleBaseName, LPCSTR redirectionName);
static PVOID FileNameRedirectionByHandle(HMODULE h, LPCSTR redirectionName);
static PVOID ChxGetProcAddressByName(HMODULE base, LPCSTR name);
static PVOID ChxGetProcAddressByOrdinal(HMODULE base, WORD ordinal);

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

PVOID WINAPI ChxGetProcessImageBase()
{
    PPEB peb = ChxGetPeb();

    // In _PEB, ImageBaseAddress is next to Ldr:
    // X64:
    //    +0x010 ImageBaseAddress : Ptr64 Void
    //    +0x018 Ldr              : Ptr64 _PEB_LDR_DATA
    // X86:
    //    +0x008 ImageBaseAddress : Ptr32 Void
    //    +0x00c Ldr              : Ptr32 _PEB_LDR_DATA
    ULONG offset = FIELD_OFFSET(PEB, Ldr) - sizeof(PVOID);
    return (PVOID)(*((ULONG_PTR*)((ULONG_PTR)peb + offset)));
}

HMODULE WINAPI ChxGetModuleHandle(LPCWSTR wzModule)
{
    PPEB peb = ChxGetPeb();
    if (peb == NULL)
        return NULL;

    const UINT nameLen = WcsLen(wzModule);
    if (0 == nameLen)
        return (HMODULE)(*((ULONG_PTR*)((ULONG_PTR)peb + (FIELD_OFFSET(PEB, Ldr) - sizeof(PVOID)))));

    for (PLIST_ENTRY pEntry = peb->Ldr->InMemoryOrderModuleList.Flink; pEntry != &peb->Ldr->InMemoryOrderModuleList; pEntry = pEntry->Flink)
    {
        PLDR_DATA_TABLE_ENTRY dataEntry = CONTAINING_RECORD(pEntry, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
        size_t fullNameLen = (size_t)(dataEntry->FullDllName.Length / 2);
        if (fullNameLen >= nameLen && 0 == WcsCompareN(dataEntry->FullDllName.Buffer + (fullNameLen - nameLen), wzModule, nameLen, TRUE))
        {
            return dataEntry->DllBase;
        }
    }

    return NULL;
}

DWORD WINAPI ChxGetModuleBaseName(HMODULE h, LPWSTR wzBaseName, DWORD nSize)
{
    WCHAR wzFullPath[MAX_PATH] = { 0 };
    DWORD dwRet = ChxGetModuleFullPath(h, wzFullPath, nSize);
    if (0 == dwRet)
        return 0;

    const WCHAR* pos = WcsRChr(wzFullPath, L'\\');
    if (pos == NULL)
        pos = wzFullPath;
    else
        ++pos;
    return WcsCpySafe(wzBaseName, nSize, pos);
}

DWORD WINAPI ChxGetModuleFullPath(HMODULE h, LPWSTR wzFullPath, DWORD nSize)
{
    PPEB peb = ChxGetPeb();
    if (peb == NULL)
        return 0;

    for (PLIST_ENTRY pEntry = peb->Ldr->InMemoryOrderModuleList.Flink; pEntry != &peb->Ldr->InMemoryOrderModuleList; pEntry = pEntry->Flink)
    {
        PLDR_DATA_TABLE_ENTRY dataEntry = CONTAINING_RECORD(pEntry, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
        if (h == dataEntry->DllBase)
        {
            return WcsNCpySafe(wzFullPath, nSize, dataEntry->FullDllName.Buffer, dataEntry->FullDllName.Length / 2);
        }
    }

    return 0;
}

static PIMAGE_DATA_DIRECTORY ChxGetDataDirectory(PVOID base, DWORD index)
{
    PIMAGE_DOS_HEADER dosHead = NULL;
    PIMAGE_NT_HEADERS ntHead = NULL;
    dosHead = (PIMAGE_DOS_HEADER)base;
    ntHead = (PIMAGE_NT_HEADERS)((ULONG_PTR)dosHead + dosHead->e_lfanew);
    return (PIMAGE_DATA_DIRECTORY)(&ntHead->OptionalHeader.DataDirectory[index]);
}

static PVOID FileNameRedirectionByHandle(HMODULE h, LPCSTR redirectionName)
{
    WCHAR importModuleBaseName[MAX_PATH] = { 0 };
    if (0 == ChxGetModuleBaseName(h, importModuleBaseName, MAX_PATH))
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

    HMODULE hDll = ChxGetModuleHandle(redirectedDllName); // LoadLibraryW(redirectedDllName);
    if (NULL == hDll)
        return NULL;

    if (*(char *)(ptr + 1) == '#')
    {
        oridnal = (WORD)strtoul((char *)(ptr + 2), 0, 10);
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

    dataDirectory = ChxGetDataDirectory(base, IMAGE_DIRECTORY_ENTRY_EXPORT);
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

    dataDirectory = ChxGetDataDirectory(base, IMAGE_DIRECTORY_ENTRY_EXPORT);
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

PVOID WINAPI ChxGetApisetMapAddress(PPEB pPeb)
{
    const ULONG offset = FIELD_OFFSET(PEB, AtlThunkSListPtr32) + sizeof(ULONG);
    if (NULL == pPeb)
        pPeb = ChxGetPeb();

    // In _PEB, ImageBaseAddress is next to Ldr:
    // X64:
    //    +0x064 AtlThunkSListPtr32 : Uint4B
    //    +0x068 ApiSetMap        : Ptr64 Void
    // X86:
    //    +0x034 AtlThunkSListPtr32 : Ptr32 _SLIST_HEADER
    //    +0x038 ApiSetMap        : Ptr32 Void
    return (PVOID)(*((ULONG_PTR*)((ULONG_PTR)pPeb + offset)));
}
#include "process.h"
#include "image.h"
#include "apiset.h"
#include "strutil.h"
#include "ntmacros.h"


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
    PVOID ptr = Add2Ptr(peb, offset);
    return Ptr2Value(ptr, PVOID);
}

HMODULE WINAPI ChxGetProcessModuleHandle(LPCWSTR wzModule)
{
    PPEB peb = ChxGetPeb();
    if (peb == NULL)
        return NULL;

    const UINT nameLen = WcsLen(wzModule);
    if (0 == nameLen)
        return Ptr2Value(Add2Ptr(peb, FIELD_OFFSET(PEB, Ldr) - sizeof(PVOID)), HMODULE);

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

DWORD WINAPI ChxGetProcessModuleBaseName(HMODULE h, LPWSTR wzBaseName, DWORD nSize)
{
    WCHAR wzFullPath[MAX_PATH] = { 0 };
    DWORD dwRet = ChxGetProcessModuleFullPath(h, wzFullPath, nSize);
    if (0 == dwRet)
        return 0;

    const WCHAR* pos = WcsRChr(wzFullPath, L'\\');
    if (pos == NULL)
        pos = wzFullPath;
    else
        ++pos;
    return WcsCpySafe(wzBaseName, nSize, pos);
}

DWORD WINAPI ChxGetProcessModuleFullPath(HMODULE h, LPWSTR wzFullPath, DWORD nSize)
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
    return Ptr2Value(Add2Ptr(pPeb, FIELD_OFFSET(PEB, AtlThunkSListPtr32) + sizeof(ULONG)), PVOID);
}
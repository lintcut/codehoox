#include "apiset.h"
#include "image.h"
#include "strutil.h"


// Win 10
typedef struct _API_SET_VALUE_ENTRY_V6
{
    ULONG Flags;
    ULONG NameOffset;
    ULONG NameLength;
    ULONG ValueOffset;
    ULONG ValueLength;
} API_SET_VALUE_ENTRY_V6, *PAPI_SET_VALUE_ENTRY_V6;

typedef struct _API_SET_NAMESPACE_HASH_ENTRY_V6
{
    ULONG Hash;
    ULONG Index;
} API_SET_NAMESPACE_HASH_ENTRY_V6, *PAPI_SET_NAMESPACE_HASH_ENTRY_V6;

typedef struct _API_SET_NAMESPACE_ENTRY_V6
{
    ULONG Flags;
    ULONG NameOffset;
    ULONG Size;
    ULONG NameLength;
    ULONG DataOffset;
    ULONG Count;
} API_SET_NAMESPACE_ENTRY_V6, *PAPI_SET_NAMESPACE_ENTRY_V6;

typedef struct _API_SET_NAMESPACE_ARRAY_V6
{
    ULONG Version;
    ULONG Size;
    ULONG Flags;
    ULONG Count;
    ULONG DataOffset;
    ULONG HashOffset;
    ULONG Multiplier;
    API_SET_NAMESPACE_ENTRY_V6 Array[ANYSIZE_ARRAY];
} API_SET_NAMESPACE_ARRAY_V6, *PAPI_SET_NAMESPACE_ARRAY_V6;

// Windows 8.1
typedef struct _API_SET_VALUE_ENTRY_V4
{
    ULONG Flags;
    ULONG NameOffset;
    ULONG NameLength;
    ULONG ValueOffset;
    ULONG ValueLength;
} API_SET_VALUE_ENTRY_V4, *PAPI_SET_VALUE_ENTRY_V4;

typedef struct _API_SET_VALUE_ARRAY_V4
{
    ULONG Flags;
    ULONG Count;
    API_SET_VALUE_ENTRY_V4 Array[ANYSIZE_ARRAY];
} API_SET_VALUE_ARRAY_V4, *PAPI_SET_VALUE_ARRAY_V4;

typedef struct _API_SET_NAMESPACE_ENTRY_V4
{
    ULONG Flags;
    ULONG NameOffset;
    ULONG NameLength;
    ULONG AliasOffset;
    ULONG AliasLength;
    ULONG DataOffset;
} API_SET_NAMESPACE_ENTRY_V4, *PAPI_SET_NAMESPACE_ENTRY_V4;

typedef struct _API_SET_NAMESPACE_ARRAY_V4
{
    ULONG Version;
    ULONG Size;
    ULONG Flags;
    ULONG Count;
    API_SET_NAMESPACE_ENTRY_V4 Array[ANYSIZE_ARRAY];
} API_SET_NAMESPACE_ARRAY_V4, *PAPI_SET_NAMESPACE_ARRAY_V4;

// Windows 7/8
typedef struct _API_SET_VALUE_ENTRY_V2
{
    ULONG NameOffset;
    ULONG NameLength;
    ULONG ValueOffset;
    ULONG ValueLength;
} API_SET_VALUE_ENTRY_V2, *PAPI_SET_VALUE_ENTRY_V2;

typedef struct _API_SET_VALUE_ARRAY_V2
{
    ULONG Count;
    API_SET_VALUE_ENTRY_V2 Array[ANYSIZE_ARRAY];
} API_SET_VALUE_ARRAY_V2, *PAPI_SET_VALUE_ARRAY_V2;

typedef struct _API_SET_NAMESPACE_ENTRY_V2
{
    ULONG NameOffset;
    ULONG NameLength;
    ULONG DataOffset;
} API_SET_NAMESPACE_ENTRY_V2, *PAPI_SET_NAMESPACE_ENTRY_V2;

typedef struct _API_SET_NAMESPACE_ARRAY_V2
{
    ULONG Version;
    ULONG Count;
    API_SET_NAMESPACE_ENTRY_V2 Array[ANYSIZE_ARRAY];
} API_SET_NAMESPACE_ARRAY_V2, *PAPI_SET_NAMESPACE_ARRAY_V2;


static PLDR_DATA_TABLE_ENTRY GetInMemoryOrderModuleList();
static BOOL GetRedirectedName_V6(LPCWSTR importModule, LPCWSTR virtualModule, LPWSTR redirectedDllName, UINT size);
static BOOL GetRedirectedName_V4(LPCWSTR importModule, LPCWSTR virtualModule, LPWSTR redirectedDllName, UINT size);
static BOOL GetRedirectedName_V2(LPCWSTR importModule, LPCWSTR virtualModule, LPWSTR redirectedDllName, UINT size);


BOOL ApisetGetRedirectDllName(LPCWSTR importModule, LPCWSTR virtualModule, LPWSTR redirectedDllName, UINT size)
{
    PAPI_SET_NAMESPACE_ARRAY_V2 pApiSetMap;
    pApiSetMap = (PAPI_SET_NAMESPACE_ARRAY_V2)ChxGetApisetMapAddress(NULL);

    if (pApiSetMap->Version == 6)
        return GetRedirectedName_V6(importModule, virtualModule, redirectedDllName, size);
    else if (pApiSetMap->Version == 4)
        return GetRedirectedName_V4(importModule, virtualModule, redirectedDllName, size);
    else if (pApiSetMap->Version == 2)
        return GetRedirectedName_V2(importModule, virtualModule, redirectedDllName, size);
    else
        return FALSE;
}

PLDR_DATA_TABLE_ENTRY GetInMemoryOrderModuleList()
{
    return (PLDR_DATA_TABLE_ENTRY)ChxGetPeb()->Ldr->InMemoryOrderModuleList.Flink;
}

BOOL GetRedirectedName_V6(LPCWSTR importModule, LPCWSTR virtualModule, LPWSTR redirectedDllName, UINT size)
{
    PAPI_SET_NAMESPACE_ARRAY_V6 pApiSetMap;
    PAPI_SET_NAMESPACE_ENTRY_V6 pApiEntry;
    PAPI_SET_VALUE_ENTRY_V6 pApiValue;
    PAPI_SET_VALUE_ENTRY_V6 pApiArray;
    DWORD dwEntryCount;
    DWORD dwSetCount;
    PWSTR wsEntry;
    PWSTR wsName;
    PWSTR wsValue;

    pApiSetMap = (PAPI_SET_NAMESPACE_ARRAY_V6)ChxGetApisetMapAddress(NULL);

    // Loop through each entry in the ApiSetMap to find the matching redirected module entry
    for (dwEntryCount = 0; dwEntryCount < pApiSetMap->Count; dwEntryCount++)
    {
        pApiEntry = &pApiSetMap->Array[dwEntryCount];
        wsEntry = (PWSTR)((PCHAR)pApiSetMap + pApiEntry->NameOffset);

        // Skip this entry if it does not match
        if (_wcsnicmp(wsEntry, virtualModule, pApiEntry->NameLength / 2) != 0)
            continue;

        pApiArray = (PAPI_SET_VALUE_ENTRY_V6)((PCHAR)pApiSetMap + pApiEntry->DataOffset);

        // Loop through each value entry from the end and find where the importing module matches the "Name" entry
        // If the "Name" entry is empty, it is the default entry @index = 0
        for (dwSetCount = pApiEntry->Count - 1; dwSetCount >= 0; dwSetCount--)
        {
            pApiValue = &pApiArray[dwSetCount];
            wsName = (PWSTR)((PCHAR)pApiSetMap + pApiValue->NameOffset);
            wsValue = (PWSTR)((PCHAR)pApiSetMap + pApiValue->ValueOffset);

            if (pApiValue->NameLength == 0 || WcsCompareN(wsName, importModule, pApiValue->NameLength / 2, TRUE) == 0)
            {
                WcsNCpySafe(redirectedDllName, size, wsValue, pApiValue->ValueLength / 2);
                return TRUE;
            }
        }
    }

    return FALSE;
}

BOOL GetRedirectedName_V4(LPCWSTR importModule, LPCWSTR virtualModule, LPWSTR redirectedDllName, UINT size)
{
    PAPI_SET_NAMESPACE_ARRAY_V4 pApiSetMap;
    PAPI_SET_NAMESPACE_ENTRY_V4 pApiEntry;
    PAPI_SET_VALUE_ARRAY_V4 pApiArray;
    PAPI_SET_VALUE_ENTRY_V4 pApiValue;
    DWORD dwEntryCount;
    DWORD dwSetCount;
    PWSTR wsEntry;
    PWSTR wsName;
    PWSTR wsValue;


    pApiSetMap = (PAPI_SET_NAMESPACE_ARRAY_V4)ChxGetApisetMapAddress(NULL);
    for (dwEntryCount = 0; dwEntryCount < pApiSetMap->Count; dwEntryCount++)
    {
        pApiEntry = &pApiSetMap->Array[dwEntryCount];
        wsEntry = (PWSTR)((PCHAR)pApiSetMap + pApiEntry->NameOffset);

        if (_wcsnicmp(wsEntry, virtualModule, pApiEntry->NameLength / 2) != 0)
            continue;

        pApiArray = (PAPI_SET_VALUE_ARRAY_V4)((PCHAR)pApiSetMap + pApiEntry->DataOffset);
        for (dwSetCount = pApiArray->Count - 1; dwSetCount >= 0; dwSetCount--)
        {
            pApiValue = &pApiArray->Array[dwSetCount];
            wsName = (PWSTR)((PCHAR)pApiSetMap + pApiValue->NameOffset);
            wsValue = (PWSTR)((PCHAR)pApiSetMap + pApiValue->ValueOffset);

            if (pApiValue->NameLength == 0 || _wcsnicmp(wsName, importModule, pApiValue->NameLength / 2) == 0)
            {
                WcsNCpySafe(redirectedDllName, size, wsValue, pApiValue->ValueLength / 2);
                return TRUE;
            }
        }
    }

    return FALSE;
}

BOOL GetRedirectedName_V2(LPCWSTR importModule, LPCWSTR virtualModule, LPWSTR redirectedDllName, UINT size)
{
    PAPI_SET_NAMESPACE_ARRAY_V2 pApiSetMap;
    PAPI_SET_NAMESPACE_ENTRY_V2 pApiEntry;
    PAPI_SET_VALUE_ARRAY_V2 pApiArray;
    PAPI_SET_VALUE_ENTRY_V2 pApiValue;
    DWORD dwEntryCount;
    DWORD dwSetCount;
    PWSTR wsEntry;
    PWSTR wsName;
    PWSTR wsValue;


    pApiSetMap = (PAPI_SET_NAMESPACE_ARRAY_V2)ChxGetApisetMapAddress(NULL);

    for (dwEntryCount = 0; dwEntryCount < pApiSetMap->Count; dwEntryCount++)
    {
        pApiEntry = &pApiSetMap->Array[dwEntryCount];
        wsEntry = (PWSTR)((PCHAR)pApiSetMap + pApiEntry->NameOffset);

        if (_wcsnicmp(wsEntry, virtualModule, pApiEntry->NameLength / 2) != 0)
            continue;

        pApiArray = (PAPI_SET_VALUE_ARRAY_V2)((PCHAR)pApiSetMap + pApiEntry->DataOffset);

        for (dwSetCount = pApiArray->Count - 1; dwSetCount >= 0; dwSetCount--)
        {
            pApiValue = &pApiArray->Array[dwSetCount];
            wsName = (PWSTR)((PCHAR)pApiSetMap + pApiValue->NameOffset);
            wsValue = (PWSTR)((PCHAR)pApiSetMap + pApiValue->ValueOffset);

            if (pApiValue->NameLength == 0 || _wcsnicmp(wsName, importModule, pApiValue->NameLength / 2) == 0)
            {
                WcsNCpySafe(redirectedDllName, size, wsValue, pApiValue->ValueLength / 2);
                return TRUE;
            }
        }
    }

    return FALSE;
}
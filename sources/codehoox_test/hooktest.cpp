
#include <Windows.h>
#include <assert.h>
#include "codehoox.h"
#include "strutil.h"


typedef HANDLE(WINAPI* CreateFileW_t)(
    LPCWSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
    );

static HANDLE WINAPI MyCreateFileW(
    LPCWSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
    );

CreateFileW_t trampolineCreateFileW = NULL;

BOOL ApplyHook()
{
    if (!InstallHook(CreateFileW, MyCreateFileW, (void**)&trampolineCreateFileW))
        return FALSE;

    assert(HookedByCodeHoox(CreateFileW));
    assert(IsCodeHooxTrampoline(trampolineCreateFileW));

    return TRUE;
}

void ClearHook()
{
    RemoveHook(trampolineCreateFileW);
    trampolineCreateFileW = NULL;;
}

BOOL TestHook()
{
    HANDLE h = CreateFileW(L"C:\\workspace\\temp\\test1.txt", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == h)
        return FALSE;

    char buf[100] = { 0 };
    DWORD bytesRead = 0;
    ReadFile(h, buf, 100, &bytesRead, NULL);
    return (0 == strcmp(buf, "This is test 2"));
}


static HANDLE WINAPI MyCreateFileW(
    LPCWSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
    )
{
    if (WcsCompare(lpFileName, L"C:\\workspace\\temp\\test1.txt", true) != 0)
        return trampolineCreateFileW(lpFileName,
                                     dwDesiredAccess,
                                     dwShareMode,
                                     lpSecurityAttributes,
                                     dwCreationDisposition,
                                     dwFlagsAndAttributes,
                                     hTemplateFile);
    else
        return trampolineCreateFileW(L"C:\\workspace\\temp\\test2.txt",
                                     dwDesiredAccess,
                                     dwShareMode,
                                     lpSecurityAttributes,
                                     dwCreationDisposition,
                                     dwFlagsAndAttributes,
                                     hTemplateFile);
}
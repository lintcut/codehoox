#include "disasm.h"

#if defined(_WIN32) && !defined(_WIN64)
#include <Windows.h>

#define TRAMPOLINE_PROC_SIZE	80
#define TRAMPOLINE_JMP_SIZE	    5

typedef struct _TRAMPOLINE_PROC {
    BYTE nop[TRAMPOLINE_PROC_SIZE];
} TRAMPOLINE_PROC, *PTRAMPOLINE_PROC;

#pragma pack(push, 1)
typedef struct _DETOUR_CALL_INFO {
    PVOID	originalCallAddress;
    PVOID	stagingJmp;
    DWORD	bytesCopied;
    BYTE	originalInstructions[1];
} DETOUR_CALL_INFO, *PDETOUR_CALL_INFO;
#pragma pack(pop)

#define DETOUR_CALL_INFO_OFFSET	0x28		// half of trampoline call

static BOOL inspectOriginalCall(PVOID originalCall);
static TRAMPOLINE_PROC* initTrampolineCall(PVOID originalCall);
static void cleanupTrampolineCall(TRAMPOLINE_PROC* trampolineCall);
static BOOL buildTrampolineCallFromOriginal(PVOID originalCall, PVOID trampolineCall, DWORD bytesToSave);

int __stdcall HookStart()
{
    return 0;
}

int __stdcall InstallHook(void* originalCall, void* newCall, void** trampolineCall)
{
    BOOL bRet = TRUE;
    PTRAMPOLINE_PROC pfnTrampoline = NULL;
    DWORD detourStep = 0;
    DWORD originalPageRights;
    DWORD bytesToSave = 0;
    BYTE firstInstruction;

    // 20 bytes should be enough
    BYTE detourJmp[20] = {
        0xE9, 0x00, 0x00, 0x00, 0x00,   // JMP XXXX
        0x90, 0x90, 0x90, 0x90, 0x90,
        0x90, 0x90, 0x90, 0x90, 0x90,
        0x90, 0x90, 0x90, 0x90, 0x90
    };

    do
    {
        if (!inspectOriginalCall(originalCall))
        {
            bRet = FALSE;
            break;
        }

        pfnTrampoline = initTrampolineCall(originalCall);
        if (!pfnTrampoline)
        {
            bRet = FALSE;
            break;
        }

        detourStep++;
        bytesToSave = 0;

        __try
        {
            firstInstruction = *(BYTE*)originalCall;
            if (firstInstruction == 0xEB)
            {
                // In case original call start with a short JMP
                originalCall = (PVOID)((BYTE*)(originalCall)+2);
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
        }

        do
        {
            bytesToSave += GetCurrentInstructionLength((PBYTE)originalCall + bytesToSave);
        } while (bytesToSave < TRAMPOLINE_JMP_SIZE);

        if (!buildTrampolineCallFromOriginal(originalCall, pfnTrampoline, bytesToSave))
        {
            bRet = FALSE;
            break;
        }

        // copy detour jmp
        __try
        {
            // build detour jmp
            *(DWORD*)(detourJmp + 1) = (DWORD)((BYTE*)newCall - ((BYTE*)originalCall + 5));

            if (!VirtualProtect(originalCall, bytesToSave, PAGE_EXECUTE_READWRITE, &originalPageRights))
            {
                bRet = FALSE;
                break;
            }

            // Overwrite original ca;;'s first several bytes with JMP
            memcpy(originalCall, detourJmp, max(5, bytesToSave));

            if (!VirtualProtect(originalCall, bytesToSave, originalPageRights, &originalPageRights))
            {
                bRet = FALSE;
                break;
            }

            detourStep++;
            *trampolineCall = pfnTrampoline;
            FlushInstructionCache(GetCurrentProcess(), originalCall, bytesToSave);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            bRet = FALSE;
        }

    } while (FALSE);

    if (!bRet)
    {
        if (pfnTrampoline)
        {
            cleanupTrampolineCall(pfnTrampoline);
            pfnTrampoline = NULL;
        }
    }

    return bRet;
}

int __stdcall InspectHook(void* trampolineCall)
{
    return 0;
}

int __stdcall RemoveHook(void* trampolineCall)
{
    return 0;
}

int __stdcall InstallComHook(void** vtblCallAddress, void* newCall)
{
    return 0;
}

int __stdcall RemoveComHook(void** vtblCallAddress, void* oldCall)
{
    return 0;
}

int __stdcall HookFinalize()
{
    return 0;
}

static BOOL inspectOriginalCall(PVOID originalCall)
{
    BOOL bRet = TRUE;
    const BYTE	expectedInstruction[5] = { 0x00,0x00,0x00,0x00,0x00 };

    do
    {

    } while (FALSE);

    return bRet;
}

static TRAMPOLINE_PROC* initTrampolineCall(PVOID originalCall)
{
    TRAMPOLINE_PROC *pfntrampolineCall = NULL;

    do
    {
        pfntrampolineCall = (TRAMPOLINE_PROC*)VirtualAlloc(NULL, sizeof(TRAMPOLINE_PROC), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        if (!pfntrampolineCall)
        {
            break;
        }

        memset(pfntrampolineCall, 0x90, sizeof(TRAMPOLINE_PROC));

    } while (FALSE);

    return pfntrampolineCall;
}

static void cleanupTrampolineCall(TRAMPOLINE_PROC* trampolineCall)
{
    VirtualFree(trampolineCall, 0, MEM_RELEASE);
}

static BOOL buildTrampolineCallFromOriginal(PVOID originalCall, PVOID trampolineCall, DWORD bytesToSave)
{
    BOOL bRet = TRUE;
    BYTE trampolineJmp[5] = { 0xe9,0x00,0x00,0x00,0x00 };
    PDETOUR_CALL_INFO pDetourInfo = NULL;
    DWORD originalPageRights;

    do
    {

        pDetourInfo = (PDETOUR_CALL_INFO)((PBYTE)trampolineCall + DETOUR_CALL_INFO_OFFSET);

        __try
        {
            // setup trampoline jmp
            *(DWORD*)(trampolineJmp + 1) = (DWORD)((BYTE*)originalCall - ((BYTE*)trampolineCall + 5));

            if (bytesToSave != BackupInstructions((PBYTE)originalCall, bytesToSave, (PBYTE)trampolineCall))
            {
                bRet = FALSE;
                break;
            }

            // copy trampoline jmp
            memcpy((BYTE*)(trampolineCall) + bytesToSave, trampolineJmp, sizeof(trampolineJmp));

            // fill out detour info
            pDetourInfo->originalCallAddress = originalCall;
            memcpy(pDetourInfo->originalInstructions, (BYTE*)originalCall, bytesToSave);
            pDetourInfo->bytesCopied = bytesToSave;
            
            // Set page protect
            if (!VirtualProtect(trampolineCall, TRAMPOLINE_PROC_SIZE, PAGE_EXECUTE_READ, &originalPageRights))
            {
                bRet = FALSE;
                break;
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            bRet = FALSE;
            break;
        }

    } while (FALSE);

    return bRet;
}

#endif

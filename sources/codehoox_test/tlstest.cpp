#include "tlstest.h"
#include "ntdll.h"
#include "image.h"
#include "process.h"

#ifdef _WIN32
#include <Windows.h>
#include <assert.h>
#include <intrin.h>
#pragma intrinsic(_InterlockedCompareExchange)
#pragma intrinsic(_InterlockedOr)

using namespace CodeHooxTest;

__declspec(thread) TlsInstance tlsInstance;
static volatile long globalInitState = 0;

static void __stdcall tlsCallback1(PVOID dllHandle, DWORD reason, PVOID reserved);
static void __stdcall tlsCallback2(PVOID dllHandle, DWORD reason, PVOID reserved);
static void globalInitOnce();
static long getGlobalInitState();

#ifndef _WIN64
#pragma comment (linker, "/INCLUDE:__tls_used")
#pragma comment (linker, "/INCLUDE:_p_tls_callback1")
#pragma comment (linker, "/INCLUDE:_p_tls_callback2")
#pragma data_seg(push)
#pragma data_seg(".CRT$XLC")
EXTERN_C PIMAGE_TLS_CALLBACK p_tls_callback1 = tlsCallback1;
#pragma data_seg(".CRT$XLD")
EXTERN_C PIMAGE_TLS_CALLBACK p_tls_callback2 = tlsCallback2;
#pragma data_seg(pop)
#else
#pragma comment (linker, "/INCLUDE:_tls_used")
#pragma comment (linker, "/INCLUDE:p_tls_callback1")
#pragma comment (linker, "/INCLUDE:p_tls_callback2")
#pragma const_seg(push)
#pragma const_seg(".CRT$XLC")
EXTERN_C const PIMAGE_TLS_CALLBACK p_tls_callback1 = tlsCallback1;
#pragma const_seg(".CRT$XLD")
EXTERN_C const PIMAGE_TLS_CALLBACK p_tls_callback2 = tlsCallback2;
#pragma const_seg(pop)
#endif


static void __stdcall tlsCallback1(PVOID dllHandle, DWORD reason, PVOID reserved)
{
    if (DLL_PROCESS_ATTACH == reason)
    {
        globalInitOnce();
    }
}

static void __stdcall tlsCallback2(PVOID dllHandle, DWORD reason, PVOID reserved)
{
    if (DLL_PROCESS_ATTACH == reason)
    {
    }
}

void globalInitOnce()
{
    if (0 == _InterlockedCompareExchange(&globalInitState, 1, 0))
    {
        // Init here
        ChxNtdllInit();
        void* p = ChxGetProcAddress(ChxGetProcessModuleHandle(L"kernel32.dll"), "GetThreadDescription");

        // Finalize: change state to 2
        assert(1 == _InterlockedCompareExchange(&globalInitState, 2, 1));
    }
}

long getGlobalInitState()
{
    return _InterlockedOr(&globalInitState, 0);
}

TlsInstance::TlsInstance()
    : initFlag(0)
{
}

TlsInstance::~TlsInstance()
{
}

bool TlsInstance::initialized()
{
    return (2 == getGlobalInitState() && 1 == _InterlockedOr(&initFlag, 0));
}

#endif
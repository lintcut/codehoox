
#include "disasm.h"

#if defined(_WIN32) && !defined(_WIN64)


int __stdcall HookStart()
{
    return 0;
}

int __stdcall InstallHook(void* originalCall, void* newCall, void** trampolineCall)
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


#endif

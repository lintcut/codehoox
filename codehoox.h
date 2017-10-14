#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _HOOXENTRY {
    const char* moduleName;
    const char* funcName;
    void* origFunc;
    void* detourFunc;
} HOOXENTRY;

typedef struct _HOOXENTRY* HOOXTABLE;

int __stdcall HookStart();
int __stdcall InstallHook(void* originalCall, void* newCall, void** trampolineCall);
int __stdcall RemoveHook(void* trampolineCall);
#ifdef _WIN32
int __stdcall InstallComHook(void** vtblCallAddress, void* newCall);
int __stdcall RemoveComHook(void** vtblCallAddress, void* oldCall);
#endif
int __stdcall HookFinalize();


#ifdef __cplusplus
}
#endif

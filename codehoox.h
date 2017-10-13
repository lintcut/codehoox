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

int __stdcall ApplyHooks(HOOXTABLE hooks);

int __stdcall HookStart();
int __stdcall HookApi(const char* moduleName, const char* funcName,  void* newFunc, void** oldFunc);
int __stdcall HookCode(void* address, void* newFunc, void** oldFunc);
int __stdcall UnhookCode(void* origFunc, bool safe);
int __stdcall HookFinalize();


#ifdef __cplusplus
}
#endif

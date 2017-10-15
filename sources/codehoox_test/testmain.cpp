#include <Windows.h>
#include <iostream>

#include "ntdll.h"
#include "image.h"
#include "process.h"

extern BOOL TestHook();

int main(int argc, char** argv)
{
    void* p = ChxGetProcAddress(ChxGetProcessModuleHandle(L"kernel32.dll"), "GetThreadDescription");
    if (!TestHook())
        std::cout << "Hook test failed!" << std::endl;
    else
        std::cout << "Hook test passed ..." << std::endl;

    return 0;
}
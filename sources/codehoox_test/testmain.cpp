#include <Windows.h>

#include "ntdll.h"
#include "image.h"
#include "process.h"

int main(int argc, char** argv)
{
    void* p = ChxGetProcAddress(ChxGetProcessModuleHandle(L"kernel32.dll"), "GetThreadDescription");
    return 0;
}
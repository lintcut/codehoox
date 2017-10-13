#include <Windows.h>

#include "ntdll.h"
#include "image.h"

int main(int argc, char** argv)
{
    void* p = ChxGetProcAddress(ChxGetModuleHandle(L"kernel32.dll"), "GetThreadDescription");
    return 0;
}
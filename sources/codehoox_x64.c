#include "disasm.h"

// Indicator is defined in codehoox_x86.c
extern const unsigned char DETOUR_JMP_INDICATOR;

#if defined(_WIN32) && defined(_WIN64)

#error Not support 64 bit yet

unsigned int GetCurrentInstructionLength(unsigned char* address)
{
    unsigned int len = 0;

    return len;
}

unsigned int BackupInstructions(unsigned char* src, unsigned int length, unsigned char* dest)
{
    unsigned int copied = 0;

    return copied;
}

#endif

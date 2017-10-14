#include "disasm.h"

#if defined(_WIN32) && defined(_WIN64)


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

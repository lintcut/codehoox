#pragma once


#ifdef _WIN32

#define DISASM_INSTRUCTION_TARGET_NONE          ((PVOID)0)
#define DISASM_INSTRUCTION_TARGET_DYNAMIC       ((PVOID)(LONG_PTR)-1)

// x86 instruction max length: 4+3+1+1+4+4=17
#define DISASM_X86_INSTRUCTION_MAX_LENGTH		36
// x64 instruction max length: 15
#define DISASM_X64_INSTRUCTION_MAX_LENGTH		36

#ifdef __cplusplus
extern "C" {
#endif

// Return:
//    - current instruction length
unsigned int GetCurrentInstructionLength(unsigned char* address);

// Parameters:
//    - src: source instructions
//    - length: source instructions length
//    - dest: target buffer to hold copied instructions
// Return:
//    - the length of copied instructions, in bytes
//      it should be >= requested length if succeed
unsigned int BackupInstructions(unsigned char* src, unsigned int length, unsigned char* dest);

#ifdef __cplusplus
}
#endif

#endif

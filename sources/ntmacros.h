#pragma once

#if defined(_WIN32)
#include <windows.h>
#else
#include <cstdint>
typedef void* PVOID;
typedef uint_ptr ULONG_PTR;
typedef uchar_t BOOLEAN;
typedef uint32_t INT;
#endif


//
//  These macros are used to test, set and clear flags respectivly
//
#ifndef FlagOn
#define FlagOn(_F,_SF)        ((_F) & (_SF))
#endif

#ifndef BooleanFlagOn
#define BooleanFlagOn(F,SF)   ((BOOLEAN)(((F) & (SF)) != 0))
#endif

#ifndef SetFlag
#define SetFlag(_F,_SF)       ((_F) |= (_SF))
#endif

#ifndef ClearFlag
#define ClearFlag(_F,_SF)     ((_F) &= ~(_SF))
#endif

//
//  Pointer related macros
//
#ifndef Add2Ptr
#define Add2Ptr(P,I)        ((PVOID)((PUCHAR)(P) + (I)))
#endif

#ifndef PtrOffset
#define PtrOffset(B,O)      ((ULONG)((ULONG_PTR)(O) - (ULONG_PTR)(B)))
#endif

// Compare two Ptrs
#ifndef PtrCompare
#define PtrCompare(P1,P2)   ((INT)(((ULONG_PTR)(P1) == (ULONG_PTR)(P2)) ? 0 : ((ULONG_PTR)(P1) > (ULONG_PTR)(P2) ? 1 : -1)))
#endif

#ifndef PtrEqual
#define PtrEqual(P1,P2)     ((BOOLEAN)((ULONG_PTR)(P1) == (ULONG_PTR)(P2)))
#endif

// Get the value pointed by _Ptr
#ifndef Ptr2Value
#define Ptr2Value(_Ptr, _ValueType)   (*((_ValueType*)(_Ptr)))
#endif

//
//  This macro takes a length & rounds it up to a multiple of the alignment
//  Alignment is given as a power of 2
//
#ifndef ROUND_TO_SIZE
#define ROUND_TO_SIZE(_length, _alignment) \
            ((((ULONG_PTR)(_length)) + ((_alignment)-1)) & ~(ULONG_PTR) ((_alignment) - 1))
#endif

//
//  This macro takes a length & rounds it down to a multiple of the alignment
//  Alignment is given as a power of 2
//
#ifndef ROUNDDOWN_TO_SIZE
#define ROUNDDOWN_TO_SIZE(_length, _alignment) \
            (((ULONG_PTR)(_length)) & ~(ULONG_PTR) ((_alignment) - 1))
#endif

//
//  Checks if 1st argument is aligned on given power of 2 boundary specified
//  by 2nd argument
//
#ifndef IS_ALIGNED
#define IS_ALIGNED(_pointer, _alignment)                        \
        ((((ULONG_PTR) (_pointer)) & ((_alignment) - 1)) == 0)
#endif

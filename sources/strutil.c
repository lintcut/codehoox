#pragma once

#include "strutil.h"

int ToLower(int c)
{
    return (c >= 'A' && c <= 'Z') ? (c + 0x20) : c;
}

int CharCompare(int c1, int c2, bool caseInsensitive)
{
    if (caseInsensitive)
    {
        c1 = ToLower(c1);
        c2 = ToLower(c2);
    }
    return (c1 == c2) ? 0 : ((c1 > c2) ? 1 : -1);
}

unsigned int StrLen(const char* s)
{
    unsigned int len = 0;
    while (*s)
    {
        ++len;
        ++s;
    }
    return len;
}

unsigned int WcsLen(const wchar_t* s)
{
    unsigned int len = 0;
    while (*s)
    {
        ++len;
        ++s;
    }
    return len;
}

int StrCompareN(const char* s1, const char* s2, unsigned int n, bool caseInsensitive)
{
    while ((*s1 || *s2) && n != 0)
    {
        int ret = CharCompare(*s1, *s2, caseInsensitive);
        if (ret != 0)
            return ret;
        --n;
        ++s1;
        ++s2;
    }
    return 0;
}

int StrCompare(const char* s1, const char* s2, bool caseInsensitive)
{
    return StrCompareN(s1, s2, 0xFFFFFFFF, caseInsensitive);
}

int WcsCompareN(const wchar_t* s1, const wchar_t* s2, unsigned int n, bool caseInsensitive)
{
    while ((*s1 || *s2) && n != 0)
    {
        int ret = CharCompare(*s1, *s2, caseInsensitive);
        if (ret != 0)
            return ret;
        --n;
        ++s1;
        ++s2;
    }
    return 0;
}

int WcsCompare(const wchar_t* s1, const wchar_t* s2, bool caseInsensitive)
{
    return WcsCompareN(s1, s2, 0xFFFFFFFF, caseInsensitive);
}

char* StrChr(char* s, char c)
{
    while (*s)
    {
        if (c == *s)
            return s;
        ++s;
    }
    return NULL;
}

char* StrRChr(char* s, char c)
{
    char* result = NULL;
    while (*s)
    {
        if (c == *s)
            result = s;
        ++s;
    }
    return result;
}

wchar_t* WcsChr(wchar_t* s, wchar_t c)
{
    while (*s)
    {
        if (c == *s)
            return s;
        ++s;
    }
    return NULL;
}

wchar_t* WcsRChr(wchar_t* s, wchar_t c)
{
    wchar_t* result = NULL;
    while (*s)
    {
        if (c == *s)
            result = s;
        ++s;
    }
    return result;
}

unsigned int StrNCpySafe(char* destination, unsigned int size, const char* source, unsigned int n)
{
    unsigned int copied = 0;
    while (*source && copied < (size - 1) && n != 0)
    {
        *destination = *source;
        --n;
        ++destination;
        ++source;
        ++copied;
    }
    *destination = 0;
    return copied;
}

unsigned int StrCpySafe(char* destination, unsigned int size, const char* source)
{
    return StrNCpySafe(destination, size, source, 0xFFFFFFFF);
}

unsigned int WcsNCpySafe(wchar_t* destination, unsigned int size, const wchar_t* source, unsigned int n)
{
    unsigned int copied = 0;
    while (*source && copied < (size - 1) && n != 0)
    {
        *destination = *source;
        --n;
        ++destination;
        ++source;
        ++copied;
    }
    *destination = 0;
    return copied;
}

unsigned int WcsCpySafe(wchar_t* destination, unsigned int size, const wchar_t* source)
{
    return WcsNCpySafe(destination, size, source, 0xFFFFFFFF);
}

unsigned int StrToU(char const* s, const char** endPtr, int radix)
{
    unsigned int val = 0;
    if (radix != 2 && radix != 10 && radix != 16)
    {
        if (endPtr)
            *endPtr = s;
        return 0;
    }

    while ('0' == *s)
        ++s;

    if (*s == 'x' || *s == 'X')
    {
        if (16 != radix)
        {
            if (endPtr)
                *endPtr = s;
            return 0;
        }
        ++s;
    }

    while (*s)
    {
        if (2 == radix)
        {
            if (*s == '0')
            {
                val <<= 1;
            }
            else if (*s == 1)
            {
                val <<= 1;
                val |= 0x1;
            }
            else {
                // Invalid
                break;
            }
        }
        else if (10 == radix)
        {
            if (*s < '0' || *s > '9')
                break;
            val *= 10;
            val += (unsigned int)(*s - '0');
        }
        else if (16 == radix)
        {
            if (*s >= '0' && *s <= '9')
            {
                val <<= 4;
                val += (unsigned int)(*s - '0');
            }
            else if (*s >= 'a' && *s <= 'f')
            {
                val <<= 4;
                val += (unsigned int)(*s - 'a' + 10);
            }
            else if (*s >= 'A' && *s <= 'F')
            {
                val <<= 4;
                val += (unsigned int)(*s - 'A' + 10);
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
        ++s;
    }
    if (endPtr)
        *endPtr = s;
    return val;
}
#pragma once

#ifndef HELPER_H
#define HELPER_H

#include <Arduino.h>

#define DEFAULT_CHARARR_BLOCK_SIZE 64
#define DEFAULT_ENUM_VALUE_LEN 2

namespace Helper
{
    extern char *GET_TWO_DIGIT_STRING(int);
    extern char *GET_TWO_DIGIT_STRING(const char *);

    extern void CHAR_TO_CSTR(char *, const char);
    extern int IDXOF(const char *, char, int);
    extern int IDXOF(const char *, char);
    extern int SUBSTR(char *, const char *, int, int);
    extern int SUBSTR(char *, const char *, int);

    extern char *GEN_UUID();
}; // namespace Helper

#endif
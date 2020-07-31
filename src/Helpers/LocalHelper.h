#pragma once

#ifndef LOCALHELPER_H
#define LOCALHELPER_H

#include "common.h"

// Append char
// extern void appendChar(char *, char);

// Array size
#define GET_ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

// Char array size
// #define DEFAULT_CHAR_ARRAY_SIZE 64

// Char array substring
// extern bool charArrSubString(char *, char *, int, int);

// Convert int to 2 char digit string, i.e. int 1 -> "01"
extern String GET_TWO_DIGIT_STRING(int);
extern String GET_TWO_DIGIT_STRING(char *);
extern String GET_TWO_DIGIT_STRING(String);

// Convert to c string
// extern void toCString(char*, int);
// extern void toCString(char*, long);
// extern void toCString(char*, double);
// extern void toCString(char*, float);

// // Cut str
// extern bool strcut(char *, char *, int, int, bool);
// extern bool strcut(char *, char *, int, int);

// // reset chip
// extern void resetChip();

// // Search c string
// extern int findIndex(char*, char);
// extern int reverseFindIndex(char*, char);

#endif
#pragma once

#ifndef HELPER_H
#define HELPER_H

#include <Arduino.h>

namespace Helper
{
    extern const int DEFAULT_ENUM_VALUE_LENGTH;

    extern String GET_TWO_DIGIT_STRING(int);
    extern String GET_TWO_DIGIT_STRING(char *);
    extern String GET_TWO_DIGIT_STRING(String);
}; // namespace Helper

#endif
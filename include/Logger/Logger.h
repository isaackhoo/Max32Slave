#pragma once

#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include "Comms/Serial/SerialComms.h"
#include "Master/Master.h"

// forward declaration
class Master;

namespace Logger
{
    extern bool init(HardwareSerial *);
    extern void attachMasterInstance(Master *);

    extern void log(String, bool);
    extern void log(String);
    extern void logError(String, bool);
    extern void logError(String);
}; // namespace Logger

#endif
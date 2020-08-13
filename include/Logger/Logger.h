#pragma once

#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include "Comms/Serial/SerialComms.h"
#include "Master/Master.h"

// forward declaration
class Master;

class Logger
{
public:
    Logger();
    bool init(HardwareSerial *);

public:
    void setMasterInstance(Master *);
    void log(const char *, bool);
    void log(const char *);
    void log(int);
    void logError(const char *, bool);
    void logError(const char *);

private:
    SerialComms serial;
    Master *masterInstance;
};

extern Logger logger;

#endif
#pragma once

#ifndef LOGGER_H
#define LOGGER_H

#include "common.h"
#include "Comms/Serial/SerialComms.h"
#include "Master/Master.h"

// forward declaration
class Master;
class Logger;

// -----------------------
// Logger Public Variables
// -----------------------
extern Logger *logger;

class Logger
{
private:
    SerialComms *serial;
    Master *masterInstance;

public:
    Logger();
    void init(HardwareSerial *);

    void log(String, bool);
    void log(String);
    void logError(String, bool);
    void logError(String);

    void attachContext(Master *);
};

#endif
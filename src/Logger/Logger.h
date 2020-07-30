#pragma once

#ifndef LOGGER_H
#define LOGGER_H

#include "common.h"
#include "Comms/Serial/SerialComms.h"
#include "Master/Master.h"

// forward declaration
class Master;

class Logger
{
    private:
        SerialComms *serial;
        Master *context;

    public:
        Logger();
        void init(HardwareSerial *);

        void log(String);
        void logError(String);

        void attachContext(Master *);
};
extern Logger *logger;

#endif
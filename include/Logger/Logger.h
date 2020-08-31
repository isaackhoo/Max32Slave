#pragma once

#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <Print.h>
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

    void logCpy(const char *);
    void logCpy(int);
    void logCpy(double);

    void logCat(const char *);
    void logCat(int);
    void logCat(double);

    void out(bool);
    void out();
    void out(const char *, bool);
    void out(const char *);
    void errOut(bool);
    void errOut();
    void errOut(const char *);
    // void log(const char *, bool);
    // void log(const char *);
    // void log(int);
    // void logError(const char *, bool);
    // void logError(const char *);

private:
    SerialComms serial;
    Master *masterInstance;

    char logStr[DEFAULT_CHARARR_BLOCK_SIZE * 4];

private:
    char *dtoa(double, size_t);
    char *dtoa(double);
};

extern Logger logger;

#endif
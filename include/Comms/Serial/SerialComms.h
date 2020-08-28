#pragma once

#ifndef SERIAL_COMMS_H
#define SERIAL_COMMS_H

#include <Arduino.h>
#include "Comms/Serial/Constants.h"
#include "Helper/Helper.h"

using namespace SerialCommsConstants;

class SerialComms
{
public:
    char serialIn[DEFAULT_CHARARR_BLOCK_SIZE * 8];

public:
    SerialComms();
    SerialComms(HardwareSerial *);
    bool init(HardwareSerial *);

    bool read(const char);
    bool send(const char *);

    void clearSerialIn();

private:
    HardwareSerial *ss;
};

#endif
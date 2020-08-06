#pragma once

#ifndef SERIAL_COMMS_H
#define SERIAL_COMMS_H

#include <Arduino.h>
#include "Comms/Serial/Constants.h"

using namespace SerialCommsConstants;

class SerialComms
{
public:
    String serialIn;

public:
    SerialComms();
    SerialComms(HardwareSerial *);
    bool init(HardwareSerial *);

    bool read(const char);
    bool send(String);

private:
    HardwareSerial *ss;
};

#endif
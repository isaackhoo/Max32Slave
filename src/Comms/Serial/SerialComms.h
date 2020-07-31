#pragma once

#ifndef SERIALCOMMS_H
#define SERIALCOMMS_H

#include "Comms/Serial/Constants.h"
#include <HardwareSerial.h>

using namespace SerialConstants;

class SerialComms
{
private:
    HardwareSerial *ss;

public:
    String serialIn;

    SerialComms(HardwareSerial *);
    void init(HardwareSerial *);

    bool read();
    bool send(const char *);
    bool send(String);
};

#endif
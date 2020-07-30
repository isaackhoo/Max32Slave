#pragma once

#ifndef SERIALCOMMS_H
#define SERIALCOMMS_H

#include <HardwareSerial.h>

extern const int DEFAULT_SERIAL_BAUD;

class SerialComms
{
    private:
        HardwareSerial *ss;
        String serialIn;

    public:
        SerialComms(HardwareSerial *);
        void init(HardwareSerial *);

        bool read();
        bool send(const char *);
        bool send(String);
};

#endif
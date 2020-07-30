#include "SerialComms.h"

const int DEFAULT_SERIAL_BAUD = 115200;

// --------------------------
// SerialComms Private Methods
// --------------------------

// --------------------------
// SerialComms Public Methods
// --------------------------
SerialComms::SerialComms(HardwareSerial *ss)
{
    this->init(ss);
};

void SerialComms::init(HardwareSerial *ss)
{
    this->ss = ss;
    this->serialIn = "";
};

bool SerialComms::read()
{
    if (this->ss->available() > 0)
    {
        this->serialIn = this->ss->readString();
        return true;
    }
    return false;
};

bool SerialComms::send(const char *msg)
{
    return this->ss->print(msg);
};

bool SerialComms::send(String str)
{
    return this->send(str.c_str());
};

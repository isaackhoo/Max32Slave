#include "Comms/Serial/SerialComms.h"

namespace SerialCommsConstants
{
    const int DEFAULT_SERIAL_BAUD = 115200;
};

// ------------------------------
// SERIALCOMMS PUBLIC VARIABLES
// ------------------------------

// ------------------------------
// SERIALCOMMS PUBLIC METHODS
// ------------------------------
SerialComms::SerialComms(){};

SerialComms::SerialComms(HardwareSerial *ss)
{
    this->init(ss);
};

bool SerialComms::init(HardwareSerial *ss)
{
    // initialize string
    this->serialIn.reserve(256);

    this->ss = ss;
    this->ss->end();
    this->ss->begin(DEFAULT_SERIAL_BAUD);
};

bool SerialComms::read(const char END_CHAR)
{
    if (this->ss->available() > 0)
    {
        this->serialIn += this->ss->readStringUntil(END_CHAR);
        this->serialIn += END_CHAR;
        return true;
    };
    return false;
};

bool SerialComms::send(String toSend)
{
    return this->ss->print(toSend);
};

// ------------------------------
// SERIALCOMMS PRIVATE VARIABLES
// ------------------------------

// ------------------------------
// SERIALCOMMS PRIVATE METHODS
// ------------------------------
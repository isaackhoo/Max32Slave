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
    // initialize serial
    this->ss = ss;
    this->ss->end();
    this->ss->begin(DEFAULT_SERIAL_BAUD);
};

bool SerialComms::read(const char END_CHAR)
{
    if (this->ss->available() > 0)
    {
        char buf[DEFAULT_CHARARR_BLOCK_SIZE];
        int bytesRead = this->ss->readBytesUntil(END_CHAR, buf, sizeof(buf));

        // force terminate a string
        if (buf[bytesRead] != '\0')
            buf[bytesRead] = '\0';

        if (strlen(this->serialIn) > 0)
            strcat(this->serialIn, buf);
        else
            strcpy(this->serialIn, buf);
            
        return true;
    };
    return false;
};

bool SerialComms::send(const char *toSend)
{
    this->ss->print(toSend);
    this->ss->flush();
    return true;
};

void SerialComms::clearSerialIn()
{
    this->serialIn[0] = '\0';
};

// ------------------------------
// SERIALCOMMS PRIVATE VARIABLES
// ------------------------------

// ------------------------------
// SERIALCOMMS PRIVATE METHODS
// ------------------------------
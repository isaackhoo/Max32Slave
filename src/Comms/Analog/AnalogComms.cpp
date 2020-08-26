#include "Comms/Analog/AnalogComms.h"

// --------------------------------
// ANALOGCOMMS PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// ANALOGCOMMS PUBLIC METHODS
// --------------------------------
AnalogComms::AnalogComms(){};

AnalogComms::AnalogComms(int pin, int io)
{
    this->init(pin, io);
};

void AnalogComms::init(int pin, int io)
{
    this->pin = (uint8_t)pin;
    this->lastReadVal = 0.00;
    pinMode(this->pin, io);
};

double AnalogComms::aRead()
{
    double read = analogRead(this->getPin());

    if (read != this->lastReadVal)
    {
        Serial.println(read);
        this->lastReadVal = read;
    }

    return this->lastReadVal;
};

uint8_t AnalogComms::getPin()
{
    return this->pin;
};

double AnalogComms::getLastReadVal()
{
    return this->lastReadVal;
};

// --------------------------------
// ANALOGCOMMS PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// ANALOGCOMMS PRIVATE METHODS
// --------------------------------
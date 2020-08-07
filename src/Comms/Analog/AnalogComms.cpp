#include "Comms/Analog/AnalogComms.h"

// --------------------------------
// ANALOGCOMMS PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// ANALOGCOMMS PUBLIC METHODS
// --------------------------------
AnalogComms::AnalogComms(){};

AnalogComms::AnalogComms(int pin)
{
    this->init(pin);
};

void AnalogComms::init(int pin)
{
    this->pin = (uint8_t)pin;
};

double AnalogComms::aRead()
{
    this->lastReadVal = analogRead(this->getPin());
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
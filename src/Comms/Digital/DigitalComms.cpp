#include "Comms/Digital/DigitalComms.h"

// --------------------------------
// DIGITALCOMMS PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// DIGITALCOMMS PUBLIC METHODS
// --------------------------------
DigitalComms::DigitalComms(){};

DigitalComms::DigitalComms(int pin)
{
    this->init(pin);
};

void DigitalComms::init(int pin)
{
    this->pin = (uint8_t)pin;
};

int DigitalComms::dRead()
{
    this->lastReadVal = digitalRead(this->getPin());
    return this->lastReadVal;
};

void DigitalComms::dWriteHigh()
{
    digitalWrite(this->getPin(), HIGH);
};

void DigitalComms::dWriteLow()
{
    digitalWrite(this->getPin(), LOW);
};

uint8_t DigitalComms::getPin()
{
    return this->pin;
};

int DigitalComms::getLastReadVal()
{
    return this->lastReadVal;
};

// --------------------------------
// DIGITALCOMMS PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// DIGITALCOMMS PRIVATE METHODS
// --------------------------------
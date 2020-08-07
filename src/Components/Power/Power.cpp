#include "Components/Power/Power.h"

// --------------------------------
// POWER PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// POWER PUBLIC METHODS
// --------------------------------
Power::Power(){};

Power::Power(int pin) : DigitalComms(pin){};

void Power::on()
{
    this->dWriteHigh();
};

void Power::off()
{
    this->dWriteLow();
};

// --------------------------------
// POWER PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// POWER PRIVATE METHODS
// --------------------------------
#include "AnalogComms.h"

// ---------------------------
// AnalogComms Public Methods
// ---------------------------
AnalogComms::AnalogComms(int pin)
{
    this->pin = pin;
};

double AnalogComms::aRead()
{
    double val = 0;

    val = analogRead(this->pin);

    return val;
};
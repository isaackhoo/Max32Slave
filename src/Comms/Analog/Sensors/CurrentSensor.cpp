#include "CurrentSensor.h"

// -----------------------------
// CurrentSensor Public Methods
// -----------------------------
CurrentSensor::CurrentSensor(int pin)
{
    this->bit = pin;
};

double CurrentSensor::readCurrent()
{
    double val;
    
    if(this->bit == finger0FirstBit || this->bit == finger1FirstBit)
    {
        val = frontFingersCS.getCurrent_mA();
    }
    else if(this->bit == finger2FirstBit || this->bit == finger3FirstBit)
    {
        val = backFingersCS.getCurrent_mA();
    }
    else
    {
        // ERROR! what to do?
    }

    return val;
};
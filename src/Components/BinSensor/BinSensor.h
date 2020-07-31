#pragma once

#ifndef BINSENSOR_H
#define BINSENSOR_H

#include "Components./Power/Power.h"
#include "Comms/Analog/AnalogComms.h"

// forward declaration
class Power;
class AnalogComms;

class BinSensor : public Power, public AnalogComms
{
    public:
        BinSensor(int, int);
};

#endif
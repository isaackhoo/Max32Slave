#pragma once

#ifndef BINSENSOR_H
#define BINSENSOR_H

#include <Arduino.h>
#include "Comms/Analog/AnalogComms.h"

class BinSensor : public AnalogComms
{
public:
    BinSensor();
    BinSensor(int);
};

#endif
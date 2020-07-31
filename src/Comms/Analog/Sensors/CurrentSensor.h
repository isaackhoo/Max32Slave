#pragma once

#ifndef CURRENTSENSOR_H
#define CURRENTSENSOR_H

#include "Comms/Analog/AnalogComms.h"

class CurrentSensor : public AnalogComms
{
    public:
        CurrentSensor(int);
        double aRead();
};

#endif
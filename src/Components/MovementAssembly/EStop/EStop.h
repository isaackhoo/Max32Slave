#pragma once

#ifndef ESTOP_H
#define ESTOP_H

#include "Comms/Analog/AnalogComms.h"
#include "Components/HBridge/HBridge.h"

class EStop : public AnalogComms, public HBridge
{
    public:
        EStop(int, int, int);
};

#endif
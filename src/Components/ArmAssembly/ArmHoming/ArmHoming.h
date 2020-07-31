#pragma once

#ifndef ARMHOMING_H
#define ARMHOMING_H

#include "Comms/Digital/Sensors/DigitalSensor.h"
#include "Components/Power/Power.h"

class ArmHoming : public DigitalSensor, public Power 
{
    public:
        ArmHoming(int, int);
};

#endif
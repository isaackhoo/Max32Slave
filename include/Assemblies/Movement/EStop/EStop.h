#pragma once

#ifndef ESTOP_H
#define ESTOP_H

#include <Arduino.h>
#include "Comms/Analog/AnalogComms.h"
#include "Components/DCMotor/DCMotor.h"
#include "Components/Power/Power.h"

class EStop : public AnalogComms, public DCMotor
{
public:
    EStop();
    EStop(int, int, int, int, int);
};

#endif
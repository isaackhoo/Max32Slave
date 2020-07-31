#pragma once

#ifndef FINGERPAIR_H
#define FINGERPAIR_H

#include "Comms/Analog/Sensors/CurrentSensor.h"
#include "Components/DCMotor/DCMotor.h"

// forward declaration
class CurrentSensor;
class DCMotor;

class FingerPair
{
private:
    CurrentSensor *csFront;
    CurrentSensor *csRear;
    DCMotor *dcFront;
    DCMotor *dcRear;

public:
    FingerPair(CurrentSensor *, CurrentSensor *, DCMotor *, DCMotor *);
    void extend();
    void retract();
};

#endif
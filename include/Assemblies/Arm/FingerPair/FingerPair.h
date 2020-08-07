#pragma once

#ifndef FINGERPAIR_H
#define FINGERPAIR_H

#include <Arduino.h>
#include "Components/Sensors/CurrentSensor/CurrentSensor.h"
#include "Components/DCMotor/DCMotor.h"

class Finger : public DCMotor
{
public:
    Finger();
    Finger(CurrentSensor *, int, int);

private:
    CurrentSensor *cs;
};

class FingerPair
{
public:
    FingerPair();
    FingerPair(
        CurrentSensor *frontCs,
        int frontP1,
        int frontP2,
        CurrentSensor *rearCs,
        int rearP1,
        int rearP2);

    bool extend();
    bool retract();

private:
    Finger frontFinger;
    Finger rearFinger;
};

#endif
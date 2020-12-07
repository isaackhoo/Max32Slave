#pragma once

#ifndef FINGERPAIR_H
#define FINGERPAIR_H

#include <Arduino.h>
#include "Components/Sensors/CurrentSensor/CurrentSensor.h"
#include "Components/DCMotor/DCMotor.h"
#include "Assemblies/Arm/FingerPair/Constants.h"
#include "Helper/Helper.h"
#include "ControlCharacters/ControlCharacters.h"

using namespace FingerPairConstants;
using namespace Helper;

class Finger : public DCMotor
{
public:
    CurrentSensor *cs;

public:
    Finger();
    Finger(CurrentSensor *, int, int, int, int);

    void resetAvgCurrentReadings();
    void appendCurrentReading(double);
    double getAvgCurrent();
    void recordRetractionAvgCurrentRead();
    double getRetractionAvgCurrentRead();

    void setMovementStarted(bool);
    bool getIsMovementStarted();

private:
    double avgCurrentDraw;
    unsigned int totalCurrentReads;

    double retractionAvgCurrentRead;

    bool movementStarted;
};

class FingerPair
{
public:
    FingerPair();
    FingerPair(
        int direction,
        CurrentSensor *frontCs,
        int frontP1,
        int frontP2,
        int frontPwrPasstru,
        int frontPwm,
        CurrentSensor *rearCs,
        int rearP1,
        int rearP2,
        int rearPwrPasstru,
        int rearPwm);
    char *run();

    void powerOn();
    void powerOff();

    bool extend();
    bool retract();

private:
    Finger frontFinger;
    Finger rearFinger;
    int direction;
    unsigned int timeStart;

private:
    void initializeFingersBeforeMove();
    ENUM_FINGER_MOVEMENT lastMovement;
    ENUM_FINGER_STATE currentFingerState;
};

#endif
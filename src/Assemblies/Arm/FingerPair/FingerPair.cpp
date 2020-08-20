#include "Assemblies/Arm/FingerPair/FingerPair.h"

// -------------------------------------------- FINGER --------------------------------------------
// --------------------------------
// FINGER PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// FINGER PUBLIC METHODS
// --------------------------------

// --------------------------------
// FINGER PRIVATE VARIABLES
// --------------------------------
Finger::Finger(){};

Finger::Finger(CurrentSensor *cs, int dPin1, int dPin2, int pwrPin, int pwmPin) : DCMotor(dPin1, dPin2, pwrPin, pwmPin)
{
    this->cs = cs;
};

// --------------------------------
// FINGERPAIR PRIVATE METHODS
// --------------------------------

// -------------------------------------------- FINGERPAIR --------------------------------------------
// --------------------------------
// FINGERPAIR PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// FINGERPAIR PUBLIC METHODS
// --------------------------------
FingerPair::FingerPair(){};

FingerPair::FingerPair(
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
    int rearPwm)
{
    this->direction = direction;
    this->frontFinger = Finger(frontCs, frontP1, frontP2, frontPwrPasstru, frontPwm);
    this->rearFinger = Finger(rearCs, rearP1, rearP2, rearPwrPasstru, rearPwm);
};

char *FingerPair::run()
{
    char *res = NULL;
    double frontCurrent = this->frontFinger.cs->readCurrent();
    double rearCurrent = this->rearFinger.cs->readCurrent();

    if (frontCurrent <= FINGER_CURRENT_THRESHOLD && rearCurrent <= FINGER_CURRENT_THRESHOLD)
    {
        static char result[DEFAULT_CHARARR_BLOCK_SIZE];
        itoa(this->direction, result, 10);
        res = result;
    }

    return res;
};

void FingerPair::powerOn()
{
    this->frontFinger.on();
    this->rearFinger.on();
};

void FingerPair::powerOff()
{
    this->frontFinger.off();
    this->rearFinger.off();
};

bool FingerPair::extend()
{
    this->frontFinger.extend();
    this->rearFinger.extend();
};

bool FingerPair::retract()
{
    this->frontFinger.retract();
    this->rearFinger.retract();
};

// --------------------------------
// FINGERPAIR PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// FINGERPAIR PRIVATE METHODS
// --------------------------------
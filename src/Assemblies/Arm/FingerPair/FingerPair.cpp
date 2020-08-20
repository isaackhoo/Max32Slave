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

    this->timeStart = 0;
};

char *FingerPair::run()
{
    char *res = NULL;

    unsigned int currentMillis = millis();
    unsigned int millisDiff = currentMillis - this->timeStart;

    if (millisDiff >= FINGER_DELAY_BEFORE_READING && millisDiff <= FINGER_TIMEOUT_DURATION)
    {
        int frontCurrent = (int)this->frontFinger.cs->readCurrent();
        int rearCurrent = (int)this->rearFinger.cs->readCurrent();

        this->frontFinger.cs->readVoltage();
        this->rearFinger.cs->readVoltage();

        if (frontCurrent <= FINGER_CURRENT_THRESHOLD && rearCurrent <= FINGER_CURRENT_THRESHOLD)
        {
            static char result[DEFAULT_CHARARR_BLOCK_SIZE];
            itoa(this->direction, result, 10);
            res = result;
        }
    }
    else if (millisDiff > FINGER_TIMEOUT_DURATION)
        res = NAKSTR "Finger timed out";

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
    this->timeStart = millis();
};

bool FingerPair::retract()
{
    this->frontFinger.retract();
    this->rearFinger.retract();
    this->timeStart = millis();
};

// --------------------------------
// FINGERPAIR PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// FINGERPAIR PRIVATE METHODS
// --------------------------------
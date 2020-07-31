#include "FingerPair.h"

// -----------------------------
// FingerPair Public Methods
// -----------------------------
FingerPair::FingerPair(CurrentSensor *csFront, CurrentSensor *csRear, DCMotor *dcFront, DCMotor *dcRear)
{
    this->csFront = csFront;
    this->csRear = csRear;
    this->dcFront = dcFront;
    this->dcRear = dcRear; 
};

void FingerPair::extend()
{

};

void FingerPair::retract()
{

};
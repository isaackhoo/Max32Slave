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

Finger::Finger(CurrentSensor *cs, int dPin1, int dPin2) : DCMotor(dPin1, dPin2)
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
    CurrentSensor *frontCs,
    int frontP1,
    int frontP2,
    CurrentSensor *rearCs,
    int rearP1,
    int rearP2)
{
    this->frontFinger = Finger(frontCs, frontP1, frontP2);
    this->rearFinger = Finger(rearCs, rearP1, rearP2);
};

bool FingerPair::extend(){

};

bool FingerPair::retract(){

};

// --------------------------------
// FINGERPAIR PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// FINGERPAIR PRIVATE METHODS
// --------------------------------
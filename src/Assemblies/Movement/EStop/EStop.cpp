#include "Assemblies/Movement/EStop/EStop.h"

// --------------------------------
// ESTOP PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// ESTOP PUBLIC METHODS
// --------------------------------
EStop::EStop(){};

EStop::EStop(int aPin, int dP1, int dP2, int pwrPin, int pwmPin) : AnalogComms(aPin), DCMotor(dP1, dP2, pwrPin, pwmPin){};

// --------------------------------
// ESTOP PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// ESTOPPAIR PRIVATE METHODS
// --------------------------------
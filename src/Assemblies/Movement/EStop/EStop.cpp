#include "Assemblies/Movement/EStop/EStop.h"

// --------------------------------
// ESTOP PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// ESTOP PUBLIC METHODS
// --------------------------------
EStop::EStop(){};

EStop::EStop(int aPin, int pPin, int dP1, int dP2) : AnalogComms(aPin), Power(pPin), DCMotor(dP1, dP2){};

// --------------------------------
// ESTOP PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// ESTOPPAIR PRIVATE METHODS
// --------------------------------
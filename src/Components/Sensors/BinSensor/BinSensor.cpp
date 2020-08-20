#include "Components/Sensors/BinSensor/BinSensor.h"

// --------------------------------
// BINSENSOR PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// BINSENSOR PUBLIC METHODS
// --------------------------------
BinSensor::BinSensor(){};

BinSensor::BinSensor(int aPin) : AnalogComms(aPin){};

char *BinSensor::run()
{
    char *res = NULL;

    double val = this->aRead();

    return res;
};

// --------------------------------
// BINSENSOR PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// BINSENSOR PRIVATE METHODS
// --------------------------------
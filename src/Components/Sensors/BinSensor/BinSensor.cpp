#include "Components/Sensors/BinSensor/BinSensor.h"

// --------------------------------
// BINSENSOR PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// BINSENSOR PUBLIC METHODS
// --------------------------------
BinSensor::BinSensor(){};

BinSensor::BinSensor(int aPin) : AnalogComms(aPin, INPUT)
{
    this->lastReadMillis = 0;
};

char *BinSensor::run()
{
    char *res = NULL;

    double val = this->aRead();
    this->updateLastReadMillis();

    return res;
};

unsigned int BinSensor::getLastReadMillis()
{
    return this->lastReadMillis;
};

// --------------------------------
// BINSENSOR PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// BINSENSOR PRIVATE METHODS
// --------------------------------
void BinSensor::updateLastReadMillis()
{
    this->lastReadMillis = millis();
};
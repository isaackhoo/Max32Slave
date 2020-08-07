#include "Components/Sensors/CurrentSensor/CurrentSensor.h"

// --------------------------------
// CURRENTSENSOR PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// CURRENTSENSOR PUBLIC METHODS
// --------------------------------
CurrentSensor::CurrentSensor(){};

CurrentSensor::CurrentSensor(uint8_t id)
{
    this->init(id);
};

void CurrentSensor::init(uint8_t id)
{
    this->sensor = Adafruit_INA219(id);
};

double CurrentSensor::read()
{
    this->lastReadVal = this->sensor.getCurrent_mA();
    return this->getLastReadVal();
};

double CurrentSensor::getLastReadVal()
{
    return this->lastReadVal;
};
// --------------------------------
// CURRENTSENSOR PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// CURRENTSENSOR PRIVATE METHODS
// --------------------------------
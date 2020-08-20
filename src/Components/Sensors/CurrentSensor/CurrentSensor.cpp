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
    this->lastReadCurrentVal = 0.0;
    this->lastReadVoltageVal = 0.0;
};

void CurrentSensor::init(uint8_t id)
{
    this->sensor = Adafruit_INA219(id);
    this->sensor.begin();
};

double CurrentSensor::readCurrent()
{
    double read = (double)this->sensor.getCurrent_mA();
    if (read != this->lastReadCurrentVal)
    {
        Serial.print("Current ");
        Serial.println(read);
        this->lastReadCurrentVal = read;
    }
    return this->getLastReadCurrentVal();
};

double CurrentSensor::getLastReadCurrentVal()
{
    return this->lastReadCurrentVal;
};

double CurrentSensor::readVoltage()
{
    double read = (double)this->sensor.getShuntVoltage_mV();
    double bus = (double)this->sensor.getBusVoltage_V();
    if (read != this->lastReadVoltageVal)
    {
        Serial.print("voltage ");
        Serial.println(read);
        Serial.print("voltage bus ");
        Serial.println(bus);
        this->lastReadVoltageVal = read;
    }
};

double CurrentSensor::getLastReadVoltageVal()
{
    return this->lastReadVoltageVal;
};

// --------------------------------
// CURRENTSENSOR PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// CURRENTSENSOR PRIVATE METHODS
// --------------------------------
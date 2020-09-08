#include "Components/Sensors/CurrentSensor/CurrentSensor.h"
#include "Logger/Logger.h"

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
    this->lastReadShuntVoltageVal = 0.0;
    this->lastReadBusVoltageVal = 0.0;
};

void CurrentSensor::init(uint8_t id)
{
    this->sensor = Adafruit_INA219(id);
    this->sensor.begin();

    this->lastReadMillis = 0;
};

unsigned int CurrentSensor::getLastReadMillis()
{
    return this->lastReadMillis;
};

double CurrentSensor::readCurrent()
{
    double read = this->sensor.getCurrent_mA();
    this->updateLastReadMillis();
    if (read != this->lastReadCurrentVal)
    {
        this->lastReadCurrentVal = read;
    }
    return this->getLastReadCurrentVal();
};

double CurrentSensor::getLastReadCurrentVal()
{
    return this->lastReadCurrentVal;
};

double CurrentSensor::readShuntVoltage()
{
    double shunt = this->sensor.getShuntVoltage_mV();
    this->updateLastReadMillis();
    if (shunt != this->lastReadShuntVoltageVal)
    {
        // Serial.print("Shunt Voltage ");
        // Serial.println(shunt);
        this->lastReadShuntVoltageVal = shunt;
    }
    return shunt;
};

double CurrentSensor::getLastReadShuntVoltageVal()
{
    return this->lastReadShuntVoltageVal;
};

double CurrentSensor::readBusVoltage()
{
    double bus = (double)this->sensor.getBusVoltage_V();
    this->updateLastReadMillis();
    if (bus != this->lastReadBusVoltageVal)
    {
        this->lastReadBusVoltageVal = bus;
    }
    return bus;
};

double CurrentSensor::getLastReadBusVoltageVal()
{
    return this->lastReadBusVoltageVal;
};

// --------------------------------
// CURRENTSENSOR PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// CURRENTSENSOR PRIVATE METHODS
// --------------------------------
void CurrentSensor::updateLastReadMillis()
{
    this->lastReadMillis = millis();
};
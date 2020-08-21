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
    this->lastReadShuntVoltageVal = 0.0;
    this->lastReadBusVoltageVal = 0.0;
};

void CurrentSensor::init(uint8_t id)
{
    this->sensor = Adafruit_INA219(id);
    this->sensor.begin();
};

double CurrentSensor::readCurrent()
{
    double read = this->sensor.getCurrent_mA();
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
    if (bus != this->lastReadBusVoltageVal)
    {
        // Serial.print("Bus Voltage ");
        // Serial.println(bus);
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
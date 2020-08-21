#pragma once

#ifndef CURRENTSENSOR_H
#define CURRENTSENSOR_H

#include <Arduino.h>
#include <Adafruit_INA219.h>

class CurrentSensor
{
public:
    CurrentSensor();
    CurrentSensor(uint8_t);
    void init(uint8_t);

    double readCurrent();
    double getLastReadCurrentVal();

    double readShuntVoltage();
    double getLastReadShuntVoltageVal();

    double readBusVoltage();
    double getLastReadBusVoltageVal();

private:
    Adafruit_INA219 sensor;
    double lastReadCurrentVal;
    double lastReadShuntVoltageVal;
    double lastReadBusVoltageVal;
};

#endif
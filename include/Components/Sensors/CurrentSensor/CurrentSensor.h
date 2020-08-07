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

    double read();
    double getLastReadVal();

private:
    Adafruit_INA219 sensor;
    double lastReadVal;
};

#endif
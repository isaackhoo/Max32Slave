#pragma once

#ifndef ANALOGCOMMS_H
#define ANALOGCOMMS_H

#include <Arduino.h>

class AnalogComms
{
public:
    AnalogComms();
    AnalogComms(int, int);
    void init(int, int);

    double aRead();

    uint8_t getPin();
    double getLastReadVal();

private:
    uint8_t pin;
    double lastReadVal;
};

#endif
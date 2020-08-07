#pragma once

#ifndef DIGITALCOMMS_H
#define DIGITALCOMMS_H

#include <Arduino.h>

class DigitalComms
{
public:
    DigitalComms();
    DigitalComms(int);
    void init(int);

    int dRead();
    void dWriteHigh();
    void dWriteLow();

    uint8_t getPin();
    int getLastReadVal();

private:
    uint8_t pin;
    int lastReadVal;
};

#endif
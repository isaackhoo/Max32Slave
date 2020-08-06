#pragma once

#ifndef DIGITALCOMMS_H
#define DIGITALCOMMS_H

#include <Arduino.h>

class DigitalComms
{
    private:
        int pin;

    public:
        DigitalComms(int);
        int dRead();
        int dWriteHigh();
        int dWriteLow();
};

#endif
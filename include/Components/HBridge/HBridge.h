#pragma once

#ifndef HBRIDGE_H
#define HBRIDGE_H

#include <Arduino.h>
#include "Comms/Digital/DigitalComms.h"
#include "Components/Power/Power.h"

class HBridge : public Power
{
public:
    HBridge();
    HBridge(int, int, int, int);

    void allowPowerPassthrough();
    void disallowPowerPassthrough();

    void switchHL();
    void switchLH();
    void switchHH();
    void switchLL();

    int readPwm();
    int readLanes();

private:
    DigitalComms lane1;
    DigitalComms lane2;

    DigitalComms pwm;
};

#endif
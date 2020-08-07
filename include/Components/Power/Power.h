#pragma once

#ifndef POWER_H
#define POWER_H

#include <Arduino.h>
#include "Comms/Digital/DigitalComms.h"

class Power : public DigitalComms
{
public:
    Power();
    Power(int);
    void on();
    void off();
};

#endif
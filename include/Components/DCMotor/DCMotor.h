#pragma once

#ifndef DCMOTOR_H
#define DCMOTOR_H

#include <Arduino.h>
#include "Components/HBridge/HBridge.h"

class DCMotor : public HBridge
{
public:
    DCMotor();
    DCMotor(int, int, int, int);

    void stop();
    void extend();
    void retract();
};

#endif
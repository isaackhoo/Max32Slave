#pragma once

#ifndef DCMOTOR_H
#define DCMOTOR_H

#include "Components/HBridge/HBridge.h"

class DCMotor : public HBridge
{
    private:
        void move();

    public:
        DCMotor(int, int);
        void stop();
        void extend();
        void retract();
};

#endif
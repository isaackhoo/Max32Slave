#pragma once

#ifndef BRAKE_H
#define BRAKE_H

#include <Arduino.h>
#include "Components/HBridge/HBridge.h"

class Brake : public HBridge
{
public:
    Brake();
    Brake(int, int, int, int);

    void engage();
    void disengage();

    bool getIsEngaged();

private:
    bool isEngaged;
};

#endif
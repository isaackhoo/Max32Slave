#pragma once

#ifndef ESTOP_H
#define ESTOP_H

#include <Arduino.h>
#include "Comms/Analog/AnalogComms.h"
#include "Components/DCMotor/DCMotor.h"
#include "Assemblies/Movement/EStop/Constants.h"
#include "Helper/Helper.h"
#include "ControlCharacters/ControlCharacters.h"

using namespace EstopConstants;
using namespace Helper;

class EStop : public AnalogComms, public DCMotor
{
public:
    EStop();
    EStop(int, int, int, int, int);
    char *run();

    void extend();
    void retract();

private:
    ENUM_ESTOP_EXTENSION lastInstructions;
    unsigned int timeStart;
};

#endif
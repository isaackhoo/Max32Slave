#pragma once

#ifndef ARMMOTOR_H
#define ARMMOTOR_H

#include <Arduino.h>
#include "Assemblies/Arm/ArmMotor/Constants.h"
#include "Comms/RoboTeq/Roboteq.h"
#include "Comms/RoboTeq/Constants.h"
#include "ControlCharacters/ControlCharacters.h"

using namespace ArmMotorConstants;
using namespace RoboteqConstants;

class ArmMotor : public Roboteq
{
public:
    ArmMotor();
    ArmMotor(HardwareSerial *);

    char *run();

    void extend(const char *);
    void home();

private:
    int targetPositionCount;
    unsigned int timeStart;

    int lastPositionCount;
    bool isHoming;

private:
    void moveTo(const char *);
};

#endif
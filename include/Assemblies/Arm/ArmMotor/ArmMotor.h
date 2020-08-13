#pragma once

#ifndef ARMMOTOR_H
#define ARMMOTOR_H

#include <Arduino.h>
#include "Assemblies/Arm/ArmMotor/Constants.h"
#include "Comms/Serial/SerialComms.h"

using namespace ArmMotorConstants;

class ArmMotor : public SerialComms
{
public:
    ArmMotor();
    ArmMotor(HardwareSerial *);

    void extend(const char *);
    void home();

private:
    void moveTo(const char *);
};

#endif
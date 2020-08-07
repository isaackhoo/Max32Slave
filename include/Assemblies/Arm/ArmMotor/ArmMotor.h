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

    void extend(String);
    void home();

private:
    void moveTo(String);
};

#endif
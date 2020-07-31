#pragma once 

#ifndef ARMMOTOR_H
#define ARMMOTOR_H

#include "common.h"
#include "Components/ArmAssembly/ArmMotor/Constants.h"
#include "Comms/Serial/SerialComms.h"

using namespace ArmMotorConstants;

class ArmMotor : public SerialComms
{
    private:
        void moveTo(String);

    public:
        ArmMotor(HardwareSerial *);
        void extend(ENUM_ARM_POSITIONS);
        void home();
};

#endif
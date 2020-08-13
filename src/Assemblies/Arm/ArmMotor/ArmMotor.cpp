#include "Assemblies/Arm/ArmMotor/ArmMotor.h"

namespace ArmMotorConstants
{
    const char *DEFAULT_ARM_HOME_VAL = "0";
};

// --------------------------------
// ARMMOTOR PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// ARMMOTOR PUBLIC METHODS
// --------------------------------
ArmMotor::ArmMotor(){};

ArmMotor::ArmMotor(HardwareSerial *ss) : SerialComms(ss){};

void ArmMotor::extend(const char *val)
{
    this->moveTo(val);
};

void ArmMotor::home()
{
    this->moveTo(DEFAULT_ARM_HOME_VAL);
};

// --------------------------------
// ARMMOTOR PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// ARMMOTOR PRIVATE METHODS
// --------------------------------
void ArmMotor::moveTo(const char *val){
    // motor setup

    // move motor to val
};
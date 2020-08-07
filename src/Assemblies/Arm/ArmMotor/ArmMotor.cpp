#include "Assemblies/Arm/ArmMotor/ArmMotor.h"

namespace ArmMotorConstants
{
    const String DEFAULT_ARM_HOME_VAL(F("0"));
};

// --------------------------------
// ARMMOTOR PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// ARMMOTOR PUBLIC METHODS
// --------------------------------
ArmMotor::ArmMotor(){};

ArmMotor::ArmMotor(HardwareSerial *ss) : SerialComms(ss){};

void ArmMotor::extend(String val)
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
void ArmMotor::moveTo(String val){
    // motor setup

    // move motor to val
};
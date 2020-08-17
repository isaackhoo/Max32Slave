#include "Components/DCMotor/DCMotor.h"

// --------------------------------
// DCMOTOR PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// DCMOTOR PUBLIC METHODS
// --------------------------------
DCMotor::DCMotor(){};

DCMotor::DCMotor(int p1, int p2, int pwrPin, int pwmPin) : HBridge(p1, p2, pwrPin, pwmPin){};

void DCMotor::stop()
{
    this->switchLL();
};

void DCMotor::extend()
{
    this->switchHL();
};

void DCMotor::retract()
{
    this->switchLH();
};

// --------------------------------
// DCMOTOR PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// DCMOTOR PRIVATE METHODS
// --------------------------------
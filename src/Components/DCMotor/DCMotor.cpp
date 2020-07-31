#include "DCMotor.h"

// ---------------------------
// HBridgeMDC Private Methods
// ---------------------------
void DCMotor::move(){

};

// ---------------------------
// HBridgeMDC Public Methods
// ---------------------------
DCMotor::DCMotor(int pin1, int pin2) : HBridge(pin1, pin2){};

void DCMotor::stop()
{
    this->move();
};

void DCMotor::extend()
{
    this->move();
};

void DCMotor::retract()
{
    this->move();
};
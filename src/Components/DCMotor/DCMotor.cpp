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


// for finger...    EXTEND  > pin1 HIGH / pin2 LOW
//                  RETRACT > pin1 LOW  / pin2 HIGH
//                  STOP    > pin1 LOW  / pin2 LOW

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
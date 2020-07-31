#include "MoveMotor.h"

// --------------------------
// MoveMotor Public Methods
// --------------------------
MoveMotor::MoveMotor(HardwareSerial *ss, int headPin, int rearPin) : SerialComms(ss)
{
    this->headSensor = new DigitalSensor(headPin);
    this->rearSensor = new DigitalSensor(rearPin);
};

void MoveMotor::moveTo(int slothole){

};
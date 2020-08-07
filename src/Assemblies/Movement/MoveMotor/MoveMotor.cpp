#include "Assemblies/Movement/MoveMotor/MoveMotor.h"

// --------------------------------
// MOVEMOTOR PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// MOVEMOTOR PUBLIC METHODS
// --------------------------------
MoveMotor::MoveMotor(){};

MoveMotor::MoveMotor(HardwareSerial *ss, int dP1, int dP2) : SerialComms(ss)
{
    this->frontSensor = DigitalSensor(dP1);
    this->rearSensor = DigitalSensor(dP2);
};

void MoveMotor::moveTo(String slothole)
{
    
};

// --------------------------------
// MOVEMOTOR PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// MOVEMOTORPAIR PRIVATE METHODS
// --------------------------------

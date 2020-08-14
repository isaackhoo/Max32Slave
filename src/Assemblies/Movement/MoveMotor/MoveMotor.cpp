#include "Assemblies/Movement/MoveMotor/MoveMotor.h"

// --------------------------------
// MOVEMOTOR PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// MOVEMOTOR PUBLIC METHODS
// --------------------------------
MoveMotor::MoveMotor(){};

MoveMotor::MoveMotor(HardwareSerial *ss, int dP1, int dP2, int currentSlothole) : SerialComms(ss)
{
    this->frontSensor = DigitalSensor(dP1);
    this->rearSensor = DigitalSensor(dP2);

    this->currentSlothole = currentSlothole;
};

void MoveMotor::moveTo(const char *slothole)
{
    this->setMotorMode(SPEED);

    // determine direction
    int targetSlothole = atoi(slothole);
    ENUM_MOVEMENT_DIRECTION direction;
    if (targetSlothole > this->currentSlothole)
        direction = FORWARD;
    else if (targetSlothole < this->currentSlothole)
        direction = REVERSE;
    else
        direction = NOT_MOVING;

    // determine speed
    int difference = abs(this->currentSlothole - targetSlothole);

    // blah blah
    
};

// --------------------------------
// MOVEMOTOR PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// MOVEMOTORPAIR PRIVATE METHODS
// --------------------------------
void MoveMotor::setMotorMode(ENUM_CLOSED_LOOP_MODES mode)
{
    // set mode
    this->currentMotorMode = mode;
    switch (this->currentMotorMode)
    {
    case ENUM_CLOSED_LOOP_MODES::SPEED:
    {
        // update motor mode
        this->send(MMODE MODE_SPEED "\n");
        // update deceleration
        this->send(MDEC SPEED_DEC "\n");
        // update PID
        this->send(KP SPEED_KP "\n");
        this->send(KI SPEED_KI "\n");
        this->send(KD SPEED_KD "\n");
        break;
    }
    case ENUM_CLOSED_LOOP_MODES::POSITION:
    {
        // update motor mode
        this->send(MMODE MODE_POSITION "\n");
        // update PID
        this->send(KP POSITION_KP "\n");
        this->send(KI POSITION_KI "\n");
        this->send(KD POSITION_KD "\n");
        break;
    }
    default:
        break;
    }
};
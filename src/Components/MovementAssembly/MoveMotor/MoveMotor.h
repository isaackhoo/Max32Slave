#pragma once

#ifndef MOVEMOTOR_H
#define MOVEMOTOR_H

#include "Comms/Serial/SerialComms.h"
#include "Comms/Digital/Sensors/DigitalSensor.h"

// forward Declaration
class DigitalSensor;

class MoveMotor : public SerialComms
{
private:
    DigitalSensor *headSensor;
    DigitalSensor *rearSensor;

public:
    MoveMotor(HardwareSerial *, int, int);
    void moveTo(int);
};

#endif
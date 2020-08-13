#pragma once

#ifndef MOVEMOTOR_H
#define MOVEMOTOR_H

#include <Arduino.h>
#include "Assemblies/Movement/MoveMotor/Constants.h"
#include "Comms/Serial/SerialComms.h"
#include "Components/Sensors/DigitalSensor/DigitalSensor.h"

using namespace MoveMotorConstants;

class MoveMotor : public SerialComms
{
public:
    MoveMotor();
    MoveMotor(HardwareSerial *, int, int);

    void moveTo(const char *);

private:
    DigitalSensor frontSensor;
    DigitalSensor rearSensor;
};

#endif
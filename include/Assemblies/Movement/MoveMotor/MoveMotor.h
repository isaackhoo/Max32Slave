#pragma once

#ifndef MOVEMOTOR_H
#define MOVEMOTOR_H

#include <Arduino.h>
#include "Assemblies/Movement/MoveMotor/Constants.h"
#include "Comms/Serial/SerialComms.h"
#include "Components/Sensors/DigitalSensor/DigitalSensor.h"
#include "Helper/Helper.h"

using namespace MoveMotorConstants;
using namespace Helper;

class MoveSensor : public DigitalSensor
{
public:
    MoveSensor();
    MoveSensor(int);

public:
    bool run(ENUM_MOVEMENT_DIRECTION);
    void setCounter(int);
    void incrementCounter();
    void decrementCounter();

    int getCount();

private:
    int counter;
};

class MoveMotor : public SerialComms
{
public:
    MoveMotor();
    MoveMotor(HardwareSerial *, int, int, int);
    char *run();

public:
    void moveTo(const char *);
    void stop();
    void updateCurrentSlothole(const char *);

private:
    MoveSensor frontSensor;
    MoveSensor rearSensor;

    MoveSensor *leadingSensor;
    MoveSensor *trailingSensor;

    ENUM_CLOSED_LOOP_MODES currentMotorMode;
    ENUM_MOVEMENT_DIRECTION currentMovementDirection;

    int targetSlothole;
    int currentSlothole;

    bool hasRead;

    unsigned int testMillis;
    int repetitions;
    bool isStopping;
    bool adjustmentStarted;

private:
    void setMotorMode(ENUM_CLOSED_LOOP_MODES);
    void updateMoveSpeed(int);
};

#endif
#pragma once

#ifndef MOVEMOTOR_H
#define MOVEMOTOR_H

#include <Arduino.h>
#include "Assemblies/Movement/MoveMotor/Constants.h"
#include "Comms/Roboteq/Roboteq.h"
#include "Comms/RoboTeq/Constants.h"
#include "Assemblies/Movement/Brake/Brake.h"
#include "Components/Sensors/DigitalSensor/DigitalSensor.h"
#include "Helper/Helper.h"
#include "ControlCharacters/ControlCharacters.h"

using namespace MoveMotorConstants;
using namespace RoboteqConstants;
using namespace Helper;

class MoveSensor : public DigitalSensor
{
public:
    MoveSensor();
    MoveSensor(int);

public:
    bool run(ENUM_MOVEMENT_DIRECTION);
    bool run(ENUM_MOTORSENSOR_READING);
    void setCounter(int);
    void incrementCounter();
    void decrementCounter();

    int getCount();

private:
    int counter;
};

class MoveMotor : public Roboteq
{
public:
    MoveMotor();
    MoveMotor(HardwareSerial *, int, int, int, int, int, int);
    char *run();

public:
    bool moveTo(const char *);
    void cutShuttleSpeed();
    void immediateStop();
    void updateCurrentSlothole(const char *);

    void engageBrake();
    void disengageBrake();

private:
    Brake brake;
    MoveSensor frontSensor;
    MoveSensor rearSensor;

    MoveSensor *leadingSensor;
    MoveSensor *trailingSensor;
    MoveSensor *readingSensor;

    ENUM_MOVEMENT_DIRECTION currentMovementDirection;

    int targetSlothole;
    int currentSlothole;

    unsigned int lastSlotholeMillis;
    unsigned int lastSlotholeStopTimeoutDuration;
    bool isStopping;
    unsigned int modeToggleMillis;
    bool shouldReverseCreep;

    int creepCount;
    unsigned int lastCreepMillis;

private:
    void updateMoveSpeed(int);
    void determineLastSlotholeTimeoutDuration();
    bool onLastSlotholeArrival();
    char *createSlotholeArriveSuccessStr();
};

#endif
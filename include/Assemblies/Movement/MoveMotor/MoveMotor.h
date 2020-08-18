#pragma once

#ifndef MOVEMOTOR_H
#define MOVEMOTOR_H

#include <Arduino.h>
#include "Assemblies/Movement/MoveMotor/Constants.h"
#include "Comms/Serial/SerialComms.h"
#include "Assemblies/Movement/Brake/Brake.h"
#include "Components/Sensors/DigitalSensor/DigitalSensor.h"
#include "Helper/Helper.h"
#include "ControlCharacters/ControlCharacters.h"

using namespace MoveMotorConstants;
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

class MoveMotor : public SerialComms
{
public:
    MoveMotor();
    MoveMotor(HardwareSerial *, int, int, int, int, int, int);
    char *run();

public:
    void moveTo(const char *);
    void stop();
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

    ENUM_CLOSED_LOOP_MODES currentMotorMode;
    ENUM_MOVEMENT_DIRECTION currentMovementDirection;

    int targetSlothole;
    int currentSlothole;

    unsigned int lastSlotholeMillis;
    unsigned int lastSlotholeStopTimeoutDuration;
    bool isStopping;

    bool shouldCreepPosition;
    int creepCount;
    unsigned int lastCreepMillis;

private:
    void setMotorMode(ENUM_CLOSED_LOOP_MODES);
    void updateMoveSpeed(int);
    void determineLastSlotholeTimeoutDuration();
    int interpretRpmFeedback(const char *);
    bool onLastSlotholeArrival();
};

#endif
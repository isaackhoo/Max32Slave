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
    ENUM_MOTORSENSOR_READING run();

    void onToggle();

    void setCounter(int);
    void incrementCounter();
    void decrementCounter();
    int getCount();

    bool getIsInhole();

private:
    int counter;
    bool isInHole;
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
    void heavyStop();
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
    MoveSensor *altSensor;

    ENUM_MOVEMENT_DIRECTION lastMovementDirection;
    ENUM_MOVEMENT_DIRECTION originalMovementDirection;
    ENUM_MOVEMENT_DIRECTION currentMovementDirection;

    int targetSlothole;
    int currentSlothole;
    int lastMoveSpeed;

    unsigned int movementTimeoutDuration;
    unsigned int movementStartMillis;
    unsigned int movementStoppedMillis;

    bool movementComplete;
    bool hasTrailingReadFirst;

    bool isPreparingStop;

    int creepCount;
    unsigned int lastCreepMillis;
    unsigned int creepTries;

private:
    void initializeMovementVariables();
    unsigned int calcMovementTimeoutDuration(int);
    bool updateMoveSpeed(int);
    void updateCurrentSlothole(int);

    void toggleReadingSensor(MoveSensor *);
    void toggleReadingSensor();

    bool updateSensorOutHoleCount(MoveSensor *);

    bool onSpeedLeadInHoleEvt();
    bool onSpeedTrailInHoleEvt();
    bool onSpeedLeadOutHoleEvt();
    bool onSpeedTrailOutHoleEvt();

    bool onCreepLeadInHoleEvt();
    bool onCreepTrailInHoleEvt();
    bool onCreepLeadOutHoleEvt();
    bool onCreepTrailOutHoleEvt();

    void onInHoleEvent(MoveSensor *);
    void onOutHoleEvent(MoveSensor *);

    char *createSlotholeArriveSuccessStr();
};

#endif
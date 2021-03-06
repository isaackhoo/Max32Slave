#pragma once

#ifndef SHUTTLE_H
#define SHUTTLE_H

#include <Arduino.h>
#include <SPI.h>
#include "Shuttle/Constants.h"
#include "Master/Master.h"
#include "Master/Constants.h"
#include "Logger/Logger.h"

// movement
#include "Assemblies/Movement/MoveMotor/MoveMotor.h"
#include "Assemblies/Movement/EStop/EStop.h"
#include "Assemblies/Movement/Brake./Brake.h"

// arm
#include "Assemblies/Arm/ArmMotor/ArmMotor.h"
#include "Assemblies/Arm/FingerPair/FingerPair.h"
#include "Assemblies/Arm/ArmHoming/ArmHoming.h"
#include "Components/Sensors/CurrentSensor/CurrentSensor.h"

// accessories
#include "Components/Sensors/BinSensor/BinSensor.h"
#include "Components/Power/Power.h"
#include "Comms/RoboTeq/Roboteq.h"

using namespace ShuttleConstants;
using namespace MasterConstants;

// forward declaration
class Master;

class Shuttle
{
public:
    Shuttle();
    bool init(HardwareSerial *, HardwareSerial *);
    void run();

public:
    void setMasterInstance(Master *);
    void setCurrentSlothole(const char *);
    void onCommand(ENUM_MASTER_ACTIONS, const char *);

private:
    // movement assembly
    MoveMotor moveMotor;
    EStop eStopL;
    EStop eStopR;
    CurrentSensor eStopCs;

    // arm assembly
    ArmMotor armMotor;
    ArmHoming armHoming;

    CurrentSensor frontCs;
    CurrentSensor rearCs;
    FingerPair leftFP;
    FingerPair rightFP;

    // accessories
    BinSensor leftBs;
    BinSensor rightBs;
    Power central12V;
    Power central24V;

private:
    Master *masterInstance;
    ENUM_MASTER_ACTIONS currentStep;
    char currentStepInstructions[DEFAULT_CHARARR_BLOCK_SIZE];

private:
    void feedbackStepCompletion(const char *);
};

extern Shuttle shuttle;

#endif
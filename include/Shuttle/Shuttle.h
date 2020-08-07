#pragma once

#ifndef SHUTTLE_H
#define SHUTTLE_H

#include <Arduino.h>
#include "Shuttle/Constants.h"

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

using namespace ShuttleConstants;

class Shuttle
{
public:
    Shuttle();
    bool init(HardwareSerial *, HardwareSerial *);
    void run();

private:
    // movement assembly
    MoveMotor moveMotor;
    Brake brake;
    EStop eStop;

    // arm assembly
    CurrentSensor frontCs;
    CurrentSensor rearCs;

    ArmMotor armMotor;
    ArmHoming armHoming;
    FingerPair leftFP;
    FingerPair rightFP;

    // accessories
    BinSensor leftBs;
    BinSensor rightBs;
    Power central12V;
    Power central24V;
};

#endif
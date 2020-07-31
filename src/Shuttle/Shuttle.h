#pragma once

#ifndef SHUTTLE_H
#define SHUTTLE_H

#include "common.h"

#include "Components/MovementAssembly/MoveMotor/MoveMotor.h"
#include "Components/MovementAssembly/Brake/Brake.h"
#include "Components/MovementAssembly/EStop/EStop.h"

#include "Comms/Analog/Sensors/CurrentSensor.h"
#include "Components/DCMotor/DCMotor.h"
#include "Components/ArmAssembly/ArmMotor/ArmMotor.h"
#include "Components/ArmAssembly/ArmHoming/ArmHoming.h"
#include "Components/ArmAssembly/FingerPair/FingerPair.h"

#include "Components/BinSensor/BinSensor.h"

// forward declaration
class MoveMotor;
class Brake;
class EStop;

class CurrentSensor;
class DCMotor;
class ArmMotor;
class ArmHoming;
class FingerPair;

class Shuttle
{
private:
    // Movement Assembly
    MoveMotor *moveMotor;
    Brake *brake;
    EStop *estop;

    // Arm Assembly
    CurrentSensor *csHead;
    CurrentSensor *csRear;
    DCMotor *dcmHead;
    DCMotor *dcmRear;

    ArmMotor *armMotor;
    ArmHoming *ahLeft;
    ArmHoming *ahRight;
    FingerPair *fpLeft;
    FingerPair *fpRight;

    // Accessories
    BinSensor *binSensor;

public:
    Shuttle();
};

#endif
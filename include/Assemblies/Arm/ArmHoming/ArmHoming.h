#pragma once

#ifndef ARMHOMING_H
#define ARMHOMING_H

#include <Arduino.h>
#include "Assemblies/Arm/ArmHoming/Constants.h"
#include "Components/Sensors/DigitalSensor/DigitalSensor.h"
#include "Components/Power/Power.h"
#include "Components/Sensors/CurrentSensor/CurrentSensor.h"
#include "Helper/Helper.h"
#include "ControlCharacters/ControlCharacters.h"

using namespace ArmhomingConstants;
using namespace Helper;

class ArmHoming
{
public:
    ArmHoming();
    ArmHoming(int leftReadPin, int leftLaserPin, uint8_t leftCsPin, int rightReadPin, int rightLaserPin, uint8_t rightCsPin);
    char *run();

    bool laserOn();
    bool laserOff();

    bool isClear();
    bool startCheckingHome(const char *);
    void clearIsCheckingHome();
    bool getIsCheckingHome();

private:
    DigitalSensor leftReceiver;
    Power leftLaser;
    CurrentSensor leftCs;

    DigitalSensor rightReceiver;
    Power rightLaser;
    CurrentSensor rightCs;

    bool isCheckingHome;
    char armCountResult[DEFAULT_CHARARR_BLOCK_SIZE];
    unsigned int timeStart;
};

#endif
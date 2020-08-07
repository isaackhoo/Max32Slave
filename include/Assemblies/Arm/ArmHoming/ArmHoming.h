#pragma once

#ifndef ARMHOMING_H
#define ARMHOMING_H

#include <Arduino.h>
#include "Components/Sensors/DigitalSensor/DigitalSensor.h"
#include "Components/Power/Power.h"

class ArmHoming
{
public:
    ArmHoming();
    ArmHoming(int leftReadPin, int leftLaserPin, int rightReadPin, int rightLaserPin);

    bool isClear();

private:
    DigitalSensor leftReceiver;
    Power leftLaser;
    DigitalSensor rightReceiver;
    Power rightLaser;
};

#endif
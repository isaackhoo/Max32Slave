#pragma once

#ifndef ROBOTEQ_H
#define ROBOTEQ_H

#include <Arduino.h>
#include "Comms/Serial/SerialComms.h"
#include "Comms/Roboteq/Constants.h"
#include "Helper/Helper.h"

using namespace RoboteqConstants;
using namespace Helper;

class Roboteq : public SerialComms
{
public:
    Roboteq();
    Roboteq(HardwareSerial *);

    // feedback
    void requestRpm();
    void requestPositionCount();
    bool available();

    int getRoboteqFeedback();
    int getRoboteqRawFeedback();

    // controls
    bool setMode(ENUM_ROBOTEQ_CONFIG);
    bool setDeceleration(const char *);
    bool setDeceleration(int);
    bool setAccleration(const char *);
    bool setAccleration(int);

    ENUM_ROBOTEQ_CONFIG getMode();

    // speed mode controls
    bool setSpeedPercent(int);

    // relative position controls
    bool setRelativePosition(int);

    // position count controls
    bool setPositionCount(int);

    // battery
    void requestBatteryLevel();

private:
    ENUM_ROBOTEQ_CONFIG currentMode;

    char queryFeedback[DEFAULT_CHARARR_BLOCK_SIZE];
    int rawFeedback;

private:
    int interpretFeedback();
};

#endif
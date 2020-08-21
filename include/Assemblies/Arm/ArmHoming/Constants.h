#pragma once

#ifndef ARMHOMING_CONSTANTS_H
#define ARMHOMING_CONSTANTS_H

#define ARMHOMING_MIN_OPERATIONAL_BUS_VOLTAGE 4.7

#define ARM_HOMING_CHECK_TIMEOUT_DURATION 3000

namespace ArmhomingConstants
{
    enum ENUM_LASER_READ
    {
        LASER_READ = 1,
        LASER_NO_READ = 0,
    };
};

#endif
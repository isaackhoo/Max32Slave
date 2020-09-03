#pragma once

#ifndef MOVEMOTOR_CONSTANTS_H
#define MOVEMOTOR_CONSTANTS_H

// setup
#define DEFAULT_STARTING_SLOTHOLE 0
#define SLOTHOLE_MIN -2
#define SLOTHOLE_MAX 119
#define SLOTHOLE_BATTERY -2
#define SLOTHOLE_LIFTER -1
#define SLOTHOLE_BUFFER 0
#define SLOTHOLES_PER_COL 6

#define DEFAULT_MOVEMENT_MILLIS 0

// speed
#define SPEED_5 1000
#define SPEED_4 750
#define SPEED_3 500
#define SPEED_2 250
#define SPEED_1 120
#define MIN_SPEED_RPM 35

// creeping
#define POSITION_INITIAL_CREEP_DELAY 150
#define CREEP_VALUE 1
#define POSITION_CREEP_MIN_RPM 0
#define POSITION_CONTINUOUS_CREEP_DELAY 25
#define DEFAULT_MAX_CREEPS 200

namespace MoveMotorConstants
{
    enum ENUM_MOVEMENT_DIRECTION
    {
        NOT_MOVING = 0,
        FORWARD = 1,  // into racks. (front sensor direction)
        REVERSE = -1, // towards pickers (rear sensor direction)
    };

    enum ENUM_MOTORSENSOR_READING
    {
        IN_HOLE = 0,
        OUT_HOLE = 1,
        NO_CHANGE = -1
    };

    enum ENUM_LAST_SLOTHOLE_TIMEOUT_DURATION
    {
        // BATTERY_LIFTER = 1200,
        // LIFTER_BUFFER = 1200,
        // BUFFER_RACK = 1000,
        // NEXT_COL_FIRST_SLOT = 750,
        // WITHIN_COL = 200,
        BATTERY_LIFTER = 0,
        LIFTER_BUFFER = 0,
        BUFFER_RACK = 0,
        NEXT_COL_FIRST_SLOT = 0,
        WITHIN_COL = 0,
    };
}; // namespace MoveMotorConstants

#endif
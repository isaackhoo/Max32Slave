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
#define MOVEMENT_COMPLETION_SETTLE_DELAY 100

#define MOVEMENT_TIMEOUT_PER_SLOTHOLE 1000
#define TIMEOUT_MULTIPLIER_5 2
#define TIMEOUT_MULTIPLIER_4 2.5
#define TIMEOUT_MULTIPLIER_3 3
#define TIMEOUT_MULTIPLIER_2 5
#define TIMEOUT_MULTIPLIER_1 10

// speed
#define SPEED_5 1000
#define SPEED_4 750
#define SPEED_3 500
#define SPEED_2 250
#define SPEED_1 120
#define ALT_SENSOR_READ_DELAY 250
#define MIN_SPEED_RPM 20

// creeping
#define CREEP_VALUE 2
#define RPM_REQ_INTERVAL 250
#define POSITION_CONTINUOUS_CREEP_DELAY 40
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
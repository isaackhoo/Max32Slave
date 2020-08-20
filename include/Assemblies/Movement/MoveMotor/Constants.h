#pragma once

#ifndef MOVEMOTOR_CONSTANTS_H
#define MOVEMOTOR_CONSTANTS_H

#define DEFAULT_STARTING_SLOTHOLE 0
#define SLOTHOLE_MIN -2
#define SLOTHOLE_MAX 119
#define DEFAULT_LAST_SLOTHOLE_MILLIS 0

#define SLOTHOLE_BATTERY -2
#define SLOTHOLE_LIFTER -1
#define SLOTHOLE_BUFFER 0
#define SLOTHOLES_PER_COL 6

#define RBTQ_ENDSTR "\r"
#define RBTQ_ENDCHAR '\r'

#define MMODE "^MMOD "
#define MODE_SPEED "1"
#define MODE_POSITION "3"

#define MDEC "^MDEC "
#define LOW_DEC "800"
#define SPEED_DEC "3000"
#define HEAVY_DEC "8000"
#define SPEED_IM_DEC "30000"

#define KP "^KP "
#define KI "^KI "
#define KD "^KD "
#define SPEED_KP "1"
#define SPEED_KI "5"
#define SPEED_KD "4"
#define POSITION_KP "40"
#define POSITION_KI "0"
#define POSITION_KD "2"

#define SPEED_MOVE "!G "
#define PR_MOVE "!PR "

#define SPEED_5 1000
#define SPEED_4 750
#define SPEED_3 500
#define SPEED_2 250
#define SPEED_1 120

#define QUERY_RPM "?BS"
#define QUERY_DELIMITER '='

#define POSITION_INITIAL_CREEP_DELAY 150
#define CREEP_VALUE "1"
#define POSITION_CREEP_MIN_RPM 35
#define POSITION_CONTINUOUS_CREEP_DELAY 25
#define DEFAULT_MAX_CREEPS 100

namespace MoveMotorConstants
{
    enum ENUM_MOVEMENT_DIRECTION
    {
        NOT_MOVING = 0,
        FORWARD = 1,  // into racks. (front sensor direction)
        REVERSE = -1, // towards pickers (rear sensor direction)
    };

    enum ENUM_CLOSED_LOOP_MODES
    {
        SPEED = 1,
        POSITION = 3
    };

    enum ENUM_MOTORSENSOR_READING
    {
        IN_HOLE = 0,
        OUT_HOLE = 1
    };

    enum ENUM_LAST_SLOTHOLE_TIMEOUT_DURATION
    {
        BATTERY_LIFTER = 1200,
        LIFTER_BUFFER = 1200,
        BUFFER_RACK = 900,
        NEXT_COL_FIRST_SLOT = 500,
        WITHIN_COL = 100,
    };
}; // namespace MoveMotorConstants

#endif
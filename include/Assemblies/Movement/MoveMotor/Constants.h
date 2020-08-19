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
#define LOW_DEC "300"
#define SPEED_DEC "3000"
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

#define SPEED_4 1000
#define SPEED_3 750
#define SPEED_2 500
#define SPEED_1 250

#define QUERY_RPM "?BS"
#define QUERY_DELIMITER '='

#define CREEP_VALUE "1"
#define POSITION_CREEP_MIN_RPM 40
#define POSITION_CREEP_DELAY 50
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
        BUFFER_RACK = 500,
        NEXT_COL_FIRST_SLOT = 400,
        WITHIN_COL = 0,
    };
}; // namespace MoveMotorConstants

#endif
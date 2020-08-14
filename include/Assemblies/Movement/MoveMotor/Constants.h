#pragma once

#ifndef MOVEMOTOR_CONSTANTS_H
#define MOVEMOTOR_CONSTANTS_H

#define SLOTHOLE_MIN -2
#define SLOTHOLE_MAX 119

#define MMODE "^MMOD "
#define MODE_SPEED "1"
#define MODE_POSITION "3"

#define MDEC "^MDEC "
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

}; // namespace MoveMotorConstants

#endif
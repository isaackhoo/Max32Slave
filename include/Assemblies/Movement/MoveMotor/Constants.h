#pragma once

#ifndef MOVEMOTOR_CONSTANTS_H
#define MOVEMOTOR_CONSTANTS_H

namespace MoveMotorConstants
{
    extern const int SLOTHOLE_MIN;
    extern const int SLOTHOLE_MAX;

    enum ENUM_MOVEMENT_DIRECTION
    {
        NOT_MOVING = 0,
        FORWARD = 1, // into racks. (front sensor direction)
        REVERSE = -1, // towards pickers (rear sensor direction)
    };
}; // namespace MoveMotorConstants

#endif
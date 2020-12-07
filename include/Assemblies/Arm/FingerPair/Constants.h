#pragma once

#ifndef FINGER_PAIR_CONSTANTS_H
#define FINGER_PAIR_CONSTANTS_H

#define FINGER_CURRENT_THRESHOLD 5.00
#define FINGER_DELAY_BEFORE_READING 0
#define FINGER_TIMEOUT_DURATION 2500
#define FINGER_READ_INTERVAL 50
#define FINGER_INITIAL_MIN_CURRENT_DRAW 200.0

namespace FingerPairConstants
{
    enum ENUM_FINGER_MOVEMENT
    {
        FINGER_EXTENSION,
        FINGER_RETRACTION
    };

    enum ENUM_FINGER_STATE
    {
        FINGER_EXTENDED,
        FINGER_RETRACTED
    };
};

#endif
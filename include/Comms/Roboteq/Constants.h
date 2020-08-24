#pragma once

#ifndef ROBOTEQ_CONSTANTS_H
#define ROBOTEQ_CONSTANTS_H

#define RBTQ_ENDSTR "\r"
#define RBTQ_ENDCHAR '\r'

#define MMODE "^MMOD "
#define MODE_SPEED "1"
#define MODE_POSITION "3"

#define MDEC "^MDEC "
#define LOW_DEC "600"
#define MID_DEC "3000"
#define HIGH_DEC "8000"
#define IM_DEC "30000"

#define MACC "^MAC "

#define KP "^KP "
#define KI "^KI "
#define KD "^KD "
#define SPEED_KP "1"
#define SPEED_KI "5"
#define SPEED_KD "4"
#define RPOSITION_KP "40"
#define RPOSITION_KI "0"
#define RPOSITION_KD "2"
#define ARM_KP "180"
#define ARM_KI "1"
#define ARM_KD "4"

#define SPEED_MOVE "!G "
#define PR_MOVE "!PR "
#define P_MOVE "!P "

#define QUERY_RPM "?BS"
#define QUERY_POSITION_COUNT "?CB"
#define QUERY_DELIMITER '='

namespace RoboteqConstants
{
    enum ENUM_ROBOTEQ_CONFIG
    {
        SPEED = 0,
        R_POSITION,
        ARM_POSITION,
    };
};

#endif
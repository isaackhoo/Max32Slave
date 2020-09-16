#pragma once

#ifndef ROBOTEQ_CONSTANTS_H
#define ROBOTEQ_CONSTANTS_H

#define RBTQ_ENDSTR "\r"
#define RBTQ_ENDCHAR '\r'

// roboteq mode
#define MMODE "^MMOD "
#define MODE_SPEED "1"
#define MODE_POSITION "3"

// deceleration settings
#define MDEC "^MDEC "
#define LOW_DEC "600"
#define MID_DEC "3000"
#define HIGH_DEC "8000"
#define IM_DEC "30000"
#define ARM_DEC "1500"

// accleration settings
#define MACC "^MAC "
#define ARM_ACC "7000"

// speed settings
#define MXRPM "^MXRPM "
#define MVEL "^MVEL "
#define ARM_SPD "2250"

// PID settings
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

// movement commands
#define SPEED_MOVE "!G "
#define PR_MOVE "!PR "
#define P_MOVE "!P "

// query commands
#define QUERY_RPM "?BS"
#define QUERY_POSITION_COUNT "?CB"
#define QUERY_DELIMITER '='

#define QUERY_BATTERY "?V"
#define QUERY_BATT_DELIMITER ':'

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
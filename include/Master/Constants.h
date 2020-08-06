#pragma once

#ifndef MASTER_CONSTANTS_H
#define MASTER_CONSTANTS_H

#include <Arduino.h>

namespace MasterConstants
{
    // actions
    enum ENUM_MASTER_ACTIONS
    {
        SLAVE_LOGIN = 0,
        SLAVE_RESET,
        SLAVE_ECHO,
        LOG,
        LOGERROR,
        SLAVE_PING,

        ENGAGE_ESTOP,
        DISENGAGE_ESTOP,

        MOVETO, // implicitly uses brake and dual sensors

        READ_BIN_SENSOR,

        EXTEND_ARM,
        HOME_ARM, // implicitly also uses arm sensors

        EXTEND_FINGER_PAIR,
        RETRACT_FINGER_PAIR,

        SLAVE_BATTERY,
        SLAVE_ERROR,
        Num_Master_Actions_Enums
    };
    const String EnumMasterActionsString[(int)Num_Master_Actions_Enums] = {
        "SLAVE_LOGIN",
        "SLAVE_RESET",
        "SLAVE_ECHO",
        "LOG",
        "LOGERROR",
        "SLAVE_PING",

        "ENGAGE_ESTOP",
        "DISENGAGE_ESTOP",

        "MOVETO", // implicitly uses brake and dual sensors

        "READ_BIN_SENSOR",

        "EXTEND_ARM",
        "HOME_ARM", // implicitly also uses arm sensors

        "EXTEND_FINGER_PAIR",
        "RETRACT_FINGER_PAIR",

        "SLAVE_BATTERY",
        "SLAVE_ERROR",
    };

    // serial communications
    extern const String HEADER_DELIMITER;
    extern const String BODY_DELIMITER;

    // pings
    extern const unsigned long MASTER_PING_INTERVAL;
    extern const unsigned long MASTER_PING_TIMEOUT;
    extern const unsigned long MAX_PINGS_DROPPED;
}; // namespace MasterConstants

#endif
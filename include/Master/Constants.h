#pragma once

#ifndef MASTER_CONSTANTS_H
#define MASTER_CONSTANTS_H

#include <Arduino.h>

// serial communications
#define HEADER_DELIMITER ','
#define HEADER_DELIMITER_STR ","
#define BODY_DELIMITER '_'
#define BODY_DELIMITER_STR "_"

// pings
#define MASTER_PING_INTERVAL 1000 * 10
#define MASTER_PING_TIMEOUT 1000 * 5
#define MAX_PINGS_DROPPED 3

namespace MasterConstants
{
    // actions
    enum ENUM_MASTER_ACTIONS
    {
        SLAVE_LOGIN = 0, // 0
        SLAVE_RESET,  // 1
        SLAVE_ECHO, // 2
        LOG, // 3
        LOGERROR, // 4
        SLAVE_PING, // 5

        ENGAGE_ESTOP, // 6
        DISENGAGE_ESTOP, // 7

        MOVETO, // 8 - implicitly uses brake and dual sensors

        READ_BIN_SENSOR, // 9

        EXTEND_ARM, // 10
        HOME_ARM, // 11 - implicitly also uses arm sensors

        EXTEND_FINGER_PAIR, // 12
        RETRACT_FINGER_PAIR, // 13

        SLAVE_BATTERY, // 14
        SLAVE_ERROR, // 15
        UPDATE_SLOTHOLE, // 16
        Num_Master_Actions_Enums
    };
    const char *EnumMasterActionsString[(int)Num_Master_Actions_Enums] = {
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
}; // namespace MasterConstants

#endif
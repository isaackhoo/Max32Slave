#pragma once

#ifndef MASTERCONSTANTS_H
#define MASTERCONSTANTS_H

#include "common.h"
#include "Helpers/LocalHelper.h"

namespace MasterConstants
{
    // actions
    enum ENUM_SLAVE_ACTIONS
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
    };

    // serial communications
    extern const char *SOH;
    extern const char *STX;
    extern const char *ETX;
    extern const char *EOT;
    extern const char *NAK;

    extern const String HEADER_DELIMITER;
    extern const String BODY_DELIMITER;

    extern const unsigned int pingTimeoutDuration;

    class MasterCommsFormat
    {
    public:
        String uuid;
        int messageLength; // excludes STX and ETX
        String action;
        String instructions;

        MasterCommsFormat(String uuid, String act, String inst)
        {
            this->init(uuid, act, inst);
        };
        MasterCommsFormat(String act, String inst)
        {
            this->init(act, inst);
        };
        void init(String uuid, String act, String inst)
        {
            this->uuid = uuid;
            this->action = GET_TWO_DIGIT_STRING(act);
            this->instructions = inst;
            this->messageLength = this->action.length();
            if (this->instructions.length() > 0)
                this->messageLength += BODY_DELIMITER.length() + this->instructions.length();
        };
        void init(String act, String inst)
        {
            this->init(String(millis()), act, inst);
        };
    };
}; // namespace MasterConstants

#endif
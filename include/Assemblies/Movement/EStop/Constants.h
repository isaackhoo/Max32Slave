#pragma once

#ifndef ESTOP_CONSTANTS_H
#define ESTOP_CONSTANTS_H

#define ESTOP_EXTENDED 1.00
#define ESTOP_RETRACTED 0.00
#define ESTOP_TIMEOUT_DURATION 1000 * 3

namespace EstopConstants
{
    enum ENUM_ESTOP_EXTENSION
    {
        ESTOP_EXTEND = 1,
        ESTOP_RETRACT = 0,
        Num_Estop_Extensions = 2
    };
    const char *EnumEstopExtensionsString[(int)Num_Estop_Extensions] = {
        "ESTOP_EXTEND",
        "ESTOP_RETRACT",
    };
}; // namespace EstopConstants

#endif
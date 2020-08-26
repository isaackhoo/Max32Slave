#pragma once

#ifndef SHUTTLE_CONSTANTS_H
#define SHUTTLE_CONSTANTS_H

// Analog Pins
#define ESTOP_L_PIN_READ  57
#define ESTOP_R_PIN_READ 56

#define BINSENSOR_PIN_LEFT 54
#define BINSENSOR_PIN_RIGHT 55

// Digital Pins
#define MOVEMENT_SENSOR_PIN_FRONT 63
#define MOVEMENT_SENSOR_PIN_REAR 6

#define BRAKE_PIN_1 72
#define BRAKE_PIN_2 73
#define BRAKE_PIN_PWRPASSTRU 47
#define BRAKE_PIN_PWM 49

#define ESTOP_PIN_POWER  65

#define ESTOP_L_PIN_1  45
#define ESTOP_L_PIN_2  46
#define ESTOP_L_PIN_PWM 67

#define ESTOP_R_PIN_1 78
#define ESTOP_R_PIN_2 44
#define ESTOP_R_PIN_PWM 66

#define ARMHOMING_READ_PIN_LEFT 2
#define ARMHOMING_LASER_PIN_LEFT 4
#define ARMHOMING_READ_PIN_RIGHT 3
#define ARMHOMING_LASER_PIN_RIGHT 5

#define FINGER_FRONTLEFT_PIN_1 36
#define FINGER_FRONTLEFT_PIN_2 37
#define FINGER_FRONTLEFT_PIN_PWM 34
#define FINGER_FRONTRIGHT_PIN_1 39
#define FINGER_FRONTRIGHT_PIN_2 38
#define FINGER_FRONTRIGHT_PIN_PWM 35
#define FINGER_FRONT_PIN_PWRPASSTRU 33

#define FINGER_REARLEFT_PIN_1 30
#define FINGER_REARLEFT_PIN_2 79
#define FINGER_REARLEFT_PIN_PWM 23
#define FINGER_REARRIGHT_PIN_1 31
#define FINGER_REARRIGHT_PIN_2 32
#define FINGER_REARRIGHT_PIN_PWM 28
#define FINGER_REAR_PIN_PWRPASSTRU 22

#define POWER_PIN_12V 74
#define POWER_PIN_24V 77

// Current Sensors
#define CS_ID_FRONT 0x45
#define CS_ID_REAR 0x41
#define CS_ESTOP 0x44
#define CS_ARMHOMING_LEFT 0x43
#define CS_ARMHOMING_RIGHT 0x4C

namespace ShuttleConstants
{
    enum ENUM_EXTENSION_DIRECTION
    {
        LEFT = -1,
        RIGHT = 1,
        Num_Extension_Directions = 2
    };
    const String EnumExtensionDirectionString[(int)Num_Extension_Directions] = {
        "LEFT",
        "RIGHT",
    };
}; // namespace ShuttleConstants

#endif
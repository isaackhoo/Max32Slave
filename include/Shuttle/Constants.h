#pragma once

#ifndef SHUTTLE_CONSTANTS_H
#define SHUTTLE_CONSTANTS_H

namespace ShuttleConstants
{
    // Analog Pins
    extern const int ESTOP_PIN_READ;

    extern const int BINSENSOR_PIN_LEFT;
    extern const int BINSENSOR_PIN_RIGHT;

    // Digital Pins
    extern const int MOVEMENT_SENSOR_PIN_FRONT;
    extern const int MOVEMENT_SENSOR_PIN_REAR;

    extern const int BRAKE_PIN_1;
    extern const int BRAKE_PIN_2;

    extern const int ESTOP_PIN_POWER;
    extern const int ESTOP_PIN_1;
    extern const int ESTOP_PIN_2;

    extern const int ARMHOMING_READ_PIN_LEFT;
    extern const int ARMHOMING_LASER_PIN_LEFT;
    extern const int ARMHOMING_READ_PIN_RIGHT;
    extern const int ARMHOMING_LASER_PIN_RIGHT;

    extern const int FINGER_FRONTLEFT_PIN_1;
    extern const int FINGER_FRONTLEFT_PIN_2;
    extern const int FINGER_FRONTRIGHT_PIN_1;
    extern const int FINGER_FRONTRIGHT_PIN_2;
    extern const int FINGER_REARLEFT_PIN_1;
    extern const int FINGER_REARLEFT_PIN_2;
    extern const int FINGER_REARRIGHT_PIN_1;
    extern const int FINGER_REARRIGHT_PIN_2;

    extern const int POWER_PIN_12V;
    extern const int POWER_PIN_24V;

    // Current Sensors
    extern const uint8_t CS_ID_FRONT;
    extern const uint8_t CS_ID_REAR;
}; // namespace ShuttleConstants

#endif
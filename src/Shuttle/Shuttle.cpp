#include "Shuttle/Shuttle.h"

namespace ShuttleConstants
{
    // Analog Pins
    const int ESTOP_PIN_READ = 57;

    const int BINSENSOR_PIN_LEFT = 54;
    const int BINSENSOR_PIN_RIGHT = 55;

    // Digital Pins
    const int MOVEMENT_SENSOR_PIN_FRONT = 63;
    const int MOVEMENT_SENSOR_PIN_REAR = 6;

    const int BRAKE_PIN_1 = 72;
    const int BRAKE_PIN_2 = 73;

    const int ESTOP_PIN_POWER = 65;
    const int ESTOP_PIN_1 = 45;
    const int ESTOP_PIN_2 = 46;

    const int ARMHOMING_READ_PIN_LEFT = 2;
    const int ARMHOMING_LASER_PIN_LEFT = 4;
    const int ARMHOMING_READ_PIN_RIGHT = 3;
    const int ARMHOMING_LASER_PIN_RIGHT = 5;

    const int FINGER_FRONTLEFT_PIN_1 = 30;
    const int FINGER_FRONTLEFT_PIN_2 = 79;
    const int FINGER_FRONTRIGHT_PIN_1 = 36;
    const int FINGER_FRONTRIGHT_PIN_2 = 37;
    const int FINGER_REARLEFT_PIN_1 = 31;
    const int FINGER_REARLEFT_PIN_2 = 32;
    const int FINGER_REARRIGHT_PIN_1 = 39;
    const int FINGER_REARRIGHT_PIN_2 = 38;

    const int POWER_PIN_12V = 74;
    const int POWER_PIN_24V = 77;

    // Current Sensors
    const uint8_t CS_ID_FRONT = 0x45;
    const uint8_t CS_ID_REAR = 0x41;
}; // namespace ShuttleConstants

// --------------------------------
// SHUTTLE PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// SHUTTLE PUBLIC METHODS
// --------------------------------
Shuttle::Shuttle(){};

bool Shuttle::init(HardwareSerial *moveSerial, HardwareSerial *armSerial)
{
    bool res = true;
    // initialize movement assembly
    this->moveMotor = MoveMotor(
        moveSerial,
        MOVEMENT_SENSOR_PIN_FRONT,
        MOVEMENT_SENSOR_PIN_REAR);
    this->brake = Brake(BRAKE_PIN_1, BRAKE_PIN_2);
    this->eStop = EStop(
        ESTOP_PIN_READ,
        ESTOP_PIN_POWER,
        ESTOP_PIN_1,
        ESTOP_PIN_2);

    // initialize arm assembly
    this->frontCs = CurrentSensor(CS_ID_FRONT);
    this->rearCs = CurrentSensor(CS_ID_REAR);

    this->armMotor = ArmMotor(armSerial);
    this->armHoming = ArmHoming(
        ARMHOMING_READ_PIN_LEFT,
        ARMHOMING_LASER_PIN_LEFT,
        ARMHOMING_READ_PIN_RIGHT,
        ARMHOMING_LASER_PIN_RIGHT);
    this->leftFP = FingerPair(
        &this->frontCs,
        FINGER_FRONTLEFT_PIN_1,
        FINGER_FRONTLEFT_PIN_2,
        &this->rearCs,
        FINGER_REARLEFT_PIN_1,
        FINGER_REARLEFT_PIN_2);
    this->rightFP = FingerPair(
        &this->frontCs,
        FINGER_FRONTRIGHT_PIN_1,
        FINGER_FRONTRIGHT_PIN_2,
        &this->rearCs,
        FINGER_REARRIGHT_PIN_1,
        FINGER_REARRIGHT_PIN_2);

    // accessories
    this->leftBs = BinSensor(BINSENSOR_PIN_LEFT);
    this->rightBs = BinSensor(BINSENSOR_PIN_RIGHT);
    this->central12V = Power(POWER_PIN_12V);
    this->central24V = Power(POWER_PIN_24V);

    // ---------------------
    // post initialization
    // ---------------------

    return res;
};

void Shuttle::run(){

};

// --------------------------------
// SHUTTLE PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// SHUTTLEPAIR PRIVATE METHODS
// --------------------------------

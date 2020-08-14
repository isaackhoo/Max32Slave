#include "Shuttle/Shuttle.h"

namespace ShuttleConstants
{

}; // namespace ShuttleConstants

// --------------------------------
// SHUTTLE PUBLIC VARIABLES
// --------------------------------
Shuttle shuttle;

// --------------------------------
// SHUTTLE PUBLIC METHODS
// --------------------------------
Shuttle::Shuttle(){};

bool Shuttle::init(HardwareSerial *armSerial, HardwareSerial *moveSerial)
{
    bool res = true;
    // initialize movement assembly
    this->moveMotor = MoveMotor(
        moveSerial,
        MOVEMENT_SENSOR_PIN_FRONT,
        MOVEMENT_SENSOR_PIN_REAR,
        0);
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
    // power up shuttle
    this->central12V.on();
    this->central24V.on();

    // arm homing power

    // estop power

    // nullify
    this->masterInstance = NULL;

    // clear out current step
    this->currentStep = Num_Master_Actions_Enums;

    return res;
};

void Shuttle::run()
{
    // run only the component that is in use currently
    char *res;

    switch (this->currentStep)
    {
    case ENGAGE_ESTOP:
    {
        break;
    }
    case DISENGAGE_ESTOP:
    {
        break;
    }
    case MOVETO:
    {
        res = this->moveMotor.run();
        if (res != NULL)
        {
            Serial.println("Completed move action");
            // engage brakes
            this->brake.engage();
        }
        break;
    }
    case READ_BIN_SENSOR:
    {
        break;
    }
    case EXTEND_ARM:
    {
        break;
    }
    case HOME_ARM:
    {
        break;
    }
    case EXTEND_FINGER_PAIR:
    {
        break;
    }
    case RETRACT_FINGER_PAIR:
    {
        break;
    }
    case SLAVE_BATTERY:
    {
        break;
    }
    default:
        break;
    }

    if (res != NULL)
    {
        // step is complete
        this->masterInstance->onStepCompletion(this->currentStep, res);

        // clear out current step
        this->currentStep = Num_Master_Actions_Enums;
    }
};

void Shuttle::setMasterInstance(Master *context)
{
    this->masterInstance = context;
};

void Shuttle::onCommand(ENUM_MASTER_ACTIONS action, const char *inst)
{
    // record new step to perform
    this->currentStep = action;
    strcpy(this->currentStepInstructions, inst);

    switch (this->currentStep)
    {
    case ENGAGE_ESTOP:
    {
        this->eStop.extend();
        break;
    }
    case DISENGAGE_ESTOP:
    {
        this->eStop.retract();
        break;
    }
    case MOVETO:
    {
        if (strcmp(inst, "-3.00") == 0)
        {
            this->moveMotor.stop();
            this->masterInstance->onStepCompletion(this->currentStep, this->currentStepInstructions);
            return;
        }

        // move motor
        this->moveMotor.moveTo(inst);

        // disengage brakes
        this->brake.disengage();
        break;
    }
    case READ_BIN_SENSOR:
    {
        break;
    }
    case EXTEND_ARM:
    {
        break;
    }
    case HOME_ARM:
    {
        break;
    }
    case EXTEND_FINGER_PAIR:
    {
        break;
    }
    case RETRACT_FINGER_PAIR:
    {
        break;
    }
    case SLAVE_BATTERY:
    {
        break;
    }
    default:
        break;
    }
};

// --------------------------------
// SHUTTLE PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// SHUTTLEPAIR PRIVATE METHODS
// --------------------------------

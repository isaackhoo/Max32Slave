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
    // initialize SPI
    pinMode(SS, OUTPUT);
    SPI.begin();

    // initialize movement assembly
    this->moveMotor = MoveMotor(
        moveSerial,
        MOVEMENT_SENSOR_PIN_FRONT,
        MOVEMENT_SENSOR_PIN_REAR,
        BRAKE_PIN_1,
        BRAKE_PIN_2,
        BRAKE_PIN_PWRPASSTRU,
        BRAKE_PIN_PWM);
    this->eStop = EStop(
        ESTOP_PIN_READ,
        ESTOP_PIN_1,
        ESTOP_PIN_2,
        ESTOP_PIN_POWER,
        ESTOP_PIN_PWN);
    this->eStopCs = CurrentSensor(CS_ESTOP);

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
        LEFT,
        &this->frontCs,
        FINGER_FRONTLEFT_PIN_1,
        FINGER_FRONTLEFT_PIN_2,
        FINGER_FRONT_PIN_PWRPASSTRU,
        FINGER_FRONTLEFT_PIN_PWM,
        &this->rearCs,
        FINGER_REARLEFT_PIN_1,
        FINGER_REARLEFT_PIN_2,
        FINGER_REAR_PIN_PWRPASSTRU,
        FINGER_REARLEFT_PIN_PWM);
    this->rightFP = FingerPair(
        RIGHT,
        &this->frontCs,
        FINGER_FRONTRIGHT_PIN_1,
        FINGER_FRONTRIGHT_PIN_2,
        FINGER_FRONT_PIN_PWRPASSTRU,
        FINGER_FRONTRIGHT_PIN_PWM,
        &this->rearCs,
        FINGER_REARRIGHT_PIN_1,
        FINGER_REARRIGHT_PIN_2,
        FINGER_REAR_PIN_PWRPASSTRU,
        FINGER_REARRIGHT_PIN_PWM);

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

    // nullify
    this->masterInstance = NULL;

    // clear out current step
    this->currentStep = Num_Master_Actions_Enums;

    // keep fingers
    this->rightFP.retract();
    this->leftFP.retract();

    // ---------------------
    // test codes
    // ---------------------
    char inst[DEFAULT_CHARARR_BLOCK_SIZE];
    itoa(LEFT, inst, 10);
    this->onCommand(EXTEND_FINGER_PAIR, inst);

    return res;
};

void Shuttle::run()
{
    // run only the component that is in use currently
    char *res = NULL;

    switch (this->currentStep)
    {
    case ENGAGE_ESTOP:
    case DISENGAGE_ESTOP:
    {
        res = this->eStop.run();
        break;
    }
    case MOVETO:
    {
        res = this->moveMotor.run();
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
    case RETRACT_FINGER_PAIR:
    {
        ENUM_EXTENSION_DIRECTION direction = (ENUM_EXTENSION_DIRECTION)atoi(this->currentStepInstructions);
        switch (direction)
        {
            case LEFT:
            {
                res = this->leftFP.run();
                break;
            }
            case RIGHT:
            {
                res = this->rightFP.run();
                break;
            }
            default:
            {
                res = NAKSTR "Invalid finger pair to run";
                break;
            }
        }
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
        this->feedbackStepCompletion(res);
};

void Shuttle::setMasterInstance(Master *context)
{
    this->masterInstance = context;
};

void Shuttle::setCurrentSlothole(const char *slothole)
{
    this->moveMotor.updateCurrentSlothole(slothole);
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
        if (strcmp(inst, "-3.00") == 0) // temporary value to get shuttle to stop
        {
            this->moveMotor.immediateStop();
            this->masterInstance->onStepCompletion(this->currentStep, this->currentStepInstructions);
            return;
        }

        // move motor
        if (!this->moveMotor.moveTo(inst))
        {
            // shuttle is already at the target position
            this->feedbackStepCompletion(inst);
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
        // interpret direction from instructions
        ENUM_EXTENSION_DIRECTION direction = (ENUM_EXTENSION_DIRECTION)atoi(this->currentStepInstructions);
        switch (direction)
        {
        case LEFT:
        {
            this->leftFP.extend();
            break;
        }
        case RIGHT:
        {
            this->rightFP.extend();
            break;
        }
        default:
        {
            this->masterInstance->onStepCompletion(this->currentStep, NAKSTR "Invalid finger pair");
            break;
        }
        }
        break;
    }
    case RETRACT_FINGER_PAIR:
    {
        // interpret direction from instructions
        ENUM_EXTENSION_DIRECTION direction = (ENUM_EXTENSION_DIRECTION)atoi(this->currentStepInstructions);
        switch (direction)
        {
        case LEFT:
        {
            this->leftFP.retract();
            break;
        }
        case RIGHT:
        {
            this->rightFP.retract();
            break;
        }
        default:
        {
            this->masterInstance->onStepCompletion(this->currentStep, NAKSTR "Invalid finger pair");
            break;
        }
        }
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
void Shuttle::feedbackStepCompletion(const char *res)
{
    // step is complete
    this->masterInstance->onStepCompletion(this->currentStep, res);

    // clear out current step
    this->currentStep = Num_Master_Actions_Enums;
};
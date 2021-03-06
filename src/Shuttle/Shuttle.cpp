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
    this->eStopL = EStop(
        ESTOP_L_PIN_READ,
        ESTOP_L_PIN_1,
        ESTOP_L_PIN_2,
        ESTOP_PIN_POWER,
        ESTOP_L_PIN_PWM);
    this->eStopR = EStop(
        ESTOP_R_PIN_READ,
        ESTOP_R_PIN_1,
        ESTOP_R_PIN_2,
        ESTOP_PIN_POWER,
        ESTOP_R_PIN_PWM);
    this->eStopCs = CurrentSensor(CS_ESTOP);

    // initialize arm assembly
    this->armMotor = ArmMotor(armSerial);
    this->armHoming = ArmHoming(
        ARMHOMING_READ_PIN_LEFT,
        ARMHOMING_LASER_PIN_LEFT,
        CS_ARMHOMING_LEFT,
        ARMHOMING_READ_PIN_RIGHT,
        ARMHOMING_LASER_PIN_RIGHT,
        CS_ARMHOMING_RIGHT);

    this->frontCs = CurrentSensor(CS_ID_FRONT);
    this->rearCs = CurrentSensor(CS_ID_REAR);
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

    // change motor mode
    this->moveMotor.setMode(SPEED);

    // keep fingers
    this->rightFP.retract();
    this->leftFP.retract();

    // home arm
    this->armMotor.home();

    // off arm homing
    this->armHoming.laserOff();

    // disengage brakes
    this->moveMotor.disengageBrake();

    // estop
    this->eStopL.retract(); // TODO: change to extend

    // ---------------------
    // test codes
    // ---------------------
    // this->leftFP.extend();
    // delay(3000);
    // this->leftFP.retract();
    // delay(3000);

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
        res = this->eStopL.run();
        if (res == NULL)
        {
            this->eStopCs.readShuntVoltage();
            this->eStopCs.readBusVoltage();
        }
        break;
    }
    case MOVETO:
    {
        res = this->moveMotor.run();
        break;
    }
    case EXTEND_ARM:
    {
        res = this->armMotor.run();
        break;
    }
    case HOME_ARM:
    {
        if (!this->armHoming.getIsCheckingHome())
        {
            res = this->armMotor.run();
            // this->armHoming.clearIsCheckingHome();F
            if (res != NULL)
            {
                this->armHoming.startCheckingHome(res);
                res = NULL;
            }     
        }
        else
        {
            // check that arm is home
            res = this->armHoming.run();
            if (res != NULL)
            {
                this->armHoming.clearIsCheckingHome();
            }
        }
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
        // taps on move motor to communicate with roboteq to get battery readings
        if (this->moveMotor.available())
        {
            // retreive voltage reading
            int voltageReading = this->moveMotor.getRoboteqFeedback();
            if (voltageReading == INT16_MIN)
                break;

            if (voltageReading > BATTERY_MAX_V)
                voltageReading = BATTERY_MAX_V;

            // convert voltage reading to percentage result
            double percDouble = (((double)voltageReading - BATTERY_MIN_V) / (BATTERY_MAX_V - BATTERY_MIN_V));

            int percentage = (int)(percDouble * 1000);

            // convert percentage to string
            static char percentageStr[DEFAULT_CHARARR_BLOCK_SIZE];
            percentageStr[0] = '\0';
            // sprintf(percentageStr, "%d", percentage);
            itoa(percentage, percentageStr, 10);
            res = percentageStr;
        }
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
    // reject battery command if shuttle is busy
    if (action == SLAVE_BATTERY && this->currentStep != Num_Master_Actions_Enums)
    {
        char *reason = NAKSTR "Slave chip busy.";
        this->masterInstance->onStepCompletion(action, reason);
        return;
    }

    // record new step to perform
    this->currentStep = action;
    strcpy(this->currentStepInstructions, inst);

    switch (this->currentStep)
    {
    case ENGAGE_ESTOP:
    {
        this->eStopL.extend();
        break;
    }
    case DISENGAGE_ESTOP:
    {
        this->eStopL.retract();
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
        if (!this->moveMotor.moveTo(this->currentStepInstructions))
        {
            // shuttle is already at the target position
            this->feedbackStepCompletion(this->currentStepInstructions);
        }
        break;
    }
    case READ_BIN_SENSOR:
    {
        static char readValStr[DEFAULT_CHARARR_BLOCK_SIZE];
        readValStr[0] = '\0';

        double readVal;
        BinSensor *activeSensor = NULL;
        ENUM_EXTENSION_DIRECTION scanDirection;

        // determine bin sensor to read
        int directionalValue = atoi(this->currentStepInstructions);

        if (directionalValue < ENUM_EXTENSION_DIRECTION::LEFT)
        {
            // scan using left bin sensor
            activeSensor = &this->leftBs;
            scanDirection = ENUM_EXTENSION_DIRECTION::LEFT;
        }
        else if (directionalValue > ENUM_EXTENSION_DIRECTION::RIGHT)
        {
            // scan using right bin sensor
            activeSensor = &this->rightBs;
            scanDirection = ENUM_EXTENSION_DIRECTION::RIGHT;
        }

        // scan for bin
        if (activeSensor == NULL)
            this->feedbackStepCompletion(NAKSTR "No bin sensor found");

        readVal = scanDirection * activeSensor->aRead();

        // feedback to master scan result
        // sprintf(readValStr, "%d", (int)readVal);
        itoa((int)readVal, readValStr, 10);
        this->feedbackStepCompletion(readValStr);

        break;
    }
    case EXTEND_ARM:
    {
        this->armMotor.extend(this->currentStepInstructions);
        break;
    }
    case HOME_ARM:
    {
        this->armMotor.home();
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
        // taps on movemotor to communicate with roboteq to get battery readings
        this->moveMotor.requestBatteryLevel();
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
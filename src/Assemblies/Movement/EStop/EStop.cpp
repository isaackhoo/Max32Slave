#include "Assemblies/Movement/EStop/EStop.h"

// --------------------------------
// ESTOP PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// ESTOP PUBLIC METHODS
// --------------------------------
EStop::EStop(){};

EStop::EStop(int aPin, int dP1, int dP2, int pwrPin, int pwmPin) : AnalogComms(aPin, INPUT), DCMotor(dP1, dP2, pwrPin, pwmPin)
{
};

char *EStop::run()
{
    char *res = NULL;

    if (millis() - this->timeStart <= ESTOP_TIMEOUT_DURATION)
    {
        double extensionValue = this->aRead();
        static char resStr[DEFAULT_CHARARR_BLOCK_SIZE];

        switch (this->lastInstructions)
        {
        case ENUM_ESTOP_EXTENSION::ESTOP_EXTEND:
        {
            if (extensionValue >= ESTOP_EXTENDED)
            {
                itoa((int)this->lastInstructions, resStr, 10);
                res = resStr;
            }

            break;
        }
        case ENUM_ESTOP_EXTENSION::ESTOP_RETRACT:
        {
            if (extensionValue <= ESTOP_RETRACTED)
            {
                itoa((int)this->lastInstructions, resStr, 10);
                res = resStr;
            }
            break;
        }
        default:
            break;
        }
    }
    else
    {
        // timed out
        res = NAKSTR "Estop timed out";
    }

    return res;
};

void EStop::extend()
{
    this->lastInstructions = ENUM_ESTOP_EXTENSION::ESTOP_EXTEND;
    this->timeStart = millis();
    DCMotor::extend();
};

void EStop::retract()
{
    this->lastInstructions = ENUM_ESTOP_EXTENSION::ESTOP_RETRACT;
    this->timeStart = millis();
    DCMotor::retract();
};

// --------------------------------
// ESTOP PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// ESTOPPAIR PRIVATE METHODS
// --------------------------------
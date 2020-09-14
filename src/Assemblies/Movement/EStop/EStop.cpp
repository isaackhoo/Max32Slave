#include "Assemblies/Movement/EStop/EStop.h"
#include "Logger/Logger.h"

// --------------------------------
// ESTOP PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// ESTOP PUBLIC METHODS
// --------------------------------
EStop::EStop(){};

EStop::EStop(int aPin, int dP1, int dP2, int pwrPin, int pwmPin) : AnalogComms(aPin, INPUT), DCMotor(dP1, dP2, pwrPin, pwmPin){};

char *EStop::run()
{
    char *res = NULL;

    if (millis() - this->timeStart <= ESTOP_TIMEOUT_DURATION)
    {
        double extensionValue = this->aRead();
        static char resStr[DEFAULT_CHARARR_BLOCK_SIZE];
        resStr[0] = '\0';

        switch (this->lastInstructions)
        {
        case ENUM_ESTOP_EXTENSION::ESTOP_EXTEND:
        {
            if (extensionValue >= ESTOP_EXTENDED)
            {
                logger.out("Estop extended");
                // sprintf(resStr, "%d", (int)extensionValue);
                itoa((int)extensionValue, resStr, 10);
                res = resStr;
            }

            break;
        }
        case ENUM_ESTOP_EXTENSION::ESTOP_RETRACT:
        {
            if (extensionValue <= ESTOP_RETRACTED)
            {
                logger.out("Estop retracted");
                // sprintf(resStr, "%d", (int)extensionValue);
                itoa((int)extensionValue, resStr, 10);
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
    logger.out("Extending Estop");
};

void EStop::retract()
{
    this->lastInstructions = ENUM_ESTOP_EXTENSION::ESTOP_RETRACT;
    this->timeStart = millis();
    DCMotor::retract();
    logger.out("Retracting Estop");
};

// --------------------------------
// ESTOP PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// ESTOPPAIR PRIVATE METHODS
// --------------------------------
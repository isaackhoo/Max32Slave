#include "Assemblies/Arm/ArmMotor/ArmMotor.h"
#include "Logger/Logger.h"

// --------------------------------
// ARMMOTOR PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// ARMMOTOR PUBLIC METHODS
// --------------------------------
ArmMotor::ArmMotor(){};

ArmMotor::ArmMotor(HardwareSerial *ss) : Roboteq(ss)
{
    // set motor mode
    this->setMode(ARM_POSITION);
    // set accleration
    this->setAccleration(ARM_ACCLERATION);
    // set deceleration
    this->setDeceleration(ARM_DECELERATION);

    // initialize variables
    this->targetPositionCount = 0;
    this->timeStart = 0;
    this->lastPositionCount = 0;
    this->isHoming = false;
};

char *ArmMotor::run()
{
    char *res = NULL;

    if (millis() - this->timeStart <= ARM_TIMEOUT_DURATION)
    {
        // check current position of arm extension
        if (this->available())
        {
            int absCount = this->getRoboteqFeedback();          // returns absolute value
            if (absCount == INT16_MAX || absCount == INT16_MIN) // abs value overflows to negative min
                return res;

            int direction = this->getRoboteqRawFeedback() < 0 ? -1 : 1;
            int directionalCount = absCount * direction;

            int limit1 = this->targetPositionCount - ARM_POSITION_COUNT_TOLERANCE;
            int limit2 = this->targetPositionCount + ARM_POSITION_COUNT_TOLERANCE;

            int minLimit = limit1 < limit2 ? limit1 : limit2;
            int maxLimit = limit1 < limit2 ? limit2 : limit1;
            int maxAbsLimit = abs(limit1) < abs(limit2) ? abs(limit2) : abs(limit1);

            // update last position read
            if (directionalCount <= maxLimit)
            {
                this->lastPositionCount = directionalCount;
            }

            if (minLimit <= directionalCount && directionalCount <= maxLimit)
            {
                // feed back that arm has reached position
                static char armCount[DEFAULT_CHARARR_BLOCK_SIZE];
                itoa(directionalCount, armCount, 10);
                res = armCount;
                logger.logCpy("Arm position ");
                logger.logCat(armCount);
                logger.logCat(" reached");
                logger.out();
            }
            else if (!this->isHoming && abs(directionalCount) > maxAbsLimit)
            {
                res = NAKSTR "Arm over-extended";
            }

            if (res == NULL)
                this->requestPositionCount();
        }
    }
    else
        res = NAKSTR "Arm timed out";

    return res;
};

void ArmMotor::extend(const char *val)
{
    this->moveTo(val);
};

void ArmMotor::home()
{
    // move arm
    this->isHoming = true;

    char homeCmd[DEFAULT_CHARARR_BLOCK_SIZE];
    itoa(ARM_POSITION_COUNT_HOME, homeCmd, 10);
    this->moveTo(homeCmd);
};

// --------------------------------
// ARMMOTOR PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// ARMMOTOR PRIVATE METHODS
// --------------------------------
void ArmMotor::moveTo(const char *directionalDepth)
{
    logger.logCpy("Moving arm to ");
    logger.logCat(directionalDepth);
    logger.out();

    // instructions already contains the direction and value
    int count = atoi(directionalDepth);

    // update target position
    this->targetPositionCount = count;

    // start timing arm
    this->timeStart = millis();

    // move arm
    this->setPositionCount(count);

    // start reading position
    this->requestPositionCount();
};
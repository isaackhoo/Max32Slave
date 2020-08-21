#include "Assemblies/Arm/ArmMotor/ArmMotor.h"

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
            int absCount = this->getRoboteqFeedback(); // returns absolute value
            int direction = this->getRoboteqRawFeedback() < 0 ? -1 : 1;
            int directionalCount = absCount * direction;

            int absMinLimit = abs(this->targetPositionCount) - ARM_POSITION_COUNT_TOLERANCE;
            int absMaxLimit = abs(this->targetPositionCount) + ARM_POSITION_COUNT_TOLERANCE;

            // update last position read
            if (absCount <= absMaxLimit)
            {
                this->lastPositionCount = directionalCount;
            }

            if (absCount >= absMinLimit && absCount <= absMaxLimit)
            {
                // feed back that arm has reached position
                static char armCount[DEFAULT_CHARARR_BLOCK_SIZE];
                itoa(directionalCount, armCount, 10);
                res = armCount;
            }
            else if (!this->isHoming && absCount > absMinLimit)
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
    // instructions already contains the direction and value
    int count = atoi(directionalDepth);

    // update target position
    this->targetPositionCount = count;

    // move arm
    this->setPositionCount(count);

    // start reading position
    this->requestPositionCount();
};
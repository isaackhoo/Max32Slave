#include "Assemblies/Arm/FingerPair/FingerPair.h"
#include "Logger/Logger.h"

// -------------------------------------------- FINGER --------------------------------------------
// --------------------------------
// FINGER PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// FINGER PUBLIC METHODS
// --------------------------------
void Finger::resetAvgCurrentReadings()
{
    this->avgCurrentDraw = 0;
    this->totalCurrentReads = 0;
};

void Finger::appendCurrentReading(double current)
{
    this->avgCurrentDraw = ((this->avgCurrentDraw * this->totalCurrentReads) + current) / (this->totalCurrentReads + 1);
    this->totalCurrentReads += 1;
};

double Finger::getAvgCurrent()
{
    return this->avgCurrentDraw;
};

void Finger::recordRetractionAvgCurrentRead()
{
    this->retractionAvgCurrentRead = this->getAvgCurrent();
    this->resetAvgCurrentReadings();
};

double Finger::getRetractionAvgCurrentRead()
{
    return this->retractionAvgCurrentRead;
};

void Finger::setMovementStarted(bool didStart)
{
    this->movementStarted = didStart;
};

bool Finger::getIsMovementStarted()
{
    return this->movementStarted;
};

// --------------------------------
// FINGER PRIVATE VARIABLES
// --------------------------------
Finger::Finger(){};

Finger::Finger(CurrentSensor *cs, int dPin1, int dPin2, int pwrPin, int pwmPin) : DCMotor(dPin1, dPin2, pwrPin, pwmPin)
{
    this->cs = cs;
};

// --------------------------------
// FINGERPAIR PRIVATE METHODS
// --------------------------------

// -------------------------------------------- FINGERPAIR --------------------------------------------
// --------------------------------
// FINGERPAIR PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// FINGERPAIR PUBLIC METHODS
// --------------------------------
FingerPair::FingerPair(){};

FingerPair::FingerPair(
    int direction,
    CurrentSensor *frontCs,
    int frontP1,
    int frontP2,
    int frontPwrPasstru,
    int frontPwm,
    CurrentSensor *rearCs,
    int rearP1,
    int rearP2,
    int rearPwrPasstru,
    int rearPwm)
{
    this->direction = direction;
    this->frontFinger = Finger(frontCs, frontP1, frontP2, frontPwrPasstru, frontPwm);
    this->rearFinger = Finger(rearCs, rearP1, rearP2, rearPwrPasstru, rearPwm);

    this->timeStart = 0;
};

char *FingerPair::run()
{
    char *res = NULL;

    unsigned int currentMillis = millis();
    unsigned int millisDiff = currentMillis - this->timeStart;

    if (millisDiff >= FINGER_DELAY_BEFORE_READING && millisDiff <= FINGER_TIMEOUT_DURATION)
    {
        double frontCurrent = this->frontFinger.cs->readCurrent();
        double rearCurrent = this->rearFinger.cs->readCurrent();

        // record finger average current draw during movement
        if (frontCurrent > FINGER_CURRENT_THRESHOLD)
            this->frontFinger.appendCurrentReading(frontCurrent);
        if (rearCurrent > FINGER_CURRENT_THRESHOLD)
            this->rearFinger.appendCurrentReading(rearCurrent);

        // check that finger movement started
        if (!this->frontFinger.getIsMovementStarted())
            if (frontCurrent >= FINGER_INITIAL_MIN_CURRENT_DRAW)
                this->frontFinger.setMovementStarted(true);

        if (!this->rearFinger.getIsMovementStarted())
            if (rearCurrent >= FINGER_INITIAL_MIN_CURRENT_DRAW)
                this->rearFinger.setMovementStarted(true);

        if (frontCurrent <= FINGER_CURRENT_THRESHOLD && rearCurrent <= FINGER_CURRENT_THRESHOLD)
        {
            static char result[DEFAULT_CHARARR_BLOCK_SIZE];
            sprintf(result, "%d", this->direction);
            // itoa(this->direction, result, 10);
            res = result;

            logger.out("Finger pair curr under min");

            // if any finger didnt manage to start movement
            if (!this->frontFinger.getIsMovementStarted() || !this->rearFinger.getIsMovementStarted())
                res = NAKSTR "Finger failed to move";
        }
    }
    else if (millisDiff > FINGER_TIMEOUT_DURATION)
    {
        res = NAKSTR "Finger timed out";

        if (this->lastMovement == FINGER_EXTENSION)
            this->retract();
        else if (this->lastMovement == FINGER_RETRACTION)
            this->extend();
    }

    return res;
};

void FingerPair::powerOn()
{
    this->frontFinger.on();
    this->rearFinger.on();
};

void FingerPair::powerOff()
{
    this->frontFinger.off();
    this->rearFinger.off();
};

bool FingerPair::extend()
{
    logger.logCpy("Extending ");
    this->direction == -1 ? logger.logCat("left") : logger.logCat("right");
    logger.logCat(" finger pair");

    this->initializeFingersBeforeMove();
    this->frontFinger.extend();
    this->rearFinger.extend();
    this->lastMovement = FINGER_EXTENSION;
};

bool FingerPair::retract()
{
    logger.logCpy("Retracting ");
    this->direction == -1 ? logger.logCat("left") : logger.logCat("right");
    logger.logCat(" finger pair");

    this->initializeFingersBeforeMove();
    this->frontFinger.retract();
    this->rearFinger.retract();
    this->lastMovement = FINGER_RETRACTION;
};

// --------------------------------
// FINGERPAIR PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// FINGERPAIR PRIVATE METHODS
// --------------------------------
void FingerPair::initializeFingersBeforeMove()
{
    // reset finger inital current spike read
    this->frontFinger.setMovementStarted(false);
    this->rearFinger.setMovementStarted(false);

    // reset finger current readings
    this->frontFinger.resetAvgCurrentReadings();
    this->rearFinger.resetAvgCurrentReadings();

    // start time millis
    this->timeStart = millis();
};
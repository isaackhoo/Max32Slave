#include "Assemblies/Movement/MoveMotor/MoveMotor.h"
#include "Logger/Logger.h"

// ---------------------------------------------- MOVE SENSOR ----------------------------------------------
// --------------------------------
// MOVESENSOR PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// MOVESENSOR PUBLIC METHODS
// --------------------------------
MoveSensor::MoveSensor(){};

MoveSensor::MoveSensor(int pin) : DigitalSensor(pin, INPUT)
{
    this->counter = 0;
    this->isInHole = false;
};

bool MoveSensor::run(ENUM_MOVEMENT_DIRECTION direction)
{
    // check for a change in values
    if (this->run(OUT_HOLE))
    {
        // detected change
        // determine if should increment or decrement counter
        if (direction == FORWARD)
            this->incrementCounter();
        else if (direction == REVERSE)
            this->decrementCounter();
        return true;
    };

    return false;
};

bool MoveSensor::run(ENUM_MOTORSENSOR_READING changeType)
{
    // store last value read
    int lastVal = this->getLastReadVal();

    // read value
    int val = this->dRead();

    // update in-hole status
    if (val == IN_HOLE)
        this->isInHole = true;
    else if (val == OUT_HOLE)
        this->isInHole = false;

    // prepare results
    bool res = false;

    // determine which type of change to look out for
    switch (changeType)
    {
    case IN_HOLE:
    {
        // looks for out-hole to in-hole
        if (lastVal == OUT_HOLE && val == IN_HOLE)
        {
            res = true;
        }
        break;
    }
    case OUT_HOLE:
    {
        // looks for in-hole to out-hole
        if (lastVal == IN_HOLE && val == OUT_HOLE)
        {
            res = true;
        }
        break;
    }
    default:
        break;
    }

    return res;
};

ENUM_MOTORSENSOR_READING MoveSensor::run()
{
    // store last value read
    int lastVal = this->getLastReadVal();

    // read value
    int val = this->dRead();

    // update in-hole status
    if (val == IN_HOLE)
        this->isInHole = true;
    else if (val == OUT_HOLE)
        this->isInHole = false;

    // determine change type
    if (lastVal == IN_HOLE && val == OUT_HOLE)
        return OUT_HOLE;
    else if (lastVal == OUT_HOLE && val == IN_HOLE)
        return IN_HOLE;
    else
        return NO_CHANGE;
}

void MoveSensor::onToggle()
{
    // code to run when toggled to be active
    // perform an inital read to update current read state
    this->dRead();
};

void MoveSensor::setCounter(int count)
{
    this->counter = count;
};

void MoveSensor::incrementCounter()
{
    this->counter += 1;
};

void MoveSensor::decrementCounter()
{
    this->counter -= 1;
};

int MoveSensor::getCount() { return this->counter; };

bool MoveSensor::getIsInhole() { return this->isInHole; };

// --------------------------------
// MOVESENSOR PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// MOVESENSOR PRIVATE METHODS
// --------------------------------

// ---------------------------------------------- MOVE MOTOR ----------------------------------------------
// --------------------------------
// MOVEMOTOR PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// MOVEMOTOR PUBLIC METHODS
// --------------------------------
MoveMotor::MoveMotor(){};

MoveMotor::MoveMotor(HardwareSerial *ss, int dP1, int dP2, int brakeP1, int brakeP2, int brakePinPwr, int brakePinPwm) : Roboteq(ss)
{
    this->brake = Brake(brakeP1, brakeP2, brakePinPwr, brakePinPwm);
    this->frontSensor = MoveSensor(dP1);
    this->rearSensor = MoveSensor(dP2);

    this->leadingSensor = NULL;
    this->trailingSensor = NULL;
    this->readingSensor = NULL;

    this->targetSlothole = DEFAULT_STARTING_SLOTHOLE;
    this->currentSlothole = DEFAULT_STARTING_SLOTHOLE;

    this->lastMovementDirection = ENUM_MOVEMENT_DIRECTION::NOT_MOVING;
    this->originalMovementDirection = ENUM_MOVEMENT_DIRECTION::NOT_MOVING;

    this->initializeMovementVariables();
};

bool MoveMotor::moveTo(const char *slothole)
{
    // initialize any movement variables
    this->initializeMovementVariables();

    // set target slothole
    this->targetSlothole = atoi(slothole);

    // determine slothole spread
    int difference = abs(this->currentSlothole - this->targetSlothole);

    // determine timeout duration for movement
    this->movementTimeoutDuration = this->calcMovementTimeoutDuration(difference);

    // determine direction
    ENUM_MOVEMENT_DIRECTION direction;
    if (this->targetSlothole > this->currentSlothole)
        direction = FORWARD;
    else if (this->targetSlothole < this->currentSlothole)
        direction = REVERSE;
    else
        direction = NOT_MOVING;

    // check if shuttle is already at current slothole
    if (direction == NOT_MOVING)
        return false;
    else
    {
        // set direction of movement
        this->currentMovementDirection = direction;
        this->originalMovementDirection = direction;
    }

    // determine leading and trailing sensors
    switch (direction)
    {
    case REVERSE:
    {
        this->leadingSensor = &this->rearSensor;
        this->trailingSensor = &this->frontSensor;
        break;
    }
    case FORWARD:
    default:
    {
        this->leadingSensor = &this->frontSensor;
        this->trailingSensor = &this->rearSensor;
        break;
    }
    }

    // set trailing sensor to start reading first
    this->toggleReadingSensor(this->trailingSensor);

    // set movement motor mode
    this->setMode(ENUM_ROBOTEQ_CONFIG::SPEED);

    // get initial sensor readings
    this->frontSensor.dRead();
    this->rearSensor.dRead();

    // log out movement
    logger.logCpy("Moving to slothole ");
    logger.logCat(this->targetSlothole);
    logger.logCat(" from ");
    logger.logCat(this->currentSlothole);
    logger.out();

    // disengage brakes
    if (this->brake.getIsEngaged())
        this->disengageBrake();

    // start moving shuttle
    this->updateMoveSpeed(difference);

    // update shuttle
    return true;
};

char *MoveMotor::run()
{
    char *res = NULL;

    // // ------------------------------
    // // Check for movement timeout
    // // ------------------------------
    // if (millis() - this->movementStartMillis <= this->movementTimeoutDuration)
    // {
    //     if (!this->movementComplete)
    //     {
    //         // ------------------------------
    //         // Run alternate sensor
    //         // ------------------------------
    //         if (!this->isPreparingStop && this->altSensor->run(IN_HOLE))
    //         {
    //             if (this->getMode() == SPEED)
    //             {
    //                 logger.errOut("Alt sensor In-hole");
    //                 logger.logCpy(this->altSensor == this->leadingSensor ? "L: " : "T: ");
    //                 logger.logCat("L ");
    //                 logger.logCat(this->leadingSensor->getCount());
    //                 logger.logCat(" T ");
    //                 logger.logCat(this->trailingSensor->getCount());
    //                 logger.errOut();

    //                 // run the outhole event of failed reading sensor
    //                 this->onOutHoleEvent(this->readingSensor);

    //                 // perform the in hole event of alternate sensor
    //                 this->onInHoleEvent(this->altSensor);

    //                 // toggle reading sensor
    //                 this->toggleReadingSensor();

    //                 logger.logCpy("Correction: ");
    //                 logger.logCat("L ");
    //                 logger.logCat(this->leadingSensor->getCount());
    //                 logger.logCat(" T ");
    //                 logger.logCat(this->trailingSensor->getCount());
    //                 logger.errOut();
    //             }
    //         }

    //         // ------------------------------
    //         // Run reading sensor
    //         // ------------------------------
    //         switch (this->readingSensor->run())
    //         {
    //         case IN_HOLE:
    //         {
    //             this->onInHoleEvent(this->readingSensor);
    //             break;
    //         }
    //         case OUT_HOLE:
    //         {
    //             // log out-hole event
    //             this->readingSensor == this->trailingSensor ? logger.logCpy("T ") : logger.logCpy("L ");
    //             logger.logCat("out-hole ");
    //             logger.logCat(this->readingSensor->getCount());
    //             logger.out();
    //             // perform outhole event
    //             this->onOutHoleEvent(this->readingSensor);
    //             // toggle sensor
    //             this->toggleReadingSensor();
    //             break;
    //         }
    //         default:
    //             break;
    //         }

    //         // ------------------------------
    //         // Run RPM Events
    //         // ------------------------------
    //         if (this->isPreparingStop)
    //         {
    //             // Read RPMS
    //             if (!this->available() && millis() - this->getLastQueryMillis() >= RPM_REQ_INTERVAL)
    //             {
    //                 logger.out("Req RPM");
    //                 this->requestRpm();
    //             }
    //             else if (this->available())
    //             {
    //                 // Look for RPM events
    //                 int rpm = this->getRoboteqFeedback();
    //                 if (rpm == INT16_MIN)
    //                     return res;

    //                 logger.logCpy("RPM: ");
    //                 logger.logCat(rpm);

    //                 if (this->getMode() == SPEED && rpm <= MIN_SPEED_RPM)
    //                 {
    //                     // check that shuttle rpm falls below min threshold
    //                     this->movementComplete = true;
    //                 }
    //                 else if (this->getMode() == R_POSITION && rpm <= MIN_CREEP_RPM)
    //                 {
    //                     if (this->creepCount < DEFAULT_MAX_CREEPS)
    //                     {
    //                         // disengage brakes
    //                         if (this->brake.getIsEngaged())
    //                             this->disengageBrake();

    //                         this->setRelativePosition(this->currentMovementDirection * CREEP_VALUE);

    //                         // update creep count
    //                         this->creepCount += 1;

    //                         // update last creep millis
    //                         this->lastCreepMillis = millis();
    //                     }
    //                     else
    //                     {
    //                         res = NAKSTR "Max creeps reached";
    //                     }
    //                 }
    //             }
    //         };
    //     }
    //     else if (this->movementComplete)
    //     {
    //         // ------------------------------
    //         // On Complete event
    //         // ------------------------------
    //         // check that shuttle is in hole
    //         if ((ENUM_MOTORSENSOR_READING)this->leadingSensor->dRead() == IN_HOLE)
    //         {
    //             // set trailing sensor count to leading sensor count
    //             this->trailingSensor->setCounter(this->leadingSensor->getCount());

    //             // change mode back to speed to release creep belt tension
    //             this->setMode(SPEED);

    //             // feedback movement complete
    //             res = this->createSlotholeArriveSuccessStr();
    //         }
    //         else
    //         {
    //             if (this->creepTries <= MAX_REPEAT_CREEPS)
    //             {
    //                 // reset terminating vars
    //                 this->movementComplete = false;
    //                 this->movementStoppedMillis = 0;
    //                 // get shuttle to reverse creep
    //                 this->setMode(R_POSITION);
    //                 // reverse movement direction
    //                 this->currentMovementDirection = this->currentMovementDirection == FORWARD ? REVERSE : FORWARD;
    //                 // set reading sensor to leading
    //                 this->toggleReadingSensor(this->leadingSensor);
    //                 // reset creep variables
    //                 this->creepCount = 0;
    //                 // increment creep tries
    //                 this->creepTries += 1;
    //                 // log
    //                 logger.out("Reverse Creeping");
    //                 this->currentMovementDirection == FORWARD ? logger.out("FORWARD") : logger.out("REVERSE");
    //             }
    //             else
    //             {
    //                 res = NAKSTR "Exceed creep retries";
    //             }
    //         }
    //     }
    // }
    // else
    // {
    //     if (!this->movementComplete)
    //     {
    //         this->immediateStop();
    //         res = NAKSTR "Movement timed out";
    //     }
    // }

    // --------------------------------
    // Check for movement timeout
    // --------------------------------
    if (millis() - this->movementStartMillis >= this->movementTimeoutDuration)
    {
        this->immediateStop();
        res = NAKSTR "Movement timed out";
        return res;
    }

    if (!this->movementComplete)
    {
        // ------------------------------
        // Run alternate sensor
        // ------------------------------
        if (!this->speedComplete && this->altSensor->run(IN_HOLE))
        {
            if (this->getMode() == SPEED)
            {
                logger.errOut("Alt sensor In-hole");
                logger.logCpy(this->altSensor == this->leadingSensor ? "L: " : "T: ");
                logger.logCat("L ");
                logger.logCat(this->leadingSensor->getCount());
                logger.logCat(" T ");
                logger.logCat(this->trailingSensor->getCount());
                logger.errOut();

                // run the outhole event of failed reading sensor
                this->onOutHoleEvent(this->readingSensor);

                // perform the in hole event of alternate sensor
                this->onInHoleEvent(this->altSensor);

                // toggle reading sensor
                this->toggleReadingSensor();

                logger.logCpy("Correction: ");
                logger.logCat("L ");
                logger.logCat(this->leadingSensor->getCount());
                logger.logCat(" T ");
                logger.logCat(this->trailingSensor->getCount());
                logger.errOut();
            }
        }

        // ------------------------------
        // Run reading sensor
        // ------------------------------
        switch (this->readingSensor->run())
        {
        case IN_HOLE:
        {
            this->onInHoleEvent(this->readingSensor);
            break;
        }
        case OUT_HOLE:
        {
            // if its trailing sensor, log out first
            if (this->readingSensor == this->trailingSensor)
            {
                logger.logCpy("T ");
                logger.logCat("out-hole ");
                logger.logCat(this->readingSensor->getCount());
                logger.out();
            }

            // perform outhole event
            this->onOutHoleEvent(this->readingSensor);

            // if its leading sensor, log out after
            if (this->readingSensor == this->leadingSensor)
            {
                logger.logCpy("L ");
                logger.logCat("out-hole ");
                logger.logCat(this->readingSensor->getCount());
                logger.out();
            }
            // toggle sensor
            this->toggleReadingSensor();
            break;
        }
        default:
            break;
        }

        // ------------------------
        // Run RPM Events
        // ------------------------
        if (this->speedComplete && this->getMode() == SPEED)
        {
            if (!this->hasRequestedRPM && !this->available())
            {
                if (millis() - this->getLastQueryMillis() >= RPM_REQ_INTERVAL)
                {
                    this->requestRpm();
                    this->hasRequestedRPM = true;
                }
            }
            else
            {
                int rpm = this->getRoboteqFeedback();
                this->hasRequestedRPM = false;
                if (rpm == INT16_MIN)
                    return res; // throw away invalid results

                if (this->getMode() == SPEED && rpm <= MIN_SPEED_RPM)
                {
                    this->movementComplete = true;
                }
            }
        }

        // ------------------------
        // Run Creep Events
        // ------------------------
        if (this->getMode() == R_POSITION)
        {
            // check if shuttle can still creep
            if (this->creepCount > DEFAULT_MAX_CREEPS)
                res = NAKSTR "Max creeps reached";
            else
            {
                if (millis() - this->lastCreepMillis >= POSITION_CONTINUOUS_CREEP_DELAY)
                {
                    // disengage brakes
                    if (this->brake.getIsEngaged())
                        this->disengageBrake();

                    this->setRelativePosition(this->currentMovementDirection * CREEP_VALUE);

                    // update creep count
                    this->creepCount += 1;

                    // update last creep millis
                    this->lastCreepMillis = millis();
                }
            }
        }
    }
    else
    {
        // ----------------------
        // Movement Complete
        // ----------------------
        if (this->getMode() == SPEED || (this->getMode() == R_POSITION && millis() - this->movementStoppedMillis >= MOVEMENT_COMPLETION_SETTLE_DELAY))
        {
            if ((ENUM_MOTORSENSOR_READING)this->leadingSensor->dRead() == IN_HOLE)
            {
                // movement ended successfully
                // set trailing sensor count to leading sensor count
                this->trailingSensor->setCounter(this->leadingSensor->getCount());

                // feedback movement complete
                res = this->createSlotholeArriveSuccessStr();
            }
            else
            {
                // shuttle sensor is not in hole. change to creep mode
                if (this->creepTries > MAX_REPEAT_CREEPS)
                {
                    res = NAKSTR "Max creep retries";
                }
                else
                {
                    // change mode
                    this->setMode(R_POSITION);

                    // reset terminating vars
                    this->movementComplete = false;
                    this->movementStoppedMillis = 0;

                    // reverse movement direction
                    this->currentMovementDirection = this->currentMovementDirection == FORWARD ? REVERSE : FORWARD;
                    // set reading sensor to leading
                    this->toggleReadingSensor(this->leadingSensor);

                    // reset creep variables
                    this->creepCount = 0;
                    // increment creep tries
                    this->creepTries += 1;
                    // log
                    logger.out("Reverse Creeping");
                    this->currentMovementDirection == FORWARD ? logger.out("FORWARD") : logger.out("REVERSE");
                }
            }
        }
    }

    return res;
};

void MoveMotor::cutShuttleSpeed()
{
    // stop speed
    this->setSpeedPercent(0);
};

void MoveMotor::heavyStop()
{
    // cut speed
    this->cutShuttleSpeed();

    // immediate deceleration
    this->setDeceleration(IM_DEC);
};

void MoveMotor::immediateStop()
{
    // engage brakes - physically theres a time delay to this
    if (!this->brake.getIsEngaged())
        this->engageBrake();

    // perform heavy stop
    this->heavyStop();
};

void MoveMotor::updateCurrentSlothole(const char *slothole)
{
    int slotholeInt = atoi(slothole);
    this->currentSlothole = slotholeInt;

    // update sensor counts
    this->frontSensor.setCounter(slotholeInt);
    this->rearSensor.setCounter(slotholeInt);

    logger.out("slotholes updated");
    logger.logCpy("current: ");
    logger.logCat(this->currentSlothole);
    logger.logCat(" F ");
    logger.logCat(this->frontSensor.getCount());
    logger.logCat(" R ");
    logger.logCat(this->rearSensor.getCount());
    logger.out();
};

void MoveMotor::engageBrake()
{
    this->brake.engage();
};

void MoveMotor::disengageBrake()
{
    this->brake.disengage();
};

// --------------------------------
// MOVEMOTOR PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// MOVEMOTORPAIR PRIVATE METHODS
// --------------------------------
void MoveMotor::initializeMovementVariables()
{
    // initialize control variables
    this->lastMoveSpeed = 0;

    // initialize millis variables
    this->movementTimeoutDuration = 0;
    this->movementStartMillis = millis();
    this->movementStoppedMillis = DEFAULT_MOVEMENT_MILLIS;

    // initialize movement control variables
    this->movementComplete = false;
    this->hasTrailingReadFirst = false;

    // initialize speed movement variables
    this->isPreparingStop = false;
    this->leadStoppedOuthole = false;
    this->speedComplete = false;
    this->hasRequestedRPM = false;

    // initialize creep variables
    this->creepCount = 0;
    this->lastCreepMillis = 0;
    this->creepTries = 0;
};

unsigned int MoveMotor::calcMovementTimeoutDuration(int diff)
{
    int multiplier = 1;

    if (diff >= 22)
        multiplier = TIMEOUT_MULTIPLIER_5;
    else if (diff >= 14)
        multiplier = TIMEOUT_MULTIPLIER_4;
    else if (diff >= 7)
        multiplier = TIMEOUT_MULTIPLIER_3;
    else if (diff >= 2)
        multiplier = TIMEOUT_MULTIPLIER_2;
    else
        multiplier = TIMEOUT_MULTIPLIER_1;

    // get timeout duration
    return (unsigned int)(multiplier * MOVEMENT_TIMEOUT_PER_SLOTHOLE * diff);
};

bool MoveMotor::updateMoveSpeed(int diff)
{
    int speed = 0;

    if (diff >= 22)
        speed = SPEED_5;
    else if (diff >= 14)
        speed = SPEED_4;
    else if (diff >= 7)
        speed = SPEED_3;
    else if (diff >= 2)
        speed = SPEED_2;
    else
        speed = SPEED_1;

    // implement movement direction
    speed *= this->currentMovementDirection;

    if (this->lastMoveSpeed != speed)
    {
        logger.logCpy("Move speed updated to ");
        logger.logCat(speed);
        logger.out();

        // ensure that the new speed is not higher than the prev speed.
        // there is no scenario where the shuttle increases speed like that.
        // acceleration to max speed will happen on initial speed change

        if (this->lastMoveSpeed != 0 && abs(this->lastMoveSpeed) < abs(speed))
        {
            // error occured. shuttle is has over run slothole
            logger.errOut("Emergency stop shuttle");
            this->immediateStop();
            return false;
        }

        // update speed
        this->setSpeedPercent(speed);
        this->lastMoveSpeed = speed;
        return true;
    }
};

void MoveMotor::updateCurrentSlothole(int slothole)
{
    this->currentSlothole = slothole;
};

void MoveMotor::toggleReadingSensor(MoveSensor *sensor)
{
    this->readingSensor = sensor;
    this->altSensor = this->readingSensor == this->leadingSensor ? this->trailingSensor : this->leadingSensor;
    // get current reading of sensors
    this->readingSensor->onToggle();
    this->altSensor->onToggle();
};

void MoveMotor::toggleReadingSensor()
{
    this->toggleReadingSensor(this->readingSensor == this->leadingSensor ? this->trailingSensor : this->leadingSensor);
};

bool MoveMotor::updateSensorOutHoleCount(MoveSensor *sensor)
{
    if (sensor == NULL)
        return false;

    sensor->setCounter(sensor->getCount() + (this->currentMovementDirection * 1));

    // update shuttle counter if sensor is leading
    if (sensor == this->leadingSensor)
        this->updateCurrentSlothole(this->leadingSensor->getCount());
    return true;
};

// ------------------------
// Speed Events
// ------------------------

bool MoveMotor::onSpeedLeadInHoleEvt()
{
    // check for last slothole event
    if (this->leadingSensor->getCount() == (this->targetSlothole - (this->currentMovementDirection * 1)))
    {
        // leading sensor is in last slothole
        // this->immediateStop();
        // this->isPreparingStop = true;
        // this->speedComplete = true;
        this->heavyStop();
    }

    return true;
};

bool MoveMotor::onSpeedTrailInHoleEvt()
{
    // check for last slothole event
    if (this->trailingSensor->getCount() == this->targetSlothole)
        this->immediateStop();
    return true;
};

bool MoveMotor::onSpeedLeadOutHoleEvt()
{
    bool res;

    res = this->updateSensorOutHoleCount(this->leadingSensor);

    // stop shuttle if its the last slothole
    if (this->leadingSensor->getCount() == this->targetSlothole)
    {
        this->leadStoppedOuthole = true;
        this->immediateStop();
        this->isPreparingStop = true;
        this->speedComplete = true;
    }

    return res;
};

bool MoveMotor::onSpeedTrailOutHoleEvt()
{
    bool res;

    if (!this->hasTrailingReadFirst)
        this->hasTrailingReadFirst = true;

    res = this->updateSensorOutHoleCount(this->trailingSensor);
    // update movement speed
    res = this->updateMoveSpeed(abs(this->trailingSensor->getCount() - this->targetSlothole));

    return res;
};

// ------------------------
// Creep Events
// ------------------------

bool MoveMotor::onCreepLeadInHoleEvt()
{
    // stop the shuttle
    this->immediateStop();
    this->movementComplete = true;
    this->movementStoppedMillis = millis();

    return true;
};

bool MoveMotor::onCreepTrailInHoleEvt()
{
    return true;
};

bool MoveMotor::onCreepLeadOutHoleEvt()
{
    // terminate creeping movement
    this->immediateStop();
    this->movementComplete = true;
    // shuttle will return to loop to check that sensor is not in-hole and reverse creep again

    return true;
};

bool MoveMotor::onCreepTrailOutHoleEvt()
{
    return true;
};

void MoveMotor::onInHoleEvent(MoveSensor *sensor)
{
    if (this->getMode() == SPEED)
    {
        if (sensor == this->leadingSensor)
            this->onSpeedLeadInHoleEvt();
        else
            this->onSpeedTrailInHoleEvt();
    }
    else if (this->getMode() == R_POSITION)
    {
        if (sensor == this->leadingSensor)
            this->onCreepLeadInHoleEvt();
        else
            this->onCreepTrailInHoleEvt();
    }
};

void MoveMotor::onOutHoleEvent(MoveSensor *sensor)
{
    if (this->getMode() == SPEED)
    {
        if (sensor == this->leadingSensor)
            this->onSpeedLeadOutHoleEvt();
        else
            this->onSpeedTrailOutHoleEvt();
    }
    else if (this->getMode() == R_POSITION)
    {
        if (sensor == this->leadingSensor)
            this->onCreepLeadOutHoleEvt();
        else
            this->onCreepTrailOutHoleEvt();
    }
};

char *MoveMotor::createSlotholeArriveSuccessStr()
{
    static char currentSlotholeStr[DEFAULT_CHARARR_BLOCK_SIZE];
    currentSlotholeStr[0] = '\0';
    itoa(this->currentSlothole, currentSlotholeStr, 10);

    return currentSlotholeStr;
};
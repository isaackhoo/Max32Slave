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

    this->initializeMovementVariables();
};

char *MoveMotor::run()
{
    char *res = NULL;

    MoveSensor *altSensor = this->readingSensor == this->leadingSensor ? this->trailingSensor : this->leadingSensor;

    // ensure that the other senor does not trigger an IN_HOLE event before reading sensor does an OUT_HOLE
    // run this only during movement
    if (!this->isFirstSlotholeRead && !this->isPreparingStop && millis() - this->movementStartMillis >= ALT_SENSOR_READ_DELAY && altSensor->run(IN_HOLE))
    {
        logger.errOut("Resolving miscount");
        logger.logCpy(altSensor == this->leadingSensor ? "L: " : "T: ");
        logger.logCat("L ");
        logger.logCat(this->leadingSensor->getCount());
        logger.logCat(" T ");
        logger.logCat(this->trailingSensor->getCount());
        logger.errOut();
        // update the count of the missed reading sensor
        this->readingSensor->setCounter(this->readingSensor->getCount() + (this->currentMovementDirection * 1));
        // run the out-hole event of the sensor that missed its reading
        if (this->getMode() == SPEED)
        {
            if (this->readingSensor == this->leadingSensor)
                this->onSpeedLeadOutHoleEvt();
            else if (this->readingSensor == this->trailingSensor)
                this->onSpeedTrailOutHoleEvt();
        }
        else if (this->getMode() == R_POSITION)
        {
            if (this->readingSensor == this->leadingSensor)
                this->onCreepLeadOutHoleEvt();
            else if (this->readingSensor == this->trailingSensor)
                this->onCreepTrailOutHoleEvt();
        }

        // toggle the reading sensor
        this->toggleReadingSensor();
        // perform in hole event
        if (this->getMode() == SPEED)
        {
            if (this->readingSensor == this->leadingSensor)
                this->onSpeedLeadInHoleEvt();
            else if (this->readingSensor == this->trailingSensor)
                this->onSpeedTrailInHoleEvt();
        }
        else if (this->getMode() == R_POSITION)
        {
            if (this->readingSensor == this->leadingSensor)
                this->onCreepLeadInHoleEvt();
            else if (this->readingSensor == this->trailingSensor)
                this->onCreepTrailInHoleEvt();
        }
    }

    // run reading sensor normally
    switch (this->readingSensor->run())
    {
    case IN_HOLE:
    {
        // this->readingSensor == this->trailingSensor ? logger.logCpy("T ") : logger.logCpy("L ");
        // logger.logCat("In-hole ");
        // logger.logCat(this->readingSensor->getCount());
        // logger.out();

        switch (this->getMode())
        {
        case SPEED:
        {
            if (this->readingSensor == this->leadingSensor)
            {
                this->onSpeedLeadInHoleEvt();
            }
            else if (this->readingSensor == this->trailingSensor)
            {
                this->onSpeedTrailInHoleEvt();
            }
            break;
        }
        case R_POSITION:
        {
            if (this->readingSensor == this->leadingSensor)
            {
                this->onCreepLeadInHoleEvt();
            }
            else if (this->readingSensor == this->trailingSensor)
            {
                this->onCreepTrailInHoleEvt();
            }
            break;
        }
        default:
            break;
        }
        break;
    }
    case OUT_HOLE:
    {
        // update sensor count
        this->readingSensor->setCounter(this->readingSensor->getCount() + (this->currentMovementDirection * 1));

        // log out-hole event
        this->readingSensor == this->trailingSensor ? logger.logCpy("T ") : logger.logCpy("L ");
        logger.logCat("out-hole ");
        logger.logCat(this->readingSensor->getCount());
        logger.out();

        // check that shuttle should event be moving
        // this is to capture events where the shuttle overruns the target slothole
        // if it has, stop the shuttle and return an error message
        bool shouldContinue = true;
        if (this->currentMovementDirection == FORWARD)
        {
            // count should be less than target slothole
            if (this->readingSensor->getCount() > this->targetSlothole)
            {
                shouldContinue = false;
            }
        }
        else if (this->currentMovementDirection == REVERSE)
        {
            // count should be greater than target slothole
            if (this->readingSensor->getCount() < this->targetSlothole)
            {
                shouldContinue = false;
            }
        }

        if (!shouldContinue)
        {
            res = NAKSTR "Shuttle over ran slothole";
            this->immediateStop();
        }
        else
        {
            // determine action based on mode
            switch (this->getMode())
            {
            case SPEED:
            {
                // readings match up
                if (this->readingSensor == this->leadingSensor)
                {
                    this->onSpeedLeadOutHoleEvt();
                }
                else if (this->readingSensor == this->trailingSensor)
                {
                    this->onSpeedTrailOutHoleEvt();
                }
                break;
            }
            case R_POSITION:
            {
                if (this->readingSensor == this->leadingSensor)
                {
                    this->onCreepLeadOutHoleEvt();
                }
                else if (this->readingSensor == this->trailingSensor)
                {
                    this->onCreepTrailOutHoleEvt();
                    res = NAKSTR "Creep overshot";
                }
                break;
            }
            default:
                break;
            }

            // toggle reading sensor
            this->toggleReadingSensor();
        } // end check to continue

        break;
    }
    case NO_CHANGE:
    default:
        break;
    }

    // look for RPM event
    if (this->getMode() == SPEED && !this->movementComplete && this->isPreparingStop && this->available())
    {
        int rpm = this->getRoboteqFeedback();

        if (rpm != INT16_MIN && rpm <= MIN_SPEED_RPM)
        {
            // shuttle fell below min rpm speed. change modes
            // check trailing sensor should be in hole
            this->trailingSensor->dRead();

            if (this->trailingSensor->getLastReadVal() == IN_HOLE)
            {
                logger.out("T ended in hole");
                this->movementComplete = true;
            }
            else
            {
                // trailing sensor out of hole
                logger.out("changing to creep mode");
                this->setMode(R_POSITION);

                if (this->trailingSensor->getCount() == this->targetSlothole + (this->currentMovementDirection * 1))
                {
                    // shuttle exceeded slothole
                    logger.out("reversing shuttle creep direction");
                    // reverse movement
                    this->shouldReverseCreep = true;
                    this->currentMovementDirection = this->currentMovementDirection == FORWARD ? REVERSE : FORWARD;
                }
            }
        };
    };

    // continue reads for rpm
    if (this->getMode() == SPEED && !this->movementComplete && this->isPreparingStop && !this->available())
    {
        if (millis() - this->getLastQueryMillis() >= RPM_REQ_INTERVAL)
        {
            // continue reading
            this->requestRpm();
        }
    }

    // look for creep event
    if (this->getMode() == R_POSITION && !this->movementComplete)
    {
        if (this->creepCount < DEFAULT_MAX_CREEPS)
        {
            // disengage brakes
            if (this->lastCreepMillis == 0)
                this->disengageBrake();

            // creep shuttle forward every delay
            if (millis() - this->lastCreepMillis >= POSITION_CONTINUOUS_CREEP_DELAY)
            {
                // creep shuttle
                this->setRelativePosition(this->currentMovementDirection * CREEP_VALUE);

                // update creep count
                this->creepCount += 1;

                // update last creep millis
                this->lastCreepMillis = millis();
            }
        }
        else
            res = NAKSTR "Max creeps reached";
    };

    // look for movement complete event
    if (this->movementComplete)
    {
        res = this->createSlotholeArriveSuccessStr();
    }

    return res;
};

bool MoveMotor::moveTo(const char *slothole)
{
    // set target slothole
    this->targetSlothole = atoi(slothole);

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
        // set direction of movement
        this->currentMovementDirection = direction;

    // set movement motor mode
    this->setMode(ENUM_ROBOTEQ_CONFIG::SPEED);

    // reset sensor counts
    this->frontSensor.setCounter(this->currentSlothole);
    this->rearSensor.setCounter(this->currentSlothole);

    // initialize any movement variables
    this->initializeMovementVariables();

    // get initial sensor readings
    this->frontSensor.dRead();
    this->rearSensor.dRead();

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

    // disengage brakes
    this->disengageBrake();

    // set trailing sensor to start reading first
    this->toggleReadingSensor(this->trailingSensor);

    // determine slothole spread
    int difference = abs(this->currentSlothole - this->targetSlothole);

    logger.logCpy("Moving to slothole ");
    logger.logCat(this->targetSlothole);
    logger.logCat(" from ");
    logger.logCat(this->currentSlothole);
    logger.out();

    // set movement start millis
    this->movementStartMillis = millis();

    // start moving shuttle
    this->updateMoveSpeed(difference);

    // update shuttle
    return true;
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
    // initialize movement control variables
    this->movementComplete = false;
    this->movementStartMillis = 0;

    // initialize speed movement variables
    this->lastMoveSpeed = 0;
    this->isFirstSlotholeRead = true;
    this->isPreparingStop = false;
    this->hasStopped = false;
    this->stoppedMillis = DEFAULT_MOVEMENT_MILLIS;

    // initialize creep variables
    this->shouldReverseCreep = false;
    this->creepCount = 0;
    this->lastCreepMillis = 0;
};

void MoveMotor::updateMoveSpeed(int diff)
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

        // update speed
        this->setSpeedPercent(speed);
        this->lastMoveSpeed = speed;
    }
};

void MoveMotor::toggleReadingSensor(MoveSensor *sensor)
{
    this->readingSensor = sensor;
    // reset the reading of the sensor
    this->readingSensor->dRead();
};

void MoveMotor::toggleReadingSensor()
{
    MoveSensor *sensor = NULL;
    sensor = this->readingSensor == this->leadingSensor ? this->trailingSensor : this->leadingSensor;
    this->toggleReadingSensor(sensor);
};

void MoveMotor::onSpeedLeadInHoleEvt()
{
    // check for last slothole event
    if (this->leadingSensor->getCount() == (this->targetSlothole - (this->currentMovementDirection * 1)))
    {
        // toggle stopped var
        this->isPreparingStop = true;
        // leading sensor is in last slothole
        this->immediateStop();
    }
};

void MoveMotor::onSpeedTrailInHoleEvt()
{
    // check for last slothole event
    if (this->trailingSensor->getCount() == this->targetSlothole)
    {
        // trailing sensor is in last slothole
        this->immediateStop();
    }
};

void MoveMotor::onSpeedLeadOutHoleEvt()
{
    // update shuttle position
    this->currentSlothole = this->leadingSensor->getCount();
};

void MoveMotor::onSpeedTrailOutHoleEvt()
{
    // update movement speed
    this->updateMoveSpeed(abs(this->trailingSensor->getCount() - this->targetSlothole));

    if (this->isFirstSlotholeRead)
        this->isFirstSlotholeRead = false;
};

void MoveMotor::onCreepLeadInHoleEvt(){
    // do nothing
};

void MoveMotor::onCreepTrailInHoleEvt()
{
    // check for last slothole event
    if (this->trailingSensor->getCount() == this->targetSlothole)
    {
        // trailing sensor is in last slothole
        this->immediateStop();

        // change back the mode
        this->setMode(SPEED);

        // check if movement was reversed
        if (this->shouldReverseCreep)
        {
            // make count correction on trailing sensor
            this->trailingSensor->setCounter(this->leadingSensor->getCount());
        }

        // immediate terminate movement
        this->movementComplete = true;
    }
};

void MoveMotor::onCreepLeadOutHoleEvt()
{
    // update shuttle position
    this->currentSlothole = this->leadingSensor->getCount();
};

void MoveMotor::onCreepTrailOutHoleEvt()
{
    // this should not happen
    logger.errOut("Trailing sensor missed slothole");

    // stop shuttle
    this->immediateStop();
};

char *MoveMotor::onSensorCountMismatch()
{
    logger.errOut("!!! Sensor count mismatch !!!");
    logger.logCpy("L ");
    logger.logCat(this->leadingSensor->getCount());
    logger.logCat(" T ");
    logger.logCat(this->trailingSensor->getCount());
    logger.errOut();

    // stop shuttle
    this->immediateStop();

    return NAKSTR "sensors count mismatched";
};

char *MoveMotor::createSlotholeArriveSuccessStr()
{
    static char currentSlotholeStr[DEFAULT_CHARARR_BLOCK_SIZE];
    currentSlotholeStr[0] = '\0';
    //csprintf(currentSlotholeStr, "%d", this->currentSlothole);
    itoa(this->currentSlothole, currentSlotholeStr, 10);

    return currentSlotholeStr;
};
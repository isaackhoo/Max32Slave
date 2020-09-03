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

// char *MoveMotor::run()
// {
//     char *res = NULL;

//     // start off at speed mode
//     switch (this->getMode())
//     {
//     case ENUM_ROBOTEQ_CONFIG::SPEED:
//     {
//         // trailing sensor will toggle to read first
//         if (!this->isStopping && this->readingSensor->run(this->currentMovementDirection))
//         {
//             // reading sensor has detected in-hole to out-hole event
//             // determine which sensor it is
//             if (this->readingSensor == this->trailingSensor)
//             {
//                 logger.logCpy("Trailing sensor out-hole ");
//                 logger.logCat(this->trailingSensor->getCount());
//                 logger.out();
//                 // update movement speed - increase or decrease speeds based on distance from target
//                 this->updateMoveSpeed(abs(this->trailingSensor->getCount() - this->targetSlothole));

//                 // check if last slothole has been scanned
//                 // prepare to stop shuttle if it has
//                 if (this->trailingSensor->getCount() == this->targetSlothole)
//                 {
//                     this->isStopping = true;
//                 }
//             }
//             else if (this->readingSensor == this->leadingSensor)
//             {
//                 logger.logCpy("Leading sensor out-hole ");
//                 logger.logCat(this->leadingSensor->getCount());
//                 logger.out();

//                 // verify that counts match up with trailing sensor
//                 if (this->leadingSensor->getCount() != this->trailingSensor->getCount())
//                 {
//                     logger.errOut("!!! count mismatch detected !!!");
//                     logger.logCpy("Leading ");
//                     logger.logCat(this->leadingSensor->getCount());
//                     logger.logCat(" Trailing ");
//                     logger.logCat(this->trailingSensor->getCount());
//                     logger.errOut();

//                     if (this->currentMovementDirection == ENUM_MOVEMENT_DIRECTION::FORWARD)
//                     {
//                         // trailing count should be >= leading count
//                         if (this->trailingSensor->getCount() < this->leadingSensor->getCount())
//                         {
//                             // force trailing count to be leading count
//                             this->trailingSensor->setCounter(this->leadingSensor->getCount());
//                         }
//                         else
//                         {
//                             // force leading count to be trailing count
//                             this->leadingSensor->setCounter(this->trailingSensor->getCount());
//                         }
//                     }
//                     else if (this->currentMovementDirection == ENUM_MOVEMENT_DIRECTION::REVERSE)
//                     {
//                         // trailing count should be <= leading count
//                         if (this->trailingSensor->getCount() > this->leadingSensor->getCount())
//                         {
//                             // force trailing count to be elading count
//                             this->trailingSensor->setCounter(this->leadingSensor->getCount());
//                         }
//                         else
//                         {
//                             // force leading count to be trailing count
//                             this->leadingSensor->setCounter(this->trailingSensor->getCount());
//                         }
//                     }
//                 }

//                 // update shuttle position
//                 this->currentSlothole = this->leadingSensor->getCount();
//             }

//             // toggle reading sensor
//             this->readingSensor = this->readingSensor == this->trailingSensor ? this->leadingSensor : this->trailingSensor;
//         }
//         // shuttle is preparing to stop
//         // check laser for in-hole event
//         else if (this->isStopping && this->leadingSensor->run(IN_HOLE))
//         {
//             // leading sensor detected out-hole to in-hole event
//             if (this->onLastSlotholeArrival())
//             {
//                 // movement is completed successfully
//                 // at least one lazer was found in hole
//                 res = this->createSlotholeArriveSuccessStr();
//             }
//             else
//             {
//                 // leading sensor dectected slothole, but both sensors out of hole
//                 // should get shuttle to reverse back into hole
//                 this->shouldReverseCreep = true;

//                 // heavy brake
//                 this->setDeceleration(HIGH_DEC);

//                 // change motor mode
//                 this->setMode(R_POSITION);
//                 this->creepCount = 0;
//                 this->lastCreepMillis = 0;

//                 // reverse the direction of travel
//                 this->currentMovementDirection = this->currentMovementDirection == FORWARD ? REVERSE : FORWARD;
//                 // switch leading and trailing sensors
//                 MoveSensor *temp = this->leadingSensor;
//                 this->leadingSensor = this->trailingSensor;
//                 this->trailingSensor = temp;

//                 logger.out("Switching to creep mode");
//             }
//         }
//         break;
//     }
//     case ENUM_ROBOTEQ_CONFIG::R_POSITION:
//     {
//         // uses relative position mode to creep to slothole
//         // once either of the sensors have entered in-hole, creeping stops
//         if (this->creepCount < DEFAULT_MAX_CREEPS)
//         {
//             // creep shuttle forward after every delay
//             if (millis() - this->lastCreepMillis >= POSITION_CONTINUOUS_CREEP_DELAY)
//             {
//                 // disengage brakes if its the first creep
//                 if (this->lastCreepMillis == 0)
//                     this->disengageBrake();

//                 // creep shuttle
//                 this->setRelativePosition(this->currentMovementDirection * CREEP_VALUE); // roboteq controls for relative position movement

//                 // update creep count
//                 this->creepCount += 1;

//                 // update last creep millis
//                 this->lastCreepMillis = millis();
//             };

//             // look out for either sensor in-hole event
//             if (this->leadingSensor->run(IN_HOLE) || this->trailingSensor->run(IN_HOLE))
//             {
//                 // no need to check for lastSlotholeArrival bool to prevent endless adjestments
//                 this->onLastSlotholeArrival();

//                 // terminate movement
//                 res = this->createSlotholeArriveSuccessStr();
//             }
//         }
//         else
//             res = NAKSTR "Max creeps reached";
//         break;
//     }
//     default:
//         res = NAKSTR "Invalid move mode";
//         break;
//     }

//     return res;
// };

char *MoveMotor::run()
{
    char *res = NULL;

    // keep reading for out-hole event, if sensor is currently in hole
    if (this->readingSensor->getIsInhole() == true && this->readingSensor->run(this->currentMovementDirection))
    {
        this->readingSensor == this->leadingSensor ? logger.logCpy("Leading ") : logger.logCpy("Trailing ");
        logger.logCat(" sensor out-hole ");
        logger.logCat(this->readingSensor->getCount());
        logger.out();

        // handle out of hole event
        // determine which sensor
        // trailing sensor out-hole event
        if (this->readingSensor == this->trailingSensor)
        {
            if (!this->isPreparingStop)
            {
                // update movement speed
                this->updateMoveSpeed(abs(this->trailingSensor->getCount() - this->targetSlothole));

                // check if second last slothole has been scanned
                if (this->trailingSensor->getCount() == this->targetSlothole)
                {
                    this->isPreparingStop = true;
                    logger.out("is preparing to stop");
                }
            };
        }
        // leading sensor out-hole event
        else if (this->readingSensor == this->leadingSensor)
        {
            // verify that trailing and leading sensor counts are consistent
            if (this->leadingSensor->getCount() != this->trailingSensor->getCount())
            {
                logger.errOut("!!! count mismatch detected !!!");
                logger.logCpy("Leading ");
                logger.logCat(this->leadingSensor->getCount());
                logger.logCat(" Trailing ");
                logger.logCat(this->trailingSensor->getCount());
                logger.errOut();

                if (this->currentMovementDirection == ENUM_MOVEMENT_DIRECTION::FORWARD)
                {
                    // trailing count should be >= leading count
                    if (this->trailingSensor->getCount() < this->leadingSensor->getCount())
                    {
                        // force trailing count to be leading count
                        this->trailingSensor->setCounter(this->leadingSensor->getCount());
                        logger.logCpy("Trailing sensor count forced to ");
                        logger.logCat(this->trailingSensor->getCount());
                    }
                    else
                    {
                        // force leading count to be trailing count
                        this->leadingSensor->setCounter(this->trailingSensor->getCount());
                        logger.logCpy("Leading sensor count forced to ");
                        logger.logCat(this->leadingSensor->getCount());
                    }
                }
                else if (this->currentMovementDirection == ENUM_MOVEMENT_DIRECTION::REVERSE)
                {
                    // trailing count should be <= leading count
                    if (this->trailingSensor->getCount() > this->leadingSensor->getCount())
                    {
                        // force trailing count to be elading count
                        this->trailingSensor->setCounter(this->leadingSensor->getCount());
                        logger.logCpy("Trailing sensor count forced to ");
                        logger.logCat(this->trailingSensor->getCount());
                    }
                    else
                    {
                        // force leading count to be trailing count
                        this->leadingSensor->setCounter(this->trailingSensor->getCount());
                        logger.logCpy("Leading sensor count forced to ");
                        logger.logCat(this->leadingSensor->getCount());
                    }
                }
                logger.out();
            }

            // update shuttle position
            this->currentSlothole = this->leadingSensor->getCount();
            logger.logCpy("current slothole: ");
            logger.logCat(this->currentSlothole);
            logger.out();
        }; // end determine which sensor read

        // alternate the sensor
        this->readingSensor = this->readingSensor == this->leadingSensor ? this->trailingSensor : this->leadingSensor;
    }; // end read out-hole event

    // read for in-hole event, if sensor is currently out-hole
    if (this->readingSensor->getIsInhole() == false && this->readingSensor->run(IN_HOLE))
    {

        this->readingSensor == this->leadingSensor ? logger.logCpy("Leading ") : logger.logCpy("Trailing ");
        logger.logCat(" sensor in-hole ");
        logger.logCat(this->readingSensor->getCount());
        logger.out();
        // determine which sensor
        // trailing sensor
        if (this->readingSensor == this->trailingSensor)
        {
            if (this->hasStopped && !this->movementComplete)
            {
                logger.out("trailing im_stop");

                this->engageBrake();

                // terminate movement
                this->movementComplete = true;

                // update final slothole
                res = this->createSlotholeArriveSuccessStr();
            }
        }
        // leading sensor
        else if (this->readingSensor == this->leadingSensor)
        {
            if (!this->hasStopped && this->isPreparingStop && this->getMode() == ENUM_ROBOTEQ_CONFIG::SPEED)
            {
                logger.out("Leading sensor trigger stop");
                // terminate shuttle speed
                this->heavyStop();
                // toggle stop variable
                this->hasStopped = true;
                // update time that heavy stop was called
                this->stoppedMillis = millis();
                // start reading RPM
                this->requestRpm();
            }
        }
    }

    // check for RPM reading. if it falls below min, change mode to RELATIVE_POSITION
    if (this->hasStopped && this->available())
    {
        int rpm = this->getRoboteqFeedback();

        logger.logCpy(rpm);
        logger.out();

        if (rpm <= MIN_SPEED_RPM)
        {
            logger.out("min RPM reached");
            // switch to relative position mode
            this->setMode(R_POSITION);

            // determine if shuttle should reverse creep or not
            // shuttle should only reverse creep if trailing sensor count is != target slothole
            // if trailing sensor count changed, it means it has gone past the slot hole
            if (this->trailingSensor->getCount() != this->targetSlothole)
            {
                // even if direction of travel is reversed
                // do not flip the leading and trailing sensors as we still want to get the trailing sensor in-hole

                // reverse the direction of travel
                this->currentMovementDirection = this->currentMovementDirection == FORWARD ? REVERSE : FORWARD;
            }
        }
    }

    // check if mode has changed to relative position
    if (!this->movementComplete && this->getMode() == R_POSITION && this->creepCount <= DEFAULT_MAX_CREEPS)
    {
        // shuttle should creep until trailing sensor finds in-hole event
        // creep shuttle periodically
        if (millis() - this->lastCreepMillis >= POSITION_INITIAL_CREEP_DELAY)
        {
            // creep shuttle
            this->setRelativePosition(this->currentMovementDirection * CREEP_VALUE);

            // increment number of creeps performed
            this->creepCount += 1;

            // update last creep millis
            this->lastCreepMillis = millis();
        }
    }

    return res;
};

bool MoveMotor::moveTo(const char *slothole)
{
    // set target slothole
    this->targetSlothole = atoi(slothole);

    // check if shuttle is already at current slothole
    if (this->currentSlothole == this->targetSlothole)
        return false;

    // set movement motor mode
    this->setMode(ENUM_ROBOTEQ_CONFIG::SPEED);

    // initialize any movement variables
    this->initializeMovementVariables();

    // determine direction
    ENUM_MOVEMENT_DIRECTION direction;
    if (this->targetSlothole > this->currentSlothole)
        direction = FORWARD;
    else if (this->targetSlothole < this->currentSlothole)
        direction = REVERSE;
    else
        direction = NOT_MOVING;

    // set direction of movement
    this->currentMovementDirection = direction;

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
    this->readingSensor = this->trailingSensor;

    // determine last slothole timeout based on current slothole and target slothole
    // this->determineLastSlotholeTimeoutDuration();

    // determine slothole spread
    int difference = abs(this->currentSlothole - this->targetSlothole);

    logger.logCpy("Moving to slothole ");
    logger.logCat(this->targetSlothole);
    logger.logCat(" from ");
    logger.logCat(this->currentSlothole);
    logger.out();

    // start moving shuttle
    this->updateMoveSpeed(difference);

    // update shuttle
    return true;
};

void MoveMotor::cutShuttleSpeed()
{
    // stop speed
    this->setSpeedPercent(0);

    logger.out("Shuttle speed cut to 0");
};

void MoveMotor::heavyStop()
{
    // cur speed
    this->cutShuttleSpeed();

    // immediate deceleration
    this->setDeceleration(IM_DEC);
    logger.out("Deceleration IM_DEC");
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

    // initialize speed movement variables
    this->isPreparingStop = false;
    this->hasStopped = false;
    this->stoppedMillis = DEFAULT_MOVEMENT_MILLIS;

    // initialize creep variables
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

    logger.logCpy("Move speed updated to ");
    logger.logCat(speed);
    logger.out();

    // update speed
    this->setSpeedPercent(speed);
};

// void MoveMotor::determineLastSlotholeTimeoutDuration()
// {
//     // from battery to lifter, from lifter to battery
//     if (this->targetSlothole == SLOTHOLE_BATTERY || (this->currentSlothole == SLOTHOLE_BATTERY && this->targetSlothole == SLOTHOLE_LIFTER))
//         this->lastSlotholeStopTimeoutDuration = BATTERY_LIFTER;

//     // from rack buffer to lifter, from lifter to rack buffer
//     else if ((this->currentSlothole > SLOTHOLE_LIFTER && this->targetSlothole == SLOTHOLE_LIFTER) || (this->currentSlothole == SLOTHOLE_LIFTER && this->targetSlothole == SLOTHOLE_BUFFER))
//         this->lastSlotholeStopTimeoutDuration = LIFTER_BUFFER;

//     // from rack buffer to first slothole, from rack to rack buffer
//     else if ((this->currentSlothole > SLOTHOLE_BUFFER && this->targetSlothole == SLOTHOLE_BUFFER) || this->currentSlothole == SLOTHOLE_BUFFER && this->targetSlothole == SLOTHOLE_BUFFER + 1)
//         this->lastSlotholeStopTimeoutDuration = BUFFER_RACK;

//     // between columns, first slothole
//     else if (this->currentSlothole < this->targetSlothole && this->targetSlothole % SLOTHOLES_PER_COL == 1)
//         this->lastSlotholeStopTimeoutDuration = NEXT_COL_FIRST_SLOT;

//     // between columns, last slothole
//     else if (this->currentSlothole > this->targetSlothole && this->targetSlothole % SLOTHOLES_PER_COL == 0)
//         this->lastSlotholeStopTimeoutDuration = NEXT_COL_FIRST_SLOT;

//     // within column
//     else
//         this->lastSlotholeStopTimeoutDuration = WITHIN_COL;
// };

// bool MoveMotor::onLastSlotholeArrival()
// {
//     // stop shuttle
//     this->immediateStop();

//     // update leading sensor count
//     if (this->leadingSensor->getCount() != this->trailingSensor->getCount())
//     {
//         this->leadingSensor->setCounter(this->trailingSensor->getCount());
//     }

//     // update current slothole
//     this->currentSlothole = this->trailingSensor->getCount();

//     // give shuttle time to settle
//     delay(150);

//     // check if at least one of the sensors are in hole
//     if (this->leadingSensor->dRead() == IN_HOLE || this->trailingSensor->dRead() == IN_HOLE)
//         return true;
//     return false;
// };

char *MoveMotor::createSlotholeArriveSuccessStr()
{
    static char currentSlotholeStr[DEFAULT_CHARARR_BLOCK_SIZE] = {'\0'};
    itoa(this->currentSlothole, currentSlotholeStr, 10);

    return currentSlotholeStr;
};
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

    this->lastSlotholeMillis = 0;
    this->lastSlotholeStopTimeoutDuration = 0;
    this->isStopping = false;

    this->creepCount = 0;
    this->lastCreepMillis = 0;
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

    // keep reading for out-hole event
    if (this->readingSensor->run(this->currentMovementDirection))
    {
        this->readingSensor == this->leadingSensor ? logger.logCpy("Leading ") : logger.logCpy("Trailing ");
        logger.logCat(" sensor out-hole ");
        logger.logCat(this->readingSensor->getCount());
        logger.out();

        // handle out of hole event
        if (!this->isStopping)
        {
            // determine which sensor
            if (this->readingSensor == this->trailingSensor)
            {
                // updae movement speed
                this->updateMoveSpeed(this->trailingSensor->getCount() - this->targetSlothole);

                // check if second last slothole has been scanned
                if (this->trailingSensor->getCount() == this->targetSlothole)
                    this->isStopping = true;
            }
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
            };
        }

        // alternate the sensor
        this->readingSensor = this->readingSensor == this->leadingSensor ? this->trailingSensor : this->leadingSensor;
    }; // end read out-hole event

    // read for in-hole event
    if (this->readingSensor->run(IN_HOLE))
    {
        if (this->isStopping)
        {
            
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
    // clear out last slothole timeout
    this->lastSlotholeMillis = DEFAULT_LAST_SLOTHOLE_MILLIS;
    this->isStopping = false;
    // reset shuttle creep control variables
    this->shouldReverseCreep = false;

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
    this->determineLastSlotholeTimeoutDuration();

    // determine slothole spread
    int difference = abs(this->currentSlothole - this->targetSlothole);

    logger.logCpy("Moving to slothole ");
    logger.logCat(this->targetSlothole);
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

void MoveMotor::immediateStop()
{
    // engage brakes - physically theres a time delay to this
    this->engageBrake();

    // immediate deceleration
    this->setDeceleration(IM_DEC);

    // stop speed
    this->setSpeedPercent(0);

    logger.out("Immediate stop performed");
};

void MoveMotor::updateCurrentSlothole(const char *slothole)
{
    int slotholeInt = atoi(slothole);
    this->currentSlothole = slotholeInt;

    // update sensor counts
    this->frontSensor.setCounter(slotholeInt);
    this->rearSensor.setCounter(slotholeInt);
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

void MoveMotor::determineLastSlotholeTimeoutDuration()
{
    // from battery to lifter, from lifter to battery
    if (this->targetSlothole == SLOTHOLE_BATTERY || (this->currentSlothole == SLOTHOLE_BATTERY && this->targetSlothole == SLOTHOLE_LIFTER))
        this->lastSlotholeStopTimeoutDuration = BATTERY_LIFTER;

    // from rack buffer to lifter, from lifter to rack buffer
    else if ((this->currentSlothole > SLOTHOLE_LIFTER && this->targetSlothole == SLOTHOLE_LIFTER) || (this->currentSlothole == SLOTHOLE_LIFTER && this->targetSlothole == SLOTHOLE_BUFFER))
        this->lastSlotholeStopTimeoutDuration = LIFTER_BUFFER;

    // from rack buffer to first slothole, from rack to rack buffer
    else if ((this->currentSlothole > SLOTHOLE_BUFFER && this->targetSlothole == SLOTHOLE_BUFFER) || this->currentSlothole == SLOTHOLE_BUFFER && this->targetSlothole == SLOTHOLE_BUFFER + 1)
        this->lastSlotholeStopTimeoutDuration = BUFFER_RACK;

    // between columns, first slothole
    else if (this->currentSlothole < this->targetSlothole && this->targetSlothole % SLOTHOLES_PER_COL == 1)
        this->lastSlotholeStopTimeoutDuration = NEXT_COL_FIRST_SLOT;

    // between columns, last slothole
    else if (this->currentSlothole > this->targetSlothole && this->targetSlothole % SLOTHOLES_PER_COL == 0)
        this->lastSlotholeStopTimeoutDuration = NEXT_COL_FIRST_SLOT;

    // within column
    else
        this->lastSlotholeStopTimeoutDuration = WITHIN_COL;
};

bool MoveMotor::onLastSlotholeArrival()
{
    // stop shuttle
    this->immediateStop();

    // update leading sensor count
    if (this->leadingSensor->getCount() != this->trailingSensor->getCount())
    {
        this->leadingSensor->setCounter(this->trailingSensor->getCount());
    }

    // update current slothole
    this->currentSlothole = this->trailingSensor->getCount();

    // give shuttle time to settle
    delay(150);

    // check if at least one of the sensors are in hole
    if (this->leadingSensor->dRead() == IN_HOLE || this->trailingSensor->dRead() == IN_HOLE)
        return true;
    return false;
};

char *MoveMotor::createSlotholeArriveSuccessStr()
{
    static char slothole[DEFAULT_CHARARR_BLOCK_SIZE];
    itoa(this->currentSlothole, slothole, 10);
    return slothole;
};
#include "Assemblies/Movement/MoveMotor/MoveMotor.h"

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

    bool res = false;

    // determine which type of change to look out for
    switch (changeType)
    {
    case IN_HOLE:
    {
        // looks for out-hole to in-hole
        if (lastVal == OUT_HOLE && val == IN_HOLE)
            res = true;
        break;
    }
    case OUT_HOLE:
    {
        // looks for in-hole to out-hole
        if (lastVal == IN_HOLE && val == OUT_HOLE)
            res = true;
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

char *MoveMotor::run()
{
    char *res = NULL;

    // start off at speed mode
    switch (this->getMode())
    {
    case ENUM_ROBOTEQ_CONFIG::SPEED:
    {
        // trailing sensor will toggle to read first
        if (!this->isStopping && this->readingSensor->run(this->currentMovementDirection))
        {
            // reading sensor has detected in-hole to out-hole event
            // determine which sensor it is
            if (this->readingSensor == this->trailingSensor)
            {
                Serial.print("Trailing sensor out-hole event: ");
                Serial.println(this->readingSensor->getCount());

                // update movement speed - increase or decrease speeds based on distance from target
                this->updateMoveSpeed(abs(this->trailingSensor->getCount() - this->targetSlothole));

                // check if last slothole has been scanned
                // prepare to stop shuttle if it has
                if (this->trailingSensor->getCount() == this->targetSlothole)
                {
                    this->isStopping = true;
                }
            }
            else if (this->readingSensor == this->leadingSensor)
            {
                Serial.print("Leading sensor out-hole event: ");
                Serial.println(this->readingSensor->getCount());

                // verify that counts match up with trailing sensor
                if (this->leadingSensor->getCount() != this->trailingSensor->getCount())
                {
                    Serial.println("!!! count mismatch detected !!!");
                    Serial.print("Leading ");
                    Serial.print(this->leadingSensor->getCount());
                    Serial.print(" Trailing ");
                    Serial.println(this->trailingSensor->getCount());

                    if (this->currentMovementDirection == ENUM_MOVEMENT_DIRECTION::FORWARD)
                    {
                        Serial.print("[Resolving forward] ");
                        // trailing count should be >= leading count
                        if (this->trailingSensor->getCount() < this->leadingSensor->getCount())
                        {
                            Serial.println("trailing < leading");
                            // force trailing count to be leading count
                            this->trailingSensor->setCounter(this->leadingSensor->getCount());
                        }
                        else
                        {
                            Serial.println("trailing > leading");
                            // force leading count to be trailing count
                            this->leadingSensor->setCounter(this->trailingSensor->getCount());
                        }
                    }
                    else if (this->currentMovementDirection == ENUM_MOVEMENT_DIRECTION::REVERSE)
                    {
                        Serial.print("[Resolving forward] ");
                        // trailing count should be <= leading count
                        if (this->trailingSensor->getCount() > this->leadingSensor->getCount())
                        {
                            Serial.println("trailing > leading");
                            // force trailing count to be elading count
                            this->trailingSensor->setCounter(this->leadingSensor->getCount());
                        }
                        else
                        {
                            Serial.println("trailing < leading");
                            // force leading count to be trailing count
                            this->leadingSensor->setCounter(this->trailingSensor->getCount());
                        }
                    }
                }

                // update shuttle position
                this->currentSlothole = this->leadingSensor->getCount();
            }

            // toggle reading sensor
            this->readingSensor = this->readingSensor == this->trailingSensor ? this->leadingSensor : this->trailingSensor;
        }
        // shuttle is preparing to stop
        // check laser for in-hole event
        else if (this->isStopping && this->leadingSensor->run(IN_HOLE))
        {
            // leading sensor detected out-hole to in-hole event
            if (this->onLastSlotholeArrival())
            {
                // movement is completed successfully
                // at least one lazer was found in hole
                res = this->createSlotholeArriveSuccessStr();
            }
            else
            {
                // leading sensor dectected slothole, but both sensors out of hole
                // should get shuttle to reverse back into hole
                this->shouldReverseCreep = true;

                // heavy brake
                this->setDeceleration(HIGH_DEC);

                // change motor mode
                this->setMode(R_POSITION);
                this->creepCount = 0;
                this->lastCreepMillis = 0;
                // this->modeToggleMillis = millis();

                // reverse the direction of travel
                this->currentMovementDirection = this->currentMovementDirection == FORWARD ? REVERSE : FORWARD;
                // switch leading and trailing sensors
                MoveSensor *temp = this->leadingSensor;
                this->leadingSensor = this->trailingSensor;
                this->trailingSensor = temp;
            }
        }
        break;
    }
    case ENUM_ROBOTEQ_CONFIG::R_POSITION:
    {
        // uses relative position mode to creep to slothole
        // once either of the sensors have entered in-hole, creeping stops
        if (this->creepCount < DEFAULT_MAX_CREEPS)
        {
            // creep shuttle forward after every delay
            if (millis() - this->lastCreepMillis >= POSITION_CONTINUOUS_CREEP_DELAY)
            {
                // disengage brakes if its the first creep
                if (this->lastCreepMillis == 0)
                    this->disengageBrake();

                // creep shuttle
                this->setRelativePosition(this->currentMovementDirection * CREEP_VALUE); // roboteq controls for relative position movement

                // update creep count
                this->creepCount += 1;

                // update last creep millis
                this->lastCreepMillis = millis();
            };

            // look out for either sensor in-hole event
            if (this->leadingSensor->run(IN_HOLE) || this->trailingSensor->run(IN_HOLE))
            {
                // no need to check for lastSlotholeArrival bool to prevent endless adjestments
                this->onLastSlotholeArrival();

                // terminate movement
                res = this->createSlotholeArriveSuccessStr();
            }
        }
        else
            res = NAKSTR "Max creeps reached";
        break;
    }
    default:
        res = NAKSTR "Invalid move mode";
        break;
    }

    return res;

    // // determine movement mode
    // switch (this->getMode())
    // {
    // case ENUM_ROBOTEQ_CONFIG::SPEED:
    // {
    //     // not yet reached last slothole
    //     if (this->lastSlotholeMillis == 0)
    //     {
    //         // trailing laser should always read before the leading laser
    //         if (this->readingSensor->run(this->currentMovementDirection))
    //         {
    //             // reading sensor has scanned from in-hole to out-hole
    //             if (this->readingSensor == this->trailingSensor)
    //             {
    //                 // trailing sensor scanned in-hole to out-hole
    //                 // update movement speed
    //                 this->updateMoveSpeed(abs(this->trailingSensor->getCount() - this->targetSlothole));

    //                 // check if trailing sensor has scanned last slothole
    //                 if (this->trailingSensor->getCount() == this->targetSlothole)
    //                 {
    //                     // prepare timer to stop shuttle
    //                     this->lastSlotholeMillis = millis();
    //                 }
    //             }
    //             else
    //             {
    //                 // leading sensor scanned from in-hole to out-hole
    //                 // validate scans
    //                 if (this->leadingSensor->getCount() != this->trailingSensor->getCount())
    //                 {
    //                     Serial.println("!!!!!! leading sensor count mismatch !!!!!!!");
    //                     Serial.print("Trailing: ");
    //                     Serial.print(this->trailingSensor->getCount());
    //                     Serial.print(" Leading: ");
    //                     Serial.println(this->leadingSensor->getCount());
    //                     // force reset leading sensor count
    //                     this->leadingSensor->setCounter(this->trailingSensor->getCount());
    //                 }

    //                 // update current slothole
    //                 this->currentSlothole = this->leadingSensor->getCount();
    //             }

    //             // toggle reading sensor after each out of hole read event
    //             this->readingSensor = this->readingSensor == this->trailingSensor ? this->leadingSensor : this->trailingSensor;
    //         }
    //     }
    //     // timer has been set for last slothole
    //     else
    //     {
    //         // check if millis has reached to stop shuttle
    //         if (!this->isStopping)
    //         {
    //             if (millis() - this->lastSlotholeMillis >= this->lastSlotholeStopTimeoutDuration)
    //             {
    //                 this->isStopping = true;
    //                 // cut shuttle speed
    //                 // this->cutShuttleSpeed();
    //                 // reduce deceleration to allow shuttle to free wheel towards slothole
    //                 // this->setDeceleration(LOW_DEC);
    //                 // start querying rpm
    //                 this->requestRpm();
    //             }
    //         }
    //         // shuttle is free rolling towards slothole
    //         else
    //         {
    //             // check leading laser for in-hole event
    //             if (this->leadingSensor->run(IN_HOLE))
    //             {
    //                 // in hole event was detected
    //                 if (this->onLastSlotholeArrival())
    //                 {
    //                     // movement is completed successfully
    //                     // at least one lazer was found in hole
    //                     return this->createSlotholeArriveSuccessStr();
    //                 }
    //                 else
    //                 {
    //                     // leading sensor dectected slothole, but both sensors out of hole
    //                     // should get shuttle to reverse back into hole
    //                     Serial.println("reverse creeping shuttle back to slothole");
    //                     this->shouldReverseCreep = true;
    //                     this->shouldCreepPosition = true;
    //                 }
    //             }

    //             // check that rpm has not reached minimum
    //             if (this->available())
    //             {
    //                 int rpm = this->getRoboteqFeedback();

    //                 if (rpm != INT16_MIN && rpm != INT16_MAX)
    //                 {
    //                     if (rpm < POSITION_CREEP_MIN_RPM)
    //                     {
    //                         Serial.println("rpm fell below min. Creeping shuttle towards slothole");
    //                         this->shouldCreepPosition = true;
    //                     }
    //                 }

    //                 // continue querying rpm
    //                 this->requestRpm();
    //             }

    //             // check if mode should be toggled
    //             if (this->shouldCreepPosition)
    //             {
    //                 // heavy brake
    //                 this->setDeceleration(HIGH_DEC);

    //                 // change motor mode
    //                 this->setMode(R_POSITION);
    //                 this->creepCount = 0;
    //                 this->lastCreepMillis = 0;
    //                 this->modeToggleMillis = millis();

    //                 // check for direction reversal
    //                 if (this->shouldReverseCreep)
    //                 {
    //                     // reverse the direction of travel
    //                     this->currentMovementDirection = this->currentMovementDirection == FORWARD ? REVERSE : FORWARD;
    //                     // switch leading and trailing sensors
    //                     MoveSensor *temp = this->leadingSensor;
    //                     this->leadingSensor = this->trailingSensor;
    //                     this->trailingSensor = temp;
    //                 }
    //             }
    //         }
    //     }

    //     break;
    // }
    // case ENUM_ROBOTEQ_CONFIG::R_POSITION:
    // {
    //     // use relative position mode to creep toward target slothole
    //     // Stop creeping once leading sensor has read out-hole to in-hole
    //     if (this->creepCount < DEFAULT_MAX_CREEPS)
    //     {
    //         if ((this->lastCreepMillis == 0 && millis() - this->modeToggleMillis >= POSITION_INITIAL_CREEP_DELAY) || millis() - this->lastCreepMillis >= POSITION_CONTINUOUS_CREEP_DELAY)
    //         {
    //             if (this->lastCreepMillis == 0)
    //                 // disengage brakes
    //                 this->disengageBrake();

    //             // continue creeping shuttle forward
    //             this->setRelativePosition((int)this->currentMovementDirection * CREEP_VALUE);
    //             this->lastCreepMillis = millis();
    //         }

    //         // look out for leading sensor in hole event
    //         if (this->leadingSensor->run(IN_HOLE) || this->trailingSensor->run(IN_HOLE))
    //         {
    //             Serial.println("Creeping found slothole");
    //             // kinda helps to jam break the motor
    //             this->setMode(ENUM_ROBOTEQ_CONFIG::SPEED);

    //             // no need to check for lastSlotholeArrival bool to prevent too many adjustments
    //             this->onLastSlotholeArrival();

    //             // movement is completed successfully
    //             return this->createSlotholeArriveSuccessStr();
    //         }
    //     }
    //     else
    //     {
    //         return NAKSTR "max creeps reached";
    //     }
    //     break;
    // }
    // default:
    // break;
    // }

    // return NULL; // move step incomplete
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

void MoveMotor::immediateStop()
{
    // engage brakes - physically theres a time delay to this
    this->engageBrake();

    // immediate deceleration
    this->setDeceleration(IM_DEC);

    // stop speed
    this->setSpeedPercent(0);
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

    Serial.print("Last slothole timeout set as ");
    Serial.println(this->lastSlotholeStopTimeoutDuration);
};

bool MoveMotor::onLastSlotholeArrival()
{
    // stop shuttle
    this->immediateStop();

    Serial.println("terminating movement");

    // update leading sensor count
    if (this->leadingSensor->getCount() != this->trailingSensor->getCount())
    {
        Serial.print("updating leading sensor count from ");
        Serial.print(this->leadingSensor->getCount());
        Serial.print(" to ");
        Serial.println(this->trailingSensor->getCount());
        this->leadingSensor->setCounter(this->trailingSensor->getCount());
    }

    // update current slothole
    this->currentSlothole = this->trailingSensor->getCount();
    Serial.print("MoveMotor current slothole count updated to ");
    Serial.println(this->currentSlothole);

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
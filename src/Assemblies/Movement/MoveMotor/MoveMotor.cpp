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

    if (val != lastVal)
    {
        Serial.print("last: ");
        Serial.print(lastVal);
        Serial.print(" read: ");
        Serial.println(val);
    }

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
    ++this->counter;
};

void MoveSensor::decrementCounter()
{
    --this->counter;
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

MoveMotor::MoveMotor(HardwareSerial *ss, int dP1, int dP2, int brakeP1, int brakeP2, int brakePinPwr, int brakePinPwm) : SerialComms(ss)
{
    this->brake = Brake(brakeP1, brakeP2, brakePinPwr, brakePinPwm);
    this->frontSensor = MoveSensor(dP1);
    this->rearSensor = MoveSensor(dP2);

    this->leadingSensor = NULL;
    this->trailingSensor = NULL;

    this->currentSlothole = DEFAULT_STARTING_SLOTHOLE;
};

char *MoveMotor::run()
{
    // determine movement mode
    switch (this->currentMotorMode)
    {
    case SPEED:
    {
        // not yet reached last slothole
        if (this->lastSlotholeMillis == 0)
        {
            // trailing laser should always read before the leading laser
            if (this->readingSensor->run(this->currentMovementDirection))
            {
                Serial.print(this->readingSensor == this->trailingSensor ? "Trailing sensor " : "Leading sensor ");
                Serial.print("read count ");
                Serial.println(this->readingSensor->getCount());
                // reading sensor has scanned from in-hole to out-hole
                if (this->readingSensor == this->trailingSensor)
                {
                    // trailing sensor scanned in-hole to out-hole
                    // update movement speed
                    this->updateMoveSpeed(abs(this->trailingSensor->getCount() - this->targetSlothole));

                    // check if trailing sensor has scanned last slothole
                    if (this->trailingSensor->getCount() == this->targetSlothole)
                    {
                        // prepare timer to stop shuttle
                        this->lastSlotholeMillis = millis();
                    }
                }
                else
                {
                    // leading sensor scanned from in-hole to out-hole
                    // validate scans
                    if (this->leadingSensor->getCount() != this->trailingSensor->getCount())
                    {
                        Serial.println("!!!!!! leading sensor count mismatch !!!!!!!");
                        Serial.print("Trailing: ");
                        Serial.print(this->trailingSensor->getCount());
                        Serial.print(" Leading: ");
                        Serial.println(this->leadingSensor->getCount());
                        // force reset leading sensor count
                        this->leadingSensor->setCounter(this->trailingSensor->getCount());
                    }

                    // update current slothole
                    this->currentSlothole = this->leadingSensor->getCount();
                }
                // toggle reading sensor
                this->readingSensor = this->readingSensor == this->trailingSensor ? this->leadingSensor : this->trailingSensor;
            }
        }
        // timer is set for last slothole
        else
        {
            // check if millis has reached to stop shuttle
            if (!this->isStopping)
            {
                if (millis() - this->lastSlotholeMillis >= this->lastSlotholeStopTimeoutDuration)
                {
                    this->isStopping = true;
                    // cut shuttle speed
                    this->send(SPEED_MOVE "0" RBTQ_ENDSTR);
                    // reduce deceleration
                    this->send(MDEC "300" RBTQ_ENDSTR);
                    // start querying rpm
                    this->send(QUERY_RPM RBTQ_ENDSTR);

                    Serial.println("cut shuttle speed to 0, deceleration 300");
                }
            }
            // shuttle is free rolling towards slothole
            else
            {
                // check leading laser for in-hole event
                if (this->leadingSensor->run(IN_HOLE))
                {
                    Serial.println("spd mode leading sensor reached slothole");
                    // in hole event was detected
                    if (this->onLastSlotholeArrival())
                    {
                        // movement is completed successfully
                        char slothole[DEFAULT_CHARARR_BLOCK_SIZE];
                        itoa(this->currentSlothole, slothole, 10);

                        Serial.println(slothole);
                        return slothole;
                    }
                    else
                    {
                        Serial.println("creeping shuttle more towards slothole");
                        this->shouldCreepPosition = true;
                    }
                }

                // check that rpm has not reached 0
                if (this->read(RBTQ_ENDCHAR))
                {
                    int rpm = this->interpretRpmFeedback(this->serialIn);
                    this->clearSerialIn();

                    Serial.println(rpm);

                    if (rpm != INT16_MIN && rpm < POSITION_CREEP_MIN_RPM)
                    {
                        Serial.println("rpm fell below. creeping shuttle");
                        this->shouldCreepPosition = true;
                    }

                    // continue querying rpm
                    this->send(QUERY_RPM RBTQ_ENDSTR);
                }

                // check if mode should be toggled
                if (this->shouldCreepPosition)
                {
                    // jam brake
                    this->send(MDEC "30000" RBTQ_ENDSTR);

                    // change motor mode
                    this->setMotorMode(POSITION);
                    this->creepCount = 0;
                    this->lastCreepMillis = 0;
                }
            }
        }

        break;
    }
    case POSITION:
    {
        // use relative position mode to creep toward target slothole
        // Stop creeping once leading sensor has read out-hole to in-hole
        if (this->creepCount < DEFAULT_MAX_CREEPS)
        {
            if (this->lastCreepMillis == 0 || millis() - this->lastCreepMillis >= POSITION_CREEP_DELAY)
            {
                // disengage brakes
                this->disengageBrake();

                // continue creeping shuttle forward
                this->send(PR_MOVE CREEP_VALUE RBTQ_ENDSTR);
                this->lastCreepMillis = millis();
            }

            // look out for leading sensor in hole event
            if (this->leadingSensor->run(IN_HOLE))
            {
                Serial.println("creeping found slothole");
                this->setMotorMode(SPEED);
                this->onLastSlotholeArrival();

                // movement is completed successfully
                // no need to check for bool to prevent too many adjustments
                char slothole[DEFAULT_CHARARR_BLOCK_SIZE];
                itoa(this->currentSlothole, slothole, 10);

                Serial.println(slothole);
                return slothole;
            }
        }
        else
        {
            return NAKSTR "max creeps reached";
        }
        break;
    }
    default:
        break;
    }

    return NULL; // move step incomplete
};

void MoveMotor::moveTo(const char *slothole)
{
    // set movement motor mode
    this->setMotorMode(SPEED);
    this->shouldCreepPosition = false;

    this->frontSensor.setCounter(0);
    this->rearSensor.setCounter(0);

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

    // set direction of movement
    this->currentMovementDirection = direction;

    // determine leading and trailing sensors
    switch (direction)
    {
    case FORWARD:
    {
        this->leadingSensor = &this->frontSensor;
        this->trailingSensor = &this->rearSensor;
        break;
    }
    case REVERSE:
    {
        this->leadingSensor = &this->rearSensor;
        this->trailingSensor = &this->frontSensor;
        break;
    }
    default:
        break;
    }

    // disengage brakes
    this->disengageBrake();

    // set trailing sensor to start reading first
    this->readingSensor = this->trailingSensor;

    // clear out last slothole timeout
    this->lastSlotholeMillis = DEFAULT_LAST_SLOTHOLE_MILLIS;
    this->isStopping = false;

    // determine last slothole timeout based on current slothole and target slothole
    this->determineLastSlotholeTimeoutDuration();

    // determine slothole spread
    int difference = abs(this->currentSlothole - this->targetSlothole);

    // start moving shuttle
    this->updateMoveSpeed(difference);
};

void MoveMotor::stop()
{
    // immediate deceleration
    // this->send(MDEC SPEED_IM_DEC RBTQ_ENDCHAR);

    // stop speed
    this->send(SPEED_MOVE "0" RBTQ_ENDSTR);
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
void MoveMotor::setMotorMode(ENUM_CLOSED_LOOP_MODES mode)
{
    // set mode
    this->currentMotorMode = mode;
    switch (this->currentMotorMode)
    {
    case ENUM_CLOSED_LOOP_MODES::SPEED:
    {
        // update motor mode
        this->send(MMODE MODE_SPEED RBTQ_ENDSTR);
        // update deceleration
        this->send(MDEC SPEED_DEC RBTQ_ENDSTR);
        // update PID
        this->send(KP SPEED_KP RBTQ_ENDSTR);
        this->send(KI SPEED_KI RBTQ_ENDSTR);
        this->send(KD SPEED_KD RBTQ_ENDSTR);
        break;
    }
    case ENUM_CLOSED_LOOP_MODES::POSITION:
    {
        // update motor mode
        this->send(MMODE MODE_POSITION RBTQ_ENDSTR);
        // update PID
        this->send(KP POSITION_KP RBTQ_ENDSTR);
        this->send(KI POSITION_KI RBTQ_ENDSTR);
        this->send(KD POSITION_KD RBTQ_ENDSTR);
        break;
    }
    default:
        break;
    }
};

void MoveMotor::updateMoveSpeed(int diff)
{
    char speed[DEFAULT_CHARARR_BLOCK_SIZE];

    if (diff > 16)
        strcpy(speed, "1000");
    else if (diff >= 13)
        strcpy(speed, "750");
    else if (diff >= 7)
        strcpy(speed, "500");
    else
        strcpy(speed, "250");
    // else if (diff > 2)
    //     strcpy(speed, "240");
    // else
    //     strcpy(speed, "120");

    char mmCmd[DEFAULT_CHARARR_BLOCK_SIZE];
    strcpy(mmCmd, SPEED_MOVE);
    strcat(mmCmd, speed);
    strcat(mmCmd, RBTQ_ENDSTR);

    this->send(mmCmd);
    Serial.println(mmCmd);
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

int MoveMotor::interpretRpmFeedback(const char *feedback)
{
    // extract and interpret RPM feedback
    int valDelimiter = IDXOF(feedback, QUERY_DELIMITER);
    if (valDelimiter == -1) // sometimes roboteq sends garbage like '+' char
        return INT16_MIN;
    char rpm[DEFAULT_CHARARR_BLOCK_SIZE];
    SUBSTR(rpm, feedback, valDelimiter + 1);
    int rpmInt = atoi(rpm);

    return abs(rpmInt);
};

bool MoveMotor::onLastSlotholeArrival()
{
    // engage brakes
    this->engageBrake();

    // immediate deceleration
    this->send(MDEC SPEED_IM_DEC RBTQ_ENDSTR);

    // update leading sensor count
    if (this->leadingSensor->getCount() != this->trailingSensor->getCount())
        this->leadingSensor->setCounter(this->trailingSensor->getCount());

    // update current slothole
    this->currentSlothole = this->trailingSensor->getCount();

    // allow shuttle to settle
    delay(50);

    // check if at least one of the sensors are in hole
    if (this->leadingSensor->dRead() == IN_HOLE || this->trailingSensor->dRead() == IN_HOLE)
        return true;
    return false;
};
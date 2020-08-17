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
    // store last value read
    int lastVal = this->getLastReadVal();

    // read value
    int val = this->dRead();

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
    // // read rear sensor
    // bool leadingBool = this->leadingSensor->run(this->currentMovementDirection);
    // // int *trailingCount = this->trailingSensor->run(this->currentMovementDirection);
    // bool trailingBool = leadingBool;

    // if (leadingBool)
    // {
    //     if (this->leadingSensor->getCount() == this->targetSlothole
    //         // && this->trailingSensor->getCount() == this->targetSlothole
    //     )
    //     {
    //         Serial.println("stopping motor");
    //         // motor brake
    //         this->stop();

    //         char res[DEFAULT_CHARARR_BLOCK_SIZE];
    //         itoa(this->leadingSensor->getCount(), res, 10);
    //         return res;
    //     }
    //     else
    //     {
    //         // target not reached
    //         // update movement speed
    //         int diff = abs(this->targetSlothole - this->leadingSensor->getCount());
    //         this->updateMoveSpeed(diff);
    //     }
    // }

    // if (!this->hasRead)
    // {
    //     this->hasRead = true;
    //     this->send("?BS\r");
    // }

    // if (this->read('\r'))
    // {
    //     // read current rpm
    //     int valDelimiter = IDXOF(this->serialIn, '=');
    //     if (valDelimiter == -1)
    //         return NULL;
    //     char value[DEFAULT_CHARARR_BLOCK_SIZE];
    //     SUBSTR(value, this->serialIn, valDelimiter + 1);
    //     int rpmInt = atoi(value);

    //     if (this->currentMotorMode == SPEED)
    //     {
    //         if (!this->isStopping && millis() - this->testMillis >= 2000)
    //         {
    //             Serial.println("STOPPPPINGGGGG");
    //             // this->stop();
    //             this->send(SPEED_MOVE "0" RBTQ_ENDCHAR);
    //             this->isStopping = true;
    //         }
    //         else if (this->isStopping)
    //         {
    //             if (rpmInt < 2)
    //                 // change mode
    //                 this->setMotorMode(POSITION);
    //         }
    //         else
    //         {
    //             this->updateMoveSpeed(1);
    //         }
    //     }
    //     else if (this->currentMotorMode == POSITION)
    //     {
    //         int maxReps = 10;

    //         if (rpmInt != 0)
    //             Serial.println(rpmInt);

    //         // creep shuttle
    //         if (!this->adjustmentStarted && rpmInt < 2 && this->repetitions < maxReps)
    //         {
    //             this->send(PR_MOVE "5" RBTQ_ENDCHAR);
    //             this->adjustmentStarted = true;
    //             ++this->repetitions;

    //             Serial.print("current reps ");
    //             Serial.println(this->repetitions);

    //             if (this->repetitions >= maxReps)
    //             {
    //                 Serial.println(this->targetSlothole);
    //                 char res[DEFAULT_CHARARR_BLOCK_SIZE];
    //                 itoa(this->targetSlothole, res, 10);
    //                 Serial.println(res);
    //                 return res;
    //             }
    //         }
    //         else if (rpmInt > 4)
    //         {
    //             this->adjustmentStarted = false;
    //         }
    //     }

    //     this->clearSerialIn();
    //     this->hasRead = false;
    // }

    // determine movement mode
    switch (this->currentMotorMode)
    {
    case SPEED:
    {
        // trailing laser should always read before the leading laser
        if (this->readingSensor->run(this->currentMovementDirection))
        {
            // reading sensor has scanned from in-hole to out-hole
            if (this->readingSensor == this->trailingSensor)
            {
                // trailing sensor scanned in-hole to out-hole
                // update movement speed
                this->updateMoveSpeed(abs(this->trailingSensor->getCount() - this->targetSlothole));

                // determine if slothole scanned is the last one
                // if it is, set timeout to stop movement
                if (this->trailingSensor->getCount() == this->targetSlothole)
                {
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

        // check for last slothole timeout
        if (this->lastSlotholeMillis != DEFAULT_LAST_SLOTHOLE_MILLIS)
        {
            if (millis() - this->lastSlotholeMillis >= this->lastSlotholeStopTimeoutDuration)
            {
                // decelerate shuttle
                this->stop();

                // clear timeout
                this->lastSlotholeMillis = DEFAULT_LAST_SLOTHOLE_MILLIS;

                // switch motor mode
                this->setMotorMode(POSITION);
                this->shouldCreepPosition = true;
                this->creepCount = 0;

                // start querying for current rpm
                this->send(QUERY_RPM RBTQ_ENDSTR);
            }
        }

        break;
    }
    case POSITION:
    {
        // use relative position mode to creep toward target slothole
        // Stop creeping once leading sensor has read out-hole to in-hole
        if (!this->leadingSensor->run(IN_HOLE))
        {
            // no in hole event detected yet
            // read motor rpm and creep shuttle
            if (this->read(RBTQ_ENDCHAR))
            {
                // extract and interpret RPM feedback
                int valDelimiter = IDXOF(this->serialIn, QUERY_DELIMITER);
                if (valDelimiter == -1) // sometimes roboteq sends garbage like '+' char
                    break;
                char rpm[DEFAULT_CHARARR_BLOCK_SIZE];
                SUBSTR(rpm, this->serialIn, valDelimiter + 1);
                int rpmInt = atoi(rpm);

                if (this->shouldCreepPosition && rpmInt < POSITION_CREEP_MIN_RPM && this->creepCount < DEFAULT_MAX_CREEPS)
                {
                    // creep shuttle
                    this->send(PR_MOVE CREEP_VALUE RBTQ_ENDSTR);
                    this->shouldCreepPosition = false;
                    ++this->creepCount;
                }
                else if (!this->shouldCreepPosition && rpmInt >= POSITION_CREEP_MIN_RPM)
                {
                    // shuttle is creeping, check that shuttle has moved past creep RPM threshold
                    // before allowing it to creep again
                    this->shouldCreepPosition = true;
                }
                else
                {
                    // max creeps recahed
                    return NAKSTR "Max creeps reached";
                }
            }
        }
        else
        {
            // detected in hole event
            // stop shuttle movements
            this->brake.engage();

            // check leading sensor position count if it matches up with trailing
            if (this->leadingSensor->getCount() != this->trailingSensor->getCount())
                this->leadingSensor->setCounter(this->trailingSensor->getCount());

            // return control to shuttle component
            char res[DEFAULT_CHARARR_BLOCK_SIZE];
            itoa(this->trailingSensor->getCount(), res, 10);
            return res;
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
    this->brake.disengage();

    // set trailing sensor to start reading first
    this->readingSensor = this->trailingSensor;

    // clear out last slothole timeout
    this->lastSlotholeMillis = DEFAULT_LAST_SLOTHOLE_MILLIS;

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
    char stopMovement[DEFAULT_CHARARR_BLOCK_SIZE];
    strcpy(stopMovement, SPEED_MOVE);
    strcat(stopMovement, "0");
    strcat(stopMovement, RBTQ_ENDSTR);
    this->send(stopMovement);
};

void MoveMotor::updateCurrentSlothole(const char *slothole)
{
    int slotholeInt = atoi(slothole);
    this->currentSlothole = slotholeInt;

    // update sensor counts
    this->frontSensor.setCounter(slotholeInt);
    this->rearSensor.setCounter(slotholeInt);
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

    if (diff > 12)
        strcpy(speed, "1000");
    else if (diff > 6)
        strcpy(speed, "750");
    else if (diff > 2)
        strcpy(speed, "500");
    else
        strcpy(speed, "250");
    // else if (diff > 1)
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
    if (this->targetSlothole == SLOTHOLE_BATTERY 
        || (this->currentSlothole == SLOTHOLE_BATTERY && this->targetSlothole == SLOTHOLE_LIFTER))
        this->lastSlotholeStopTimeoutDuration = BATTERY_LIFTER;

    // from rack buffer to lifter, from lifter to rack buffer
    else if ((this->currentSlothole > SLOTHOLE_LIFTER && this->targetSlothole == SLOTHOLE_LIFTER) 
        || (this->currentSlothole == SLOTHOLE_LIFTER && this->targetSlothole == SLOTHOLE_BUFFER))
        this->lastSlotholeStopTimeoutDuration = LIFTER_BUFFER;

    // from rack buffer to first slothole, from rack to rack buffer
    else if ((this->currentSlothole > SLOTHOLE_BUFFER && this->targetSlothole == SLOTHOLE_BUFFER) 
    || this->currentSlothole == SLOTHOLE_BUFFER && this->targetSlothole == SLOTHOLE_BUFFER + 1)
        this->lastSlotholeStopTimeoutDuration = BUFFER_RACK;

    // between columns, first slothole
    else if (this->currentSlothole < this->targetSlothole && this->targetSlothole % SLOTHOLES_PER_COL == 1)
        this->lastSlotholeStopTimeoutDuration = NEXT_COL_FIRST_SLOT;

    // between columns, last slothole
    else if(this->currentSlothole > this->targetSlothole && this->targetSlothole % SLOTHOLES_PER_COL == 0)
        this->lastSlotholeStopTimeoutDuration = NEXT_COL_FIRST_SLOT;

    // within column
    else
        this->lastSlotholeStopTimeoutDuration = WITHIN_COL;
}
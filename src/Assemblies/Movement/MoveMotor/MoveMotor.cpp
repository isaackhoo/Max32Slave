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
    if (lastVal == IN_HOLE && val == OUT_HOLE)
    {
        // detected change
        // determine if should increment or decrement counter
        if (direction == FORWARD)
            this->incrementCounter();
        else if (direction == REVERSE)
            this->decrementCounter();

        Serial.print("count changed ");
        Serial.println(this->getCount());
        // if change has been detected, return current value
        return true;
    };

    return false;
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

MoveMotor::MoveMotor(HardwareSerial *ss, int dP1, int dP2, int currentSlothole) : SerialComms(ss)
{
    this->frontSensor = MoveSensor(dP1);
    this->rearSensor = MoveSensor(dP2);

    this->leadingSensor = NULL;
    this->trailingSensor = NULL;

    this->currentSlothole = currentSlothole;

    this->hasRead = false;
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

    if (!this->hasRead)
    {
        this->hasRead = true;
        this->send("?BS\r");
    }

    if (this->read('\r'))
    {
        // read current rpm
        int valDelimiter = IDXOF(this->serialIn, '=');
        if (valDelimiter == -1)
            return NULL;
        char value[DEFAULT_CHARARR_BLOCK_SIZE];
        SUBSTR(value, this->serialIn, valDelimiter + 1);
        int rpmInt = atoi(value);

        if (this->currentMotorMode == SPEED)
        {
            if (!this->isStopping && millis() - this->testMillis >= 2000)
            {
                Serial.println("STOPPPPINGGGGG");
                // this->stop();
                this->send(SPEED_MOVE "0" RBTQ_ENDCHAR);
                this->isStopping = true;
            }
            else if (this->isStopping)
            {
                if (rpmInt < 2)
                    // change mode
                    this->setMotorMode(POSITION);
            }
            else
            {
                this->updateMoveSpeed(1);
            }
        }
        else if (this->currentMotorMode == POSITION)
        {
            int maxReps = 10;

            if (rpmInt != 0)
                Serial.println(rpmInt);

            // creep shuttle
            if (!this->adjustmentStarted && rpmInt < 2 && this->repetitions < maxReps)
            {
                this->send(PR_MOVE "5" RBTQ_ENDCHAR);
                this->adjustmentStarted = true;
                ++this->repetitions;

                Serial.print("current reps ");
                Serial.println(this->repetitions);

                if (this->repetitions >= maxReps)
                {
                    Serial.println(this->targetSlothole);
                    char res[DEFAULT_CHARARR_BLOCK_SIZE];
                    itoa(this->targetSlothole, res, 10);
                    Serial.println(res);
                    return res;
                }
            }
            else if (rpmInt > 4)
            {
                this->adjustmentStarted = false;
            }
        }

        this->clearSerialIn();
        this->hasRead = false;
    }

    return NULL; // move step incomplete
};

void MoveMotor::moveTo(const char *slothole)
{
    this->setMotorMode(SPEED);

    this->testMillis = millis();
    this->repetitions = 0;
    this->adjustmentStarted = false;
    this->isStopping = false;

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
    // switch (direction)
    // {
    // case FORWARD:
    // {
    //     this->leadingSensor = &this->frontSensor;
    //     this->trailingSensor = &this->rearSensor;
    //     break;
    // }
    // case REVERSE:
    // {
    //     this->leadingSensor = &this->rearSensor;
    //     this->trailingSensor = &this->frontSensor;
    //     break;
    // }
    // default:
    //     break;
    // }

    this->leadingSensor = &this->rearSensor;

    // determine slothole spread
    int difference = abs(this->currentSlothole - this->targetSlothole);
    difference = 2;
    this->updateMoveSpeed(difference);
};

void MoveMotor::stop()
{
    // immediate deceleration
    this->send(MDEC SPEED_IM_DEC RBTQ_ENDCHAR);

    // stop speed
    char stopMovement[DEFAULT_CHARARR_BLOCK_SIZE];
    strcpy(stopMovement, SPEED_MOVE);
    strcat(stopMovement, "0");
    strcat(stopMovement, RBTQ_ENDCHAR);
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
        this->send(MMODE MODE_SPEED RBTQ_ENDCHAR);
        // update deceleration
        this->send(MDEC SPEED_DEC RBTQ_ENDCHAR);
        // update PID
        this->send(KP SPEED_KP RBTQ_ENDCHAR);
        this->send(KI SPEED_KI RBTQ_ENDCHAR);
        this->send(KD SPEED_KD RBTQ_ENDCHAR);
        break;
    }
    case ENUM_CLOSED_LOOP_MODES::POSITION:
    {
        // update motor mode
        this->send(MMODE MODE_POSITION RBTQ_ENDCHAR);
        // update PID
        this->send(KP POSITION_KP RBTQ_ENDCHAR);
        this->send(KI POSITION_KI RBTQ_ENDCHAR);
        this->send(KD POSITION_KD RBTQ_ENDCHAR);
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
    else if (diff > 9)
        strcpy(speed, "750");
    else if (diff > 4)
        strcpy(speed, "500");
    else if (diff > 1)
        strcpy(speed, "240");
    else
        strcpy(speed, "120");

    char mmCmd[DEFAULT_CHARARR_BLOCK_SIZE];
    strcpy(mmCmd, SPEED_MOVE);
    strcat(mmCmd, speed);
    strcat(mmCmd, RBTQ_ENDCHAR);

    this->send(mmCmd);
    Serial.println(mmCmd);
};
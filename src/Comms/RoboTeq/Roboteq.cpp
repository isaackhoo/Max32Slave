#include "Comms/RoboTeq/Roboteq.h"

// --------------------------------
// ROBOTEQ PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// ROBOTEQ PUBLIC METHODS
// --------------------------------
Roboteq::Roboteq(){};

Roboteq::Roboteq(HardwareSerial *ss) : SerialComms(ss)
{
    this->currentMode = SPEED;

    this->rawFeedback = 0;
};

void Roboteq::requestRpm()
{
    this->send(QUERY_RPM RBTQ_ENDSTR);
};

void Roboteq::requestPositionCount()
{
    this->send(QUERY_POSITION_COUNT RBTQ_ENDSTR);
};

bool Roboteq::available()
{
    if (this->read(RBTQ_ENDCHAR))
        return true;
    return false;
};

int Roboteq::getRoboteqFeedback()
{
    int res = this->interpretFeedback();
    this->clearSerialIn();
    return res;
};

int Roboteq::getRoboteqRawFeedback()
{
    return this->rawFeedback;
};

bool Roboteq::setMode(ENUM_ROBOTEQ_CONFIG mode)
{
    // set mode
    this->currentMode = mode;
    switch (this->currentMode)
    {
    case ENUM_ROBOTEQ_CONFIG::SPEED:
    {
        // update motor mode
        this->send(MMODE MODE_SPEED RBTQ_ENDSTR);
        // update deceleration
        this->setDeceleration(MID_DEC);
        // update PID
        this->send(KP SPEED_KP RBTQ_ENDSTR);
        this->send(KI SPEED_KI RBTQ_ENDSTR);
        this->send(KD SPEED_KD RBTQ_ENDSTR);
        break;
    }
    case ENUM_ROBOTEQ_CONFIG::R_POSITION:
    {
        // update motor mode
        this->send(MMODE MODE_POSITION RBTQ_ENDSTR);
        // update PID
        this->send(KP RPOSITION_KP RBTQ_ENDSTR);
        this->send(KI RPOSITION_KI RBTQ_ENDSTR);
        this->send(KD RPOSITION_KD RBTQ_ENDSTR);
        break;
    }
    case ENUM_ROBOTEQ_CONFIG::ARM_POSITION:
    {
        // update motor mode
        this->send(MMODE MODE_POSITION RBTQ_ENDSTR);
        // update PID
        this->send(KP ARM_KP RBTQ_ENDSTR);
        this->send(KI ARM_KI RBTQ_ENDSTR);
        this->send(KD ARM_KD RBTQ_ENDSTR);
    }
    default:
        break;
    }
};

bool Roboteq::setDeceleration(const char *dec)
{
    char decCmd[DEFAULT_CHARARR_BLOCK_SIZE];
    strcpy(decCmd, MDEC);
    strcat(decCmd, dec);
    strcat(decCmd, RBTQ_ENDSTR);

    this->send(decCmd);
    return true;
};

bool Roboteq::setDeceleration(int dec)
{
    static char decStr[DEFAULT_CHARARR_BLOCK_SIZE];
    itoa(dec, decStr, 10);
    return this->setDeceleration(decStr);
};

bool Roboteq::setAccleration(const char *acc)
{
    char accCmd[DEFAULT_CHARARR_BLOCK_SIZE];
    strcpy(accCmd, MACC);
    strcat(accCmd, acc);
    strcat(accCmd, RBTQ_ENDSTR);

    this->send(accCmd);
    return true;
};

bool Roboteq::setAccleration(int acc)
{
    static char accStr[DEFAULT_CHARARR_BLOCK_SIZE];
    itoa(acc, accStr, 10);
    return this->setDeceleration(accStr);
};

ENUM_ROBOTEQ_CONFIG Roboteq::getMode()
{
    return this->currentMode;
};

// speed mode controls
bool Roboteq::setSpeedPercent(int perc)
{
    if (this->currentMode != SPEED)
        return false;

    static char spdStr[DEFAULT_CHARARR_BLOCK_SIZE];
    itoa(perc, spdStr, 10);

    char speedCmd[DEFAULT_CHARARR_BLOCK_SIZE];
    strcpy(speedCmd, SPEED_MOVE);
    strcat(speedCmd, spdStr);
    strcat(speedCmd, RBTQ_ENDSTR);

    this->send(speedCmd);
    return true;
};

// relative position controls
bool Roboteq::setRelativePosition(int relativeCount)
{
    if (this->currentMode != R_POSITION)
        return false;

    static char rposStr[DEFAULT_CHARARR_BLOCK_SIZE];
    itoa(relativeCount, rposStr, 10);

    char rposCmd[DEFAULT_CHARARR_BLOCK_SIZE];
    strcpy(rposCmd, PR_MOVE);
    strcat(rposCmd, rposStr);
    strcat(rposCmd, RBTQ_ENDSTR);

    this->send(rposCmd);
    return true;
};

bool Roboteq::setPositionCount(int count)
{
    if (this->currentMode != ARM_POSITION)
        return false;

    static char cposStr[DEFAULT_CHARARR_BLOCK_SIZE];
    itoa(count, cposStr, 10);

    char cposCmd[DEFAULT_CHARARR_BLOCK_SIZE];
    strcpy(cposCmd, P_MOVE);
    strcat(cposCmd, cposStr);
    strcat(cposCmd, RBTQ_ENDSTR);

    this->send(cposCmd);
    return true;
};

// --------------------------------
// ROBOTEQ PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// ROBOTEQ PRIVATE METHODS
// --------------------------------
int Roboteq::interpretFeedback()
{
    // extract and interpret RPM feedback
    int valDelimiter = IDXOF(this->serialIn, QUERY_DELIMITER);
    if (valDelimiter == -1) // sometimes roboteq sends garbage like '+' char or strings with random control chars
        return INT16_MIN;
    char val[DEFAULT_CHARARR_BLOCK_SIZE];
    SUBSTR(val, this->serialIn, valDelimiter + 1);
    int res = atoi(val);

    this->rawFeedback = res;

    return abs(res);
};
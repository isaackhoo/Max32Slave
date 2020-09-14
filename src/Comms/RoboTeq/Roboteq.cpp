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
    this->lastQueryMillis = 0;
};

void Roboteq::requestRpm()
{
    this->send(QUERY_RPM RBTQ_ENDSTR);
    this->lastQueryMillis = millis();
};

void Roboteq::requestPositionCount()
{
    this->send(QUERY_POSITION_COUNT RBTQ_ENDSTR);
    this->lastQueryMillis = millis();
};

bool Roboteq::available()
{
    if (this->read(RBTQ_ENDCHAR))
        return true;
    return false;
};

unsigned int Roboteq::getLastQueryMillis()
{
    return this->lastQueryMillis;
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
    decStr[0] = '\0';

    sprintf(decStr, "%d", dec);
    // itoa(dec, decStr, 10);
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
    accStr[0] = '\0';

    sprintf(accStr, "%d", acc);
    // itoa(acc, accStr, 10);
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
    spdStr[0] = '\0';

    sprintf(spdStr, "%d", perc);
    // itoa(perc, spdStr, 10);

    static char speedCmd[DEFAULT_CHARARR_BLOCK_SIZE];
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
    rposStr[0] = '\0';

    sprintf(rposStr, "%d", relativeCount);
    // itoa(relativeCount, rposStr, 10);

    static char rposCmd[DEFAULT_CHARARR_BLOCK_SIZE];
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
    cposStr[0] = '\0';

    sprintf(cposStr, "%d", count);
    // itoa(count, cposStr, 10);

    static char cposCmd[DEFAULT_CHARARR_BLOCK_SIZE];
    strcpy(cposCmd, P_MOVE);
    strcat(cposCmd, cposStr);
    strcat(cposCmd, RBTQ_ENDSTR);

    this->send(cposCmd);
    return true;
};

void Roboteq::requestBatteryLevel()
{
    this->send(QUERY_BATTERY RBTQ_ENDSTR);
    this->lastQueryMillis = millis();
};

// --------------------------------
// ROBOTEQ PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// ROBOTEQ PRIVATE METHODS
// --------------------------------
int Roboteq::interpretFeedback()
{
    // extract and interpret feedback
    int res = INT16_MIN;
    static char resStr[DEFAULT_CHARARR_BLOCK_SIZE] = {'\0'};

    // check if feedback if for battery
    int firstBattDeli = IDXOF(this->serialIn, QUERY_BATT_DELIMITER);
    if (firstBattDeli != -1)
    {
        // feedback is for battery
        int secondBattDeli = IDXOF(this->serialIn, QUERY_BATT_DELIMITER, firstBattDeli + 1);

        if (firstBattDeli != -1 && secondBattDeli != -1)
        {
            // valid battery reading
            // retrieve battery reading
            SUBSTR(resStr, this->serialIn, firstBattDeli + 1, secondBattDeli);
        }
    }
    else
    {
        // feedback is for rpm or motor count
        int countDelimiter = IDXOF(this->serialIn, QUERY_DELIMITER);
        if (countDelimiter != -1)
        {
            // prevent reading rubbish like symbols and '+' signs for no reason
            // retrieve rpm / motor count reading
            SUBSTR(resStr, this->serialIn, countDelimiter + 1);
        }
    }

    if (strlen(resStr) > 0)
    {
        // a valid result was retrieved
        this->rawFeedback = atoi(resStr);
        res = abs(this->rawFeedback);
    }

    return res;
};
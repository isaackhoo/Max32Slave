#include "Master/Master.h"
// ------------------------------------------------ MASTERCOMMS ------------------------------------------------
// ------------------------------
// MASTERCOMMS PUBLIC VARIABLES
// ------------------------------

// ------------------------------
// MASTERCOMMS PUBLIC METHODS
// ------------------------------
MasterComms::MasterComms(){};

void MasterComms::init(const char *uuid, ENUM_MASTER_ACTIONS action, const char *inst)
{
    strcpy(this->uuid, uuid);
    this->action = action;
    strcpy(this->instructions, inst);

    // calculate msg length
    this->messageLength = 0;
    this->messageLength += DEFAULT_ENUM_VALUE_LEN;
    int instLength = strlen(this->instructions);
    if (instLength > 0)
    {
        this->messageLength += strlen(BODY_DELIMITER_STR);
        this->messageLength += instLength;
    }
};

void MasterComms::init(ENUM_MASTER_ACTIONS a, const char *i)
{
    this->init(GEN_UUID(), a, i);
};

char *MasterComms::toString()
{
    // send format
    // 1      - SOH
    // x      - uuid
    // x+1    - HEADER_DELIMITER
    // x+2-y  - message length EXCLUDING STX and ETX
    // y+1    - STX
    // y+2-z  - stepAction-stepInstructions
    // z+1    - ETX
    // z+2    - EOT

    strcpy(this->str, SOHSTR);
    strcat(this->str, this->getUuid());
    strcat(this->str, HEADER_DELIMITER_STR);
    strcat(this->str, GET_TWO_DIGIT_STRING(this->getMessageLength()));
    strcat(this->str, STXSTR);
    strcat(this->str, GET_TWO_DIGIT_STRING((int)this->getAction()));
    if (strlen(this->instructions) > 0)
    {
        strcat(this->str, BODY_DELIMITER_STR);
        strcat(this->str, this->getInstructions());
    }
    strcat(this->str, ETXSTR);
    strcat(this->str, EOTSTR);

    return this->str;
};

char *MasterComms::getUuid() { return this->uuid; };

ENUM_MASTER_ACTIONS MasterComms::getAction() { return this->action; };

char *MasterComms::getInstructions() { return this->instructions; };

int MasterComms::getMessageLength() { return this->messageLength; };

// ------------------------------
// MASTERCOMMS PRIVATE VARIABLES
// ------------------------------

// ------------------------------
// MASTERCOMMS PRIVATE METHODS
// ------------------------------

// ------------------------------------------------ MASTER ------------------------------------------------
// ------------------------------
// MASTER PUBLIC VARIABLES
// ------------------------------
Master master;

// ------------------------------
// MASTER PUBLIC METHODS
// ------------------------------
Master::Master(){};

bool Master::init(HardwareSerial *ss)
{
    bool res;

    // nullify
    this->shuttleInstance = NULL;

    // set up serial to master chip
    res = this->serial.init(ss);
    this->echoBroker.init(1000 * 5, 3);

    return res;
};

void Master::run()
{
    // check for master serial msg
    if (this->serial.read(EOT))
        this->extractSerialInput();

    // check for pings
    this->runPing();

    // check for echos
    int droppedEchos = this->echoBroker.run();
    if (droppedEchos > 0)
    {
        char *echoStr = this->echoBroker.getNextDroppedString();
        while (echoStr != NULL)
        {
            // resend dropped echos
            this->send(echoStr, true, false);

            // iterate
            echoStr = this->echoBroker.getNextDroppedString();
        }
    }
};

void Master::login()
{
    MasterComms loginMsg;
    loginMsg.init(SLAVE_LOGIN, "");
    this->send(loginMsg, false, false);
};

void Master::setShuttleInstance(Shuttle *context)
{
    this->shuttleInstance = context;
};

void Master::onStepCompletion(ENUM_MASTER_ACTIONS action, const char *res)
{
    MasterComms stepComplete;
    stepComplete.init(action, res);
    this->send(stepComplete);
};

void Master::forwardLog(const char *log)
{
    MasterComms flog;
    flog.init(LOG, log);
    this->send(flog, false, true);
};

void Master::forwardErrorLog(const char *err)
{
    MasterComms elog;
    elog.init(LOGERROR, err);
    this->send(elog, false, true);
};

// ------------------------------
// MASTER PRIVATE VARIABLES
// ------------------------------

// ------------------------------
// MASTER PRIVATE METHODS
// ------------------------------
bool Master::send(const char *toSend, bool shouldLog, bool awaitEcho)
{
    // send out string
    bool res = this->serial.send(toSend);

    // check if should log
    if (shouldLog)
    {
        char sendLog[DEFAULT_CHARARR_BLOCK_SIZE * 2];

        if (!res)
            strcpy(sendLog, "Failed to send to master");
        else
        {
            strcpy(sendLog, "<< ");
            strcat(sendLog, toSend);
        }
        logger.log(sendLog, false);
    };

    // check if should await echo
    if (awaitEcho)
    {
        // retrieve uuid
        int deliIdx = IDXOF(toSend, HEADER_DELIMITER);
        char uuid[DEFAULT_CHARARR_BLOCK_SIZE / 4];
        SUBSTR(uuid, toSend, 1, deliIdx);

        this->echoBroker.push(uuid, toSend);
    }

    return res;
};

bool Master::send(MasterComms c, bool sl, bool ae)
{
    return this->send(c.toString(), sl, ae);
};

bool Master::send(MasterComms c)
{
    return this->send(c, true, true);
};

void Master::pingMaster()
{
    MasterComms pingMsg;
    pingMsg.init(SLAVE_PING, "");
    this->pong = false;
    this->pongChecked = false;
    this->lastPingMillis = millis();
    this->send(pingMsg, false, false);
};

void Master::runPing()
{
    unsigned int currentMillis = millis();

    // check for unreplied ping
    if (currentMillis - this->lastPingMillis >= MASTER_PING_TIMEOUT)
    {
        if (!this->pongChecked)
        {
            this->pongChecked = true;
            if (!this->pong)
            {
                // ping was not replied
                if (this->droppedPings < MAX_PINGS_DROPPED)
                    this->droppedPings += 1;
                else
                {
                    logger.log("Max pings dropped.", false);
                    delay(100);
                    executeSoftReset(RUN_SKETCH_ON_BOOT);
                }
            }
        }
    }

    // check if its time to ping master again
    if (currentMillis - this->lastPingMillis >= MASTER_PING_INTERVAL)
        this->pingMaster();
};

void Master::startPings()
{
    this->pingMaster();
};

void Master::updatePingReceived()
{
    this->pong = true;
    this->droppedPings = 0;
};

void Master::extractSerialInput()
{
    // string is read until EOT. EOT char is not part of string
    int sohIdx = IDXOF(this->serial.serialIn, SOH);
    int etxIdx = IDXOF(this->serial.serialIn, ETX, sohIdx + 1);

    while (etxIdx >= 0 && etxIdx > sohIdx)
    {
        // extract input
        char input[DEFAULT_CHARARR_BLOCK_SIZE];
        SUBSTR(input, this->serial.serialIn, sohIdx + 1, etxIdx + 1); // include etx in string

        int nextSohIdx = IDXOF(this->serial.serialIn, SOH, etxIdx);
        if (nextSohIdx >= 0)
            SUBSTR(this->serial.serialIn, this->serial.serialIn, nextSohIdx);
        else
            this->serial.clearSerialIn();

        // interpret and run input
        this->perform(this->interpret(input));

        // check for more inputs
        if (strlen(this->serial.serialIn) > 0)
        {
            sohIdx = IDXOF(this->serial.serialIn, SOH);
            etxIdx = IDXOF(this->serial.serialIn, ETX, sohIdx + 1);
        }
        else
        {
            sohIdx = -1;
            etxIdx = -1;
        }
    }
};

MasterComms Master::interpret(const char *input)
{
    // receive format
    // x      - uuid - millis
    // x+1    - HEADER_DELIMITER
    // x+2-y  - message length EXCLUDING STX and ETX
    // y+1    - STX
    // y+2-z  - stepAction-stepInstructions
    // z+1    - ETX

    char uuid[DEFAULT_CHARARR_BLOCK_SIZE / 4];
    char msgLen[DEFAULT_CHARARR_BLOCK_SIZE / 4];
    char body[DEFAULT_CHARARR_BLOCK_SIZE / 2];

    int headerDeliIdx = IDXOF(input, HEADER_DELIMITER);
    int stxIdx = IDXOF(input, STX);
    int etxIdx = IDXOF(input, ETX);

    SUBSTR(uuid, input, 0, headerDeliIdx);
    SUBSTR(msgLen, input, headerDeliIdx + 1, stxIdx);
    SUBSTR(body, input, stxIdx + 1, etxIdx);

    MasterComms formattedInput;

    // validate message integrity
    int msgLenInt = atoi(msgLen);
    if (strlen(body) != msgLenInt)
    {
        formattedInput.init(SLAVE_ERROR, "Mismatched msg length");
    }
    else
    {
        // interpret body
        int bdyDeliIdx = IDXOF(body, BODY_DELIMITER);
        char actionStr[DEFAULT_CHARARR_BLOCK_SIZE / 4];
        char inst[DEFAULT_CHARARR_BLOCK_SIZE / 2];

        if (bdyDeliIdx > 0)
        {
            // instructions exists
            SUBSTR(actionStr, body, 0, bdyDeliIdx);
            SUBSTR(inst, body, bdyDeliIdx + 1);
        }
        else
        {
            // no instructions
            strcpy(actionStr, body);
        }

        // convert action to enum
        ENUM_MASTER_ACTIONS action = (ENUM_MASTER_ACTIONS)atoi(actionStr);

        formattedInput.init(uuid, action, inst);
    }

    return formattedInput;
};

void Master::perform(MasterComms input)
{
    // echo back message to master chip
    if (input.getAction() != SLAVE_LOGIN && input.getAction() != SLAVE_PING && input.getAction() != SLAVE_ECHO)
    {
        MasterComms echo;
        echo.init(input.getUuid(), SLAVE_ECHO, "");
        this->send(echo, false, false);

        if (input.getAction() != LOG && input.getAction() != LOGERROR)
        {
            // log input
            char inputLog[DEFAULT_CHARARR_BLOCK_SIZE];
            strcpy(inputLog, ">> ");
            strcat(inputLog, EnumMasterActionsString[input.getAction()]);
            strcat(inputLog, " ");
            strcat(inputLog, input.getInstructions());
            logger.log(inputLog, false);
        }
    };

    // determine action to perform
    switch (input.getAction())
    {
    case SLAVE_LOGIN:
    {
        // update current slothole information
        if (this->shuttleInstance != NULL)
            this->shuttleInstance->setCurrentSlothole(input.getInstructions());

        // begin pings
        this->startPings();
        break;
    }
    case SLAVE_RESET:
    {
        executeSoftReset(RUN_SKETCH_ON_BOOT);
        break;
    }
    case SLAVE_ECHO:
    {
        this->echoBroker.verify(input.getUuid());
        break;
    }
    case SLAVE_PING:
    {
        this->updatePingReceived();
        break;
    }
    case ENGAGE_ESTOP:
    case DISENGAGE_ESTOP:
    case MOVETO:
    case READ_BIN_SENSOR:
    case EXTEND_ARM:
    case HOME_ARM:
    case EXTEND_FINGER_PAIR:
    case RETRACT_FINGER_PAIR:
    case SLAVE_BATTERY:
    {
        this->shuttleInstance->onCommand(input.getAction(), input.getInstructions());
        break;
    }
    case UPDATE_SLOTHOLE:
    {
        Serial.println("updating slothole");
        if (this->shuttleInstance != NULL)
            this->shuttleInstance->setCurrentSlothole(input.getInstructions());
        break;
    }
    default:
        break;
    }
};
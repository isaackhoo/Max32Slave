#include "Master.h"

// ----------------------------
// Master Constants Definition
// ----------------------------
const char *MasterConstants::SOH = "\x01";
const char *MasterConstants::STX = "\x02";
const char *MasterConstants::ETX = "\x03";
const char *MasterConstants::EOT = "\x04";
const char *MasterConstants::NAK = "\x25";

const String MasterConstants::HEADER_DELIMITER = ",";
const String MasterConstants::BODY_DELIMITER = "-";

const unsigned int MasterConstants::pingTimeoutDuration = 1000 * 35;

// --------------------------
// Master Private Methods
// --------------------------
String Master::createSendString(MasterCommsFormat sendObj, bool includeBoundingControlChars)
{
    // send format
    // 1      - SOH
    // x      - uuid - millis
    // x+1    - HEADER_DELIMITER
    // x+2-y  - message length EXCLUDING STX and ETX
    // y+1    - STX
    // y+2-z  - stepAction-stepInstructions
    // z+1    - ETX
    // z+2    - EOT

    String sendString = "";
    if (includeBoundingControlChars)
        sendString += SOH;
    sendString += sendObj.uuid != "" ? sendObj.uuid : String(millis());
    sendString += HEADER_DELIMITER;
    sendString += GET_TWO_DIGIT_STRING(sendObj.messageLength);
    sendString += STX;
    sendString += GET_TWO_DIGIT_STRING(sendObj.action);
    if (sendObj.instructions != "")
    {
        sendString += BODY_DELIMITER;
        sendString += sendObj.instructions;
    }
    sendString += ETX;
    if (includeBoundingControlChars)
        sendString += EOT;

    return sendString;
};

String Master::createSendString(MasterCommsFormat input)
{
    return this->createSendString(input, true);
};

bool Master::send(MasterCommsFormat input, bool shouldLog, bool awaitEcho)
{
    return this->send(this->createSendString(input), shouldLog, awaitEcho);
};

bool Master::send(MasterCommsFormat input, bool shouldLog)
{
    return this->send(input, shouldLog, true);
};

bool Master::send(MasterCommsFormat input)
{
    return this->send(input, true);
};

void Master::runPing()
{
    unsigned int currentMillis = millis();

    // check if pings are alive
    if (currentMillis - this->lastPingMillis >= pingTimeoutDuration)
    {
        logger->logError("Master pings unresponsive for " + String(pingTimeoutDuration) + "ms.", false);
        executeSoftReset(RUN_SKETCH_ON_BOOT);
    }
};

MasterCommsFormat *Master::interpret(String input)
{
    // receive format
    // x      - uuid - millis
    // x+1    - HEADER_DELIMITER
    // x+2-y  - message length EXCLUDING STX and ETX
    // y+1    - STX
    // y+2-z  - stepAction-stepInstructions
    // z+1    - ETX

    int headerDeliIdx = input.indexOf(HEADER_DELIMITER);
    int stxIdx = input.indexOf(STX, headerDeliIdx + 1);
    int bdyDeliIdx = input.indexOf(BODY_DELIMITER, stxIdx + 1); // can be -1
    int etxIdx = input.indexOf(ETX, stxIdx + 1);

    String uuid = input.substring(0, headerDeliIdx);
    int messageLength = input.substring(headerDeliIdx + 1, stxIdx).toInt();
    int actionEndIdx = bdyDeliIdx > 0 ? bdyDeliIdx : etxIdx;
    String action = input.substring(stxIdx + 1, actionEndIdx);
    String instructions = "";
    if (bdyDeliIdx > 0)
        instructions = input.substring(bdyDeliIdx + 1, etxIdx);

    // validate input
    String msg = input.substring(stxIdx + 1, etxIdx);
    if (messageLength != msg.length())
    {
        logger->logError("Invalid message - " + msg);
        return NULL;
    }

    MasterCommsFormat *formattedInput = new MasterCommsFormat(uuid, action, instructions);

    return formattedInput;
};

void Master::perform(MasterCommsFormat *formattedInput)
{
    if (formattedInput == NULL)
        return;

    ENUM_SLAVE_ACTIONS action = (ENUM_SLAVE_ACTIONS)formattedInput->action.toInt();

    if (action != SLAVE_LOGIN && action != SLAVE_PING && action != SLAVE_ECHO)
    {
        MasterCommsFormat *echoReply = new MasterCommsFormat(
            formattedInput->uuid,
            String(SLAVE_ECHO),
            "");
        this->send(*echoReply, false, false);
        delete echoReply;
    }

    switch (action)
    {
    case SLAVE_LOGIN:
    {
        // do nothing. One way message to master only
        break;
    }
    case SLAVE_RESET:
    {
        executeSoftReset(RUN_SKETCH_ON_BOOT);
        break;
    }
    case SLAVE_ECHO:
    {
        this->echos->verifyEcho(formattedInput->uuid);
        break;
    }
    case LOG:
    {
        // do nothing. this cmd is one way from slave to master
        break;
    }
    case LOGERROR:
    {
        // do nothing. this cmd is one way from slave to master
        break;
    }
    case SLAVE_PING:
    {
        // reply ping
        this->lastPingMillis = millis();
        MasterCommsFormat *pingReply = new MasterCommsFormat(
            formattedInput->uuid,
            String(SLAVE_PING),
            "");
        this->send(*pingReply, false, false);
        delete pingReply;
        break;
    }
    case ENGAGE_ESTOP:
    {
        break;
    }
    case DISENGAGE_ESTOP:
    {
        break;
    }
    case MOVETO:
    {
        break;
    }
    case READ_BIN_SENSOR:
    {
        break;
    }
    case EXTEND_ARM:
    {
        break;
    }
    case HOME_ARM:
    {
        break;
    }
    case EXTEND_FINGER_PAIR:
    {
        break;
    }
    case RETRACT_FINGER_PAIR:
    {
        break;
    }
    case SLAVE_BATTERY:
    {
        break;
    }
    default:
        break;
    };

    // remove input
    delete formattedInput;
};

void Master::handleSerialInput()
{
    int sohIdx = this->serial->serialIn.indexOf(SOH);
    int eotIdx = this->serial->serialIn.indexOf(EOT);

    while (eotIdx >= 0 && eotIdx > sohIdx)
    {
        // extract input, excluding SOH and EOT
        String input = this->serial->serialIn.substring(sohIdx + 1, eotIdx);
        if (eotIdx + 1 == this->serial->serialIn.length())
            this->serial->serialIn = "";
        else
            this->serial->serialIn = this->serial->serialIn.substring(eotIdx + 1);

        // interpret and run input
        this->perform(this->interpret(input));

        // check for more inputs
        if (this->serial->serialIn.length() > 0)
        {
            sohIdx = this->serial->serialIn.indexOf(SOH);
            eotIdx = this->serial->serialIn.indexOf(EOT);
        }
        else
        {
            sohIdx = -1;
            eotIdx = -1;
        }
    };
};

// --------------------------
// Master Public Methods
// --------------------------
Master::Master(){};

void Master::init(HardwareSerial *ss)
{
    this->serial = new SerialComms(ss);
    this->echos = new Echo();

    this->echos->attachLogContext(logger);
    this->echos->attachMasterContext(this);

    this->lastPingMillis = 0;
};

void Master::run()
{
    // check for master message
    if (this->serial->read())
        this->handleSerialInput();

    // check pings
    this->runPing();

    // check echos
    this->echos->run();
};

void Master::login()
{
    MasterCommsFormat *loginMsg = new MasterCommsFormat(
        String(SLAVE_LOGIN),
        "");
    this->send(*loginMsg, false, false);
    delete loginMsg;
};

bool Master::logToMasterChip(String msg)
{
    MasterCommsFormat *logComms = new MasterCommsFormat(
        String(LOG),
        msg);
    this->send(*logComms, false);
    delete logComms;
    return true;
};

bool Master::logErrorToMasterChip(String msg)
{
    MasterCommsFormat *logErrComms = new MasterCommsFormat(
        String(LOGERROR),
        msg);
    this->send(*logErrComms, false);
    delete logErrComms;
    return true;
};

bool Master::send(String msg, bool shouldLog, bool awaitEcho)
{
    // check if message should be logged
    // if (shouldLog)
    //     logger->log("Sent: " + msg, false);

    // logger->log("Sent should log msg");

    // send msg
    bool res = this->serial->send(msg);

    // if (!res && shouldLog)
    //     logger->logError("Failed to send to serial", false);

    // check if msg should await echo
    if (awaitEcho)
    {
        int sohIdx = msg.indexOf(SOH);
        int headerDelimiterIdx = msg.indexOf(HEADER_DELIMITER);
        String echoUuid = msg.substring(sohIdx + 1, headerDelimiterIdx);
        this->echos->pushEcho(
            echoUuid,
            millis(),
            msg.length(),
            msg);
    };

    return res;
};
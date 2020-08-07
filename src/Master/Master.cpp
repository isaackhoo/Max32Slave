#include "Master/Master.h"

namespace MasterConstants
{
    // serial communications
    const String HEADER_DELIMITER(F(","));
    const String BODY_DELIMITER(F("_"));

    // pings
    const unsigned long MASTER_PING_INTERVAL = 1000 * 10; // 10s
    const unsigned long MASTER_PING_TIMEOUT = 1000 * 5;
    const unsigned long MAX_PINGS_DROPPED = 3;
}; // namespace MasterConstants

// ------------------------------------------------ MASTERCOMMS ------------------------------------------------
// ------------------------------
// MASTERCOMMS PUBLIC VARIABLES
// ------------------------------
MasterComms::MasterComms(){};

MasterComms::MasterComms(String u, ENUM_MASTER_ACTIONS a, String i)
{
    this->init(u, a, i);
};

MasterComms::MasterComms(ENUM_MASTER_ACTIONS a, String i) : MasterComms(String(millis()), a, i){};

void MasterComms::init(String uuid, ENUM_MASTER_ACTIONS action, String inst)
{
    this->uuid = uuid;
    this->action = action;
    this->instructions = inst;
    this->msgLength = 0;

    // calculate message length
    this->msgLength += DEFAULT_ENUM_VALUE_LENGTH;
    if (this->instructions.length() > 0)
    {
        this->msgLength += BODY_DELIMITER.length();
        this->msgLength += this->instructions.length();
    }
};

void MasterComms::init(ENUM_MASTER_ACTIONS a, String i)
{
    this->init(String(millis()), a, i);
};

String MasterComms::toString(bool includeBoundingControlChars)
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

    String str((char *)0);
    str.reserve(128);

    if (includeBoundingControlChars)
        str += SOH;
    str += this->getUuid();
    str += HEADER_DELIMITER;
    str += GET_TWO_DIGIT_STRING(this->getMsgLength());
    str += STX;
    str += GET_TWO_DIGIT_STRING((int)this->getAction());
    if (this->instructions.length() > 0)
    {
        str += BODY_DELIMITER;
        str += this->getInstructions();
    }
    str += ETX;
    if (includeBoundingControlChars)
        str += EOT;

    return str;
};

String MasterComms::toString()
{
    return this->toString(true);
};

String MasterComms::getUuid()
{
    return this->uuid;
};

ENUM_MASTER_ACTIONS MasterComms::getAction()
{
    return this->action;
};

String MasterComms::getInstructions()
{
    return this->instructions;
};

int MasterComms::getMsgLength()
{
    return this->msgLength;
};

// ------------------------------
// MASTERCOMMS PUBLIC METHODS
// ------------------------------

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

// ------------------------------
// MASTER PUBLIC METHODS
// ------------------------------
Master::Master(){};

bool Master::init(HardwareSerial *serial)
{
    // initialize private variables
    this->pong = false;
    this->pongChecked = false;
    this->lastPingMillis = 0;
    this->droppedPings = 0;

    bool res;

    // init serial
    res = this->ss.init(serial);

    // init echos
    this->echoBroker.init(1000 * 5, 3);

    return res;
};

void Master::run()
{
    // check for incoming messages from master
    if (this->ss.read(EOT))
        this->extractSerialInput();

    // check for pings
    this->runPing();

    // check for echos
};

void Master::login()
{
    MasterComms loginReq(SLAVE_LOGIN, "");
    this->send(loginReq, false, false);
};

void Master::forwardLog(String log)
{
    MasterComms flog(LOG, log);
    this->send(flog);
};

void Master::forwardErrorLog(String err)
{
    MasterComms felog(LOGERROR, err);
    this->send(felog);
};

// ------------------------------
// MASTER PRIVATE VARIABLES
// ------------------------------

// ------------------------------
// MASTER PRIVATE METHODS
// ------------------------------
bool Master::send(String toSend, bool shouldLog, bool awaitEcho, unsigned int retries)
{
    bool res = this->ss.send(toSend);

    // check if should log
    if (shouldLog)
    {
        String sendLog((char *)0);
        sendLog.reserve(128);

        if (!res)
            sendLog += F("Failed to send to Master");
        else
        {
            sendLog += F("<< ");
            sendLog += toSend;
        };
        Logger::log(sendLog, false);
    };

    // check if should await msg echo
    if (awaitEcho)
    {
        // todo
    };

    return res;
};

bool Master::send(String s, bool sl, bool ae)
{
    return this->send(s, sl, ae, 0);
};

bool Master::send(MasterComms c, bool sl, bool ae)
{
    return this->send(c.toString(), sl, ae);
};

bool Master::send(MasterComms c, bool sl)
{
    return this->send(c, sl, true);
};

bool Master::send(MasterComms c)
{
    return this->send(c, true);
};

void Master::pingMaster()
{
    MasterComms pingMaster(SLAVE_PING, "");
    this->pong = false;
    this->pongChecked = false;
    this->lastPingMillis = millis();
    this->send(pingMaster, false, false);
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
                    ++this->droppedPings;
                else
                {
                    Logger::log(F("Max pings dropped. Resetting chip"), false);
                    executeSoftReset(RUN_SKETCH_ON_BOOT);
                }
            }
        }
    }

    // check if its time to ping server again
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
    // input details are stored within TCP
    int sohIdx = this->ss.serialIn.indexOf(SOH);
    int eotIdx = this->ss.serialIn.indexOf(EOT);

    while (eotIdx >= 0 && eotIdx > sohIdx)
    {
        // extract input, excluding STX and ETX
        String input((char *)0);
        input.reserve(64);
        input = this->ss.serialIn.substring(sohIdx + 1, eotIdx);
        if (eotIdx + 1 == this->ss.serialIn.length())
            this->ss.serialIn = "";
        else
            this->ss.serialIn = this->ss.serialIn.substring(eotIdx + 1);

        // interpret & run input
        this->perform(this->interpret(input));

        // check for more inputs
        if (this->ss.serialIn.length() > 0)
        {
            sohIdx = this->ss.serialIn.indexOf(SOH);
            eotIdx = this->ss.serialIn.indexOf(EOT);
        }
        else
        {
            sohIdx = -1;
            eotIdx = -1;
        }
    }
};

MasterComms Master::interpret(String input)
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
    int etxIdx = input.indexOf(ETX, stxIdx + 1);

    String uuid((char *)0);
    uuid.reserve(64);
    int msgLength;
    String message((char *)0);
    message.reserve(128);

    uuid = input.substring(0, headerDeliIdx);
    msgLength = input.substring(headerDeliIdx + 1, stxIdx).toInt();
    message = input.substring(stxIdx + 1, etxIdx);

    MasterComms formattedInput;

    // validate message integrity
    if (msgLength != message.length())
    {
        formattedInput.init(SLAVE_ERROR, "Mismatch message length " + message);
    }
    else
    {
        int bdyDeliIdx = message.indexOf(BODY_DELIMITER); // can be -1
        String actionStr((char *)0);
        actionStr.reserve(3);
        actionStr += bdyDeliIdx > 0 ? message.substring(0, bdyDeliIdx) : message;
        ENUM_MASTER_ACTIONS action = (ENUM_MASTER_ACTIONS)actionStr.toInt();
        String inst((char *)0);
        inst.reserve(64);
        inst += bdyDeliIdx > 0 ? message.substring(bdyDeliIdx + 1) : "";

        formattedInput.init(uuid, action, inst);
    }

    return formattedInput;
};

void Master::perform(MasterComms input)
{
    // echo back message to slave
    if (input.getAction() != SLAVE_LOGIN && input.getAction() != SLAVE_PING && input.getAction() != SLAVE_ECHO)
    {
        MasterComms echo(input.getUuid(), SLAVE_ECHO, "");
        this->send(echo, false, false);

        if (input.getAction() != LOG && input.getAction() != LOGERROR)
        {
            // log input
            String received((char *)0);
            received.reserve(128);
            received += ">> ";
            received += input.toString();
            Logger::log(received, false);
        }
    };

    // determine slave action to perform
    switch (input.getAction())
    {
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
    // case READ_BIN_SENSOR:
    case EXTEND_ARM:
    case HOME_ARM:
    case EXTEND_FINGER_PAIR:
    case RETRACT_FINGER_PAIR:
    {
        Logger::log("received: " + String(input.getAction()) + " " + input.getInstructions(), false);
        this->send(input);
        break;
    }
    case SLAVE_BATTERY:
    {
        break;
    }
    case SLAVE_ERROR:
    {
        Logger::logError(input.getInstructions());
        break;
    }
    default:
        break;
    }
};
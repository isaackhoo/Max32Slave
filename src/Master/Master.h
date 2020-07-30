#pragma once

#ifndef MASTER_H
#define MASTER_H

#include "common.h"
#include "Comms/Serial/SerialComms.h"
#include "Echo/Echo.h"

// actions
enum ENUM_SLAVE_ACTIONS
{
    SLAVE_ECHO = 0,
    LOG,
    LOGERROR,
    SLAVE_PING,

    ENGAGE_ESTOP,
    DISENGAGE_ESTOP,

    MOVETO, // implicitly uses brake and dual sensors

    READ_BIN_SENSOR,

    EXTEND_ARM,
    HOME_ARM, // implicitly also uses arm sensors

    EXTEND_FINGER_PAIR,
    RETRACT_FINGER_PAIR,

    SLAVE_BATTERY,
};

// serial communications
extern const char *SOH;
extern const char *STX;
extern const char *ETX;
extern const char *EOT;
extern const char *NAK;

extern const String HEADER_DELIMITER;
extern const String BODY_DELIMITER;

class MasterCommsFormat
{
public:
    String uuid;
    int messageLength; // excludes STX and ETX
    String action;
    String instructions;

    MasterCommsFormat(String uuid, String act, String inst)
    {
        this->init(uuid, act, inst);
    };
    MasterCommsFormat(String act, String inst)
    {
        this->init(act, inst);
    };
    void init(String uuid, String act, String inst)
    {
        this->uuid = uuid;
        this->action = act;
        this->instructions = inst;
        this->messageLength = this->action.length() + BODY_DELIMITER.length() + this->instructions.length();
    };
    void init(String act, String inst)
    {
        this->init(String(millis()), act, inst);
    };
};

// forward declaration
class Echo;

class Master
{
private:
    SerialComms *serial;
    Echo *echos;

    unsigned int lastPingMillis;
    static const unsigned int pingTimeoutDuration;

    void pingMasterChip();
    void runPing();

    void handleSerialInput();

public:
    Master();
    void init(HardwareSerial *);
    void run();

    bool logToMasterChip(String);
    bool logErrorToMasterChip(String);

    bool send(String, bool, bool);
};
extern Master *master;

#endif
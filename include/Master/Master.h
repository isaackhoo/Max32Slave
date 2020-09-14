#pragma once

#ifndef MASTER_H
#define MASTER_H

#include <Arduino.h>
#include "Master/Constants.h"
#include "Comms/Serial/SerialComms.h"
#include "Helper/Helper.h"
#include "ControlCharacters/ControlCharacters.h"
#include "Echo/Echo.h"
#include "Logger/Logger.h"
#include "Shuttle/Shuttle.h"

#define MASTER_RESET_SIGNAL "POWERON_RESET"

using namespace MasterConstants;
using namespace Helper;

// forward declaration
class Shuttle;

class MasterComms
{
public:
    MasterComms();
    void init(const char *, ENUM_MASTER_ACTIONS, const char *);
    void init(ENUM_MASTER_ACTIONS, const char *);

public:
    char *toString();

    char *getUuid();
    ENUM_MASTER_ACTIONS getAction();
    char *getInstructions();
    int getMessageLength();

private:
    int messageLength;
    char str[DEFAULT_CHARARR_BLOCK_SIZE * 2]; // return value for toString mtd

    char uuid[DEFAULT_CHARARR_BLOCK_SIZE / 2];
    ENUM_MASTER_ACTIONS action;
    char instructions[DEFAULT_CHARARR_BLOCK_SIZE / 2];
};

class Master
{
public:
    Master();
    bool init(HardwareSerial *);
    void run();

public:
    void login();
    void setShuttleInstance(Shuttle *);
    void onStepCompletion(ENUM_MASTER_ACTIONS, const char *);

    void forwardLog(const char *);
    void forwardErrorLog(const char *);

private:
    SerialComms serial;
    EchoBroker echoBroker;
    Shuttle *shuttleInstance;

    bool pong;
    bool pongChecked;
    unsigned int lastPingMillis;
    unsigned int droppedPings;

    unsigned int lastMessageReceivedMillis;

private:
    bool send(const char *, bool, bool);
    bool send(MasterComms, bool, bool);
    bool send(MasterComms);

    void pingMaster();
    void runPing();
    void startPings();
    void updatePingReceived();

    void extractSerialInput();
    MasterComms interpret(const char *);
    void perform(MasterComms);
};

extern Master master;

#endif
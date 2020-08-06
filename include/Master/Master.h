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

using namespace MasterConstants;
using namespace Helper;
using namespace ControlCharacters;

class MasterComms
{
public:
    MasterComms();
    MasterComms(String, ENUM_MASTER_ACTIONS, String);
    MasterComms(ENUM_MASTER_ACTIONS, String);
    void init(String, ENUM_MASTER_ACTIONS, String);
    void init(ENUM_MASTER_ACTIONS, String);
    String toString(bool);
    String toString();

    String getUuid();
    ENUM_MASTER_ACTIONS getAction();
    String getInstructions();
    int getMsgLength();

private:
    String uuid;
    ENUM_MASTER_ACTIONS action;
    String instructions;
    int msgLength;
};

class Master
{
public:
    Master();
    bool init(HardwareSerial *);
    void run();
    void login();

    void forwardLog(String);
    void forwardErrorLog(String);

private:
    SerialComms ss;
    EchoBroker echoBroker;

    bool pong;
    bool pongChecked;
    unsigned int lastPingMillis;
    unsigned int droppedPings;

private:
    bool send(String, bool, bool, unsigned int);
    bool send(String, bool, bool);
    bool send(MasterComms, bool, bool);
    bool send(MasterComms, bool);
    bool send(MasterComms);

    void pingMaster();
    void runPing();
    void startPings();
    void updatePingReceived();

    void extractSerialInput();
    MasterComms interpret(String);
    void perform(MasterComms);
};

#endif
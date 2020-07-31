#pragma once

#ifndef MASTER_H
#define MASTER_H

#include "common.h"
#include "Master/Constants.h"
#include "Helpers/LocalHelper.h"
#include "Comms/Serial/SerialComms.h"
#include "Echo/Echo.h"

// forward declaration
class Echo;

using namespace MasterConstants;

class Master
{
private:
    SerialComms *serial;
    Echo *echos;

    unsigned int lastPingMillis;

    String createSendString(MasterCommsFormat, bool);
    String createSendString(MasterCommsFormat);
    bool send(MasterCommsFormat, bool, bool);
    bool send(MasterCommsFormat, bool);
    bool send(MasterCommsFormat);

    void runPing();

    MasterCommsFormat *interpret(String);
    void perform(MasterCommsFormat *);
    void handleSerialInput();

public:
    Master();
    void init(HardwareSerial *);
    void run();
    void login();

    bool logToMasterChip(String);
    bool logErrorToMasterChip(String);

    bool send(String, bool, bool);
};
extern Master *master;

#endif
#pragma once

#ifndef ECHO_H
#define ECHO_H

#include "common.h"
#include "Echo/Constants.h"
#include "Logger/Logger.h"
#include "Master/Master.h"

// forward declaration
class Logger;
class Master;

using namespace EchoConstants;

class Echo
{
private:
    EchoNode *HEAD;
    EchoNode *TAIL;
    unsigned int totalEchos;

    unsigned int echoTimeoutDuration;
    unsigned int maxDrops;

    Logger *logContext;
    Master *masterContext;

    void incrementTotalEchos();
    void decrementTotalEchos();

public:
    Echo();
    void setTimeout(unsigned int);
    void setMaxDrops(unsigned int);
    void pushEcho(String, unsigned int, int, String);
    bool removeEcho(String);
    bool verifyEcho(String);
    int getTotalEchos();
    void reset();
    void run();

    void attachLogContext(Logger *);
    void attachMasterContext(Master *);
};

#endif
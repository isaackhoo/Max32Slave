#pragma once

#ifndef ECHO_H
#define ECHO_H

#include "common.h"
#include "Logger/Logger.h"
#include "Master/Master.h"

// forward declaration
class Logger;
class Master;

const unsigned int DEFAULT_ECHO_TIMEOUT = 1000;
const unsigned int DEFAULT_MAX_DROPS = 3;
struct EchoNode
{
    String uuid;
    unsigned int timeSent;
    int droppedEchoCounter;

    int messageLength;
    String message;

    EchoNode *prev;
    EchoNode *next;
};

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
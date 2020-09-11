#ifndef ECHO_H
#define ECHO_H

#include <Arduino.h>
#include "Helper/Helper.h"

#define MAX_ECHO_NODES 8

class EchoNode
{
public:
    EchoNode();
    void init(const char *, const char *, unsigned int);
    void init(const char *, const char *);

public:
    char *getUuid();
    char *getMessage();

    unsigned int getTimeRecorded();
    unsigned int getDropCount();

    bool getIsAssigned();

    bool verify(const char *);
    void incrementDropCount();
    void updateTimeSent(unsigned int);
    void clearEcho();

private:
    char uuid[DEFAULT_CHARARR_BLOCK_SIZE / 4];
    char message[DEFAULT_CHARARR_BLOCK_SIZE];

    unsigned int timeRecorded;
    unsigned int dropCount;

    bool isAssigned;
};

class EchoBroker
{
public:
    EchoBroker();
    void init(unsigned int, unsigned int);

public:
    bool push(const char *, const char *, unsigned int);
    bool push(const char *, const char *);

    bool verify(const char *);
    int run();
    char *getNextDroppedString();

private:
    unsigned int echoTimeout;
    unsigned int maxDrops;

    EchoNode echos[MAX_ECHO_NODES];
    int echoPtr;

    char droppedEchos[MAX_ECHO_NODES][DEFAULT_CHARARR_BLOCK_SIZE];
    int curDroppedPtr;
    int freeDroppedPtr;

private:
    bool getNextUnassignedNode();
};

#endif
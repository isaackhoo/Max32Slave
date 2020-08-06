#ifndef ECHO_H
#define ECHO_H

#include <Arduino.h>

class EchoNode
{
public:
    unsigned int recorded;
    unsigned int dropped;

public:
    EchoNode *prev;
    EchoNode *next;

public:
    EchoNode(String, String, unsigned int);
    EchoNode(String, String);
    bool validate(String);
    String getUuid();
    String getMessage();

private:
    String uuid;
    String message;
};

class EchoBroker
{
public:
    EchoBroker();
    void init(unsigned int, unsigned int);
    void push(String, String, unsigned int);
    void push(String, String);
    bool verify(String);
    int run();
    int getEchoCount();
    EchoNode *getNextDroppedNode();
    void removeCurrentDroppedNode();
    void reset();

private:
    EchoNode *HEAD;
    EchoNode *TAIL;
    EchoNode *DroppedHead;
    EchoNode *DroppedTail;
    unsigned int totalEchos;

    unsigned int timeoutDuration;
    unsigned int maxDrops;

private:
    void incrementTotalEchos();
    void decrementTotalEchos();
    void addPendingNode(EchoNode *);
    void addDroppedNode(EchoNode *);
    void removeNode(EchoNode *);
};

#endif
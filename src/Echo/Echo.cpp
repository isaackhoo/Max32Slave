#include "Echo/Echo.h"

// --------------------------------------------- Echo Node ---------------------------------------------
// ---------------------------------
// ECHONODE PUBLIC VARIABLES
// ---------------------------------

// ---------------------------------
// ECHONODE PUBLIC METHODS
// ---------------------------------
EchoNode::EchoNode()
{
    this->isAssigned = false;
};

void EchoNode::init(const char *uuid, const char *msg, unsigned int dropped)
{
    strcpy(this->uuid, uuid);
    strcpy(this->message, msg);
    this->dropCount = dropped;
    this->timeRecorded = millis();
    this->isAssigned = true;
};

void EchoNode::init(const char *uuid, const char *msg)
{
    this->init(uuid, msg, 0);
};

char *EchoNode::getUuid() { return this->uuid; };

char *EchoNode::getMessage() { return this->message; };

unsigned int EchoNode::getTimeRecorded() { return this->timeRecorded; };

unsigned int EchoNode::getDropCount() { return this->dropCount; };

bool EchoNode::getIsAssigned() { return this->isAssigned; };

bool EchoNode::verify(const char *uuid)
{
    return strcmp(this->uuid, uuid) == 0 ? true : false;
};

void EchoNode::incrementDropCount()
{
    ++this->dropCount;
};

void EchoNode::updateTimeSent(unsigned int timeSent)
{
    this->timeRecorded = timeSent;
};

void EchoNode::clearEcho()
{
    this->uuid[0] = '\0';
    this->message[0] = '\0';
    this->dropCount = 0;
    this->isAssigned = false;
};

// ---------------------------------
// ECHONODE PRIVATE VARIABLES
// ---------------------------------

// ---------------------------------
// ECHONODE PRIVATE METHODS
// ---------------------------------

// --------------------------------------------- Echo Broker ---------------------------------------------
// ---------------------------------
// ECHO PUBLIC VARIABLES
// ---------------------------------

// ---------------------------------
// ECHO PUBLIC METHODS
// ---------------------------------
EchoBroker::EchoBroker(){};

void EchoBroker::init(unsigned int echoTimeout, unsigned int maxDrops)
{
    this->echoPtr = 0;
    this->curDroppedPtr = 0;
    this->freeDroppedPtr = 0;

    this->echoTimeout = echoTimeout;
    this->maxDrops = maxDrops;
};

bool EchoBroker::push(const char *uuid, const char *msg, unsigned int dropped)
{
    bool res = true;
    // search for unassigned node
    res = this->getNextUnassignedNode();

    if (res)
    {
        // convert the echo node that echo ptr is pointing to.
        this->echos[this->echoPtr].init(uuid, msg, dropped);
    }

    return res;
};

bool EchoBroker::push(const char *uuid, const char *msg)
{
    this->push(uuid, msg, 0);
};

bool EchoBroker::verify(const char *uuid)
{
    bool res = false;
    // search nodes for existing uuid
    int offset = 0;
    while (offset < MAX_ECHO_NODES)
    {
        EchoNode *node = &this->echos[(this->echoPtr + offset) % MAX_ECHO_NODES];
        if (node->getIsAssigned() && node->verify(uuid))
        {
            node->clearEcho();
            res = true;
            break;
        }
        ++offset;
    }
    return res;
};

int EchoBroker::run()
{
    // checks for dropped echos
    unsigned int currentMillis = millis();
    int droppedEchos = 0;

    int offset = 0;
    while (offset < MAX_ECHO_NODES)
    {

        EchoNode *node = &this->echos[(this->echoPtr + offset) % MAX_ECHO_NODES];
        if (node->getIsAssigned())
        {
            // check if echo has dropped
            if (currentMillis - node->getTimeRecorded() >= this->echoTimeout)
            {
                if (node->getDropCount() < this->maxDrops)
                {
                    // increment echo's drop count
                    node->incrementDropCount();
                    ++droppedEchos;

                    // update dropped echo timing
                    node->updateTimeSent(currentMillis);

                    // add string to send to dropped echos
                    strcpy(this->droppedEchos[this->freeDroppedPtr], node->getMessage());
                    // move pointer
                    ++this->freeDroppedPtr;
                }
                else
                {
                    // echo has reached max drops.
                    // dispose of echo
                    node->clearEcho();
                }
            }
        }
        ++offset;
    }

    return droppedEchos;
};

char *EchoBroker::getNextDroppedString()
{
    static char str[DEFAULT_CHARARR_BLOCK_SIZE];

    // check if end of dropped echos have been reached
    if (this->curDroppedPtr == 0 && this->freeDroppedPtr == 0 && strlen(this->droppedEchos[this->curDroppedPtr]) == 0)
        return NULL;

    // found an echo string. copy to static str to pass out
    strcpy(str, this->droppedEchos[this->curDroppedPtr]);

    // empty out dropped echo
    this->droppedEchos[this->curDroppedPtr][0] = '\0';

    // move cur dropped ptr
    if (this->curDroppedPtr < this->freeDroppedPtr)
        ++this->curDroppedPtr;

    if (this->curDroppedPtr == this->freeDroppedPtr)
    {
        this->curDroppedPtr = 0;
        this->freeDroppedPtr = 0;
    }

    return str;
};

// ---------------------------------
// ECHO PRIVATE VARIABLES
// ---------------------------------

// ---------------------------------
// ECHO PRIVATE METHODS
// ---------------------------------
bool EchoBroker::getNextUnassignedNode()
{
    int tries = 0;
    while (this->echos[this->echoPtr].getIsAssigned() && tries < MAX_ECHO_NODES)
    {
        if (this->echoPtr + 1 == MAX_ECHO_NODES)
            this->echoPtr = 0;
        else
            ++this->echoPtr;
        ++tries;
    }

    return this->echos[this->echoPtr].getIsAssigned() == false ? true : false;
};
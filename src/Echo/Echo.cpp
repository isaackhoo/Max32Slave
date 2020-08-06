#include "Echo/Echo.h"

// --------------------------------------------- Echo Node ---------------------------------------------
EchoNode::EchoNode(String uuid, String msg, unsigned int dropCount)
{
    this->uuid = uuid;
    this->message = msg;

    this->recorded = millis();
    this->dropped = dropCount;

    this->prev = NULL;
    this->next = NULL;
};

EchoNode::EchoNode(String uuid, String msg) : EchoNode::EchoNode(uuid, msg, 0){};

bool EchoNode::validate(String uuid)
{
    return this->uuid == uuid ? true : false;
};

String EchoNode::getUuid() { return this->uuid; };

String EchoNode::getMessage() { return this->message; };
// --------------------------------------------- Echo Broker ---------------------------------------------

// ---------------------------------
// Echo Public Methods
// ---------------------------------
EchoBroker::EchoBroker()
{
    this->HEAD = NULL;
    this->TAIL = NULL;

    this->DroppedHead = NULL;
    this->DroppedTail = NULL;

    this->totalEchos = 0;
};

void EchoBroker::init(unsigned int timeoutDuration, unsigned int maxDrops)
{
    this->timeoutDuration = timeoutDuration;
    this->maxDrops = maxDrops;
};

void EchoBroker::push(String uuid, String msg, unsigned int currentDropped)
{
    EchoNode *node = new EchoNode(uuid, msg, currentDropped);

    this->addPendingNode(node);

    // increment echo counts
    this->incrementTotalEchos();
};

void EchoBroker::push(String uuid, String msg)
{
    this->push(uuid, msg, 0);
};

bool EchoBroker::verify(String uuid)
{
    // iterate over list of echos to find the uuid.
    bool res = false;

    if (this->totalEchos > 0)
    {
        EchoNode *iter = this->HEAD;
        while (iter != NULL)
        {
            if (iter->validate(uuid))
            {
                // remove node
                this->removeNode(iter);
                this->decrementTotalEchos();
                delete iter;
                iter = NULL;
                // notify node found
                res = true;
                // end loop
                break;
            }
            else
                iter = iter->next;
        }
    }

    return res;
};

int EchoBroker::run()
{
    if (this->totalEchos <= 0)
        return 0;

    unsigned int currentMillis;
    EchoNode *iter;
    int droppedEchos;

    // check for expired nodes
    currentMillis = millis();
    iter = this->HEAD;
    droppedEchos = 0;

    while (iter != NULL)
    {
        EchoNode *temp = iter;
        iter = iter->next;

        if (currentMillis - temp->recorded >= this->timeoutDuration)
        {
            this->removeNode(temp);
            this->decrementTotalEchos();
            if (temp->dropped < this->maxDrops)
            {
                ++temp->dropped;
                this->addDroppedNode(temp);
                ++droppedEchos;
            }
            else
            {
                delete temp;
            }
        }
    }

    return droppedEchos;
};

int EchoBroker::getEchoCount()
{
    return this->totalEchos;
};

EchoNode *EchoBroker::getNextDroppedNode()
{
    return this->DroppedHead;
};

void EchoBroker::removeCurrentDroppedNode()
{
    if (this->DroppedHead != NULL)
    {
        EchoNode *temp = this->DroppedHead;
        this->DroppedHead = this->DroppedHead->next;
        delete temp;
        temp = NULL;
    }
};

void EchoBroker::reset()
{
    while (this->HEAD != NULL)
        this->removeNode(this->HEAD);
    while (this->DroppedHead != NULL)
        this->removeCurrentDroppedNode();
};

// ---------------------------------
// Echo Private Methods
// ---------------------------------
void EchoBroker::incrementTotalEchos()
{
    ++this->totalEchos;
};

void EchoBroker::decrementTotalEchos()
{
    --this->totalEchos;
};

void EchoBroker::addPendingNode(EchoNode *node)
{
    if (this->HEAD == NULL && this->TAIL == NULL)
    {
        this->HEAD = node;
        this->TAIL = node;
    }
    else
    {
        this->TAIL->next = node;
        node->prev = this->TAIL;
        this->TAIL = node;
    }
};

void EchoBroker::addDroppedNode(EchoNode *node)
{
    if (this->DroppedHead == NULL && this->DroppedTail == NULL)
    {
        this->DroppedHead = node;
        this->DroppedTail = node;
    }
    else
    {
        this->DroppedTail->next = node;
        node->prev = this->DroppedTail;
        this->DroppedTail = node;
    }
};

void EchoBroker::removeNode(EchoNode *iter)
{
    // remove node from list
    // link prev node to next
    if (iter->prev != NULL)
        iter->prev->next = iter->next;
    // move head pointer
    if (iter == this->HEAD)
        this->HEAD = iter->next;
    // link next node to prev
    if (iter->next != NULL)
        iter->next->prev = iter->prev;
    // move tail pointer
    if (iter == this->TAIL)
        this->TAIL = iter->prev;
};
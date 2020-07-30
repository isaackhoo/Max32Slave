#include "Echo.h"

Echo::Echo()
{
    this->HEAD = NULL;
    this->TAIL = NULL;
    this->totalEchos = 0;
    this->echoTimeoutDuration = DEFAULT_ECHO_TIMEOUT;
    this->maxDrops = DEFAULT_MAX_DROPS;

    this->logContext = NULL;
    this->masterContext = NULL;
};

void Echo::setTimeout(unsigned int timeout)
{
    this->echoTimeoutDuration = timeout;
};

void Echo::setMaxDrops(unsigned int maxDrops)
{
    this->maxDrops = maxDrops;
};

void Echo::incrementTotalEchos()
{
    ++this->totalEchos;
}

void Echo::decrementTotalEchos()
{
    if (this->totalEchos > 0)
        --this->totalEchos;
}

void Echo::pushEcho(String uuid, unsigned int timeMillis, int messageLength, String msg)
{
    // create echo node
    EchoNode *node = new EchoNode();
    node->uuid = uuid;
    node->droppedEchoCounter = 0;
    node->timeSent = timeMillis;
    node->messageLength = messageLength;
    node->message = msg;
    node->prev = NULL;
    node->next = NULL;

    // if is first node
    if (this->HEAD == NULL && this->TAIL == NULL)
    {
        this->HEAD = node;
        this->TAIL = node;
    }
    // its not first node
    else
    {
        this->TAIL->next = node;
        node->prev = this->TAIL;
        this->TAIL = node;
    }

    // increment number of nodes
    this->incrementTotalEchos();
};

bool Echo::removeEcho(String uuid)
{
    bool isRemoved = false;

    if (this->getTotalEchos() == 0)
        return isRemoved;

    EchoNode *iterator = this->HEAD;
    while (iterator != NULL)
    {
        // look for node with the uuid to remove
        if (iterator->uuid == uuid)
        {
            // uuid matches. remove from linked list

            // link prev node to next node
            if (iterator->prev != NULL)
                iterator->prev->next = iterator->next;
            // move head pointer if iterator is first node
            if (this->HEAD == iterator)
                this->HEAD = iterator->next;
            // link next node to prev node
            if (iterator->next != NULL)
                iterator->next->prev = iterator->prev;
            // move tail pointer if iterator is last node
            if (this->TAIL == iterator)
                this->TAIL = iterator->prev;

            // remove node
            delete iterator;
            // reduce count
            this->decrementTotalEchos();
            isRemoved = true;
            iterator = NULL;
            // break out of while loop
            break;
        }
        // iterate to next node
        else
        {
            iterator = iterator->next;
        }
    };

    return isRemoved;
};

bool Echo::verifyEcho(String uuid)
{
    // iterate linked list to find node with same uuid
    // if found, remove node from linked list and returns the echo found
    return this->removeEcho(uuid);
}

int Echo::getTotalEchos()
{
    return this->totalEchos;
}

void Echo::reset()
{
    // clears out all existing echo nodes
    while (this->HEAD != NULL && this->TAIL != NULL)
    {
        this->removeEcho(this->HEAD->uuid);
    }
}

void Echo::run()
{
    if (this->getTotalEchos() == 0)
        return;

    // checks for expired echos
    unsigned int currentMillis = millis();
    EchoNode *iterator = this->HEAD;
    EchoNode *expiredEchos[this->getTotalEchos()];
    int expiredIterator = 0;

    while (iterator != NULL)
    {
        if (currentMillis - iterator->timeSent >= this->echoTimeoutDuration)
        {
            expiredEchos[expiredIterator] = iterator;
            ++expiredIterator;
        }

        // iterate to next
        iterator = iterator->next;
    }

    // send a message and remove all expired echos
    for (int i = 0; i < expiredIterator; ++i)
    {
        EchoNode *expiredNode = expiredEchos[i];
        String expiredStr = "";
        expiredStr += "[Echo Timed-out] uuid: ";
        expiredStr += expiredNode->uuid;
        expiredStr += ", message: ";
        expiredStr += expiredNode->message;
        if (this->logContext != NULL)
            this->logContext->logError(expiredStr);

        // check if node has overrun number of retries
        if (expiredNode->droppedEchoCounter < this->maxDrops)
        {
            // increment drop counter
            ++expiredNode->droppedEchoCounter;
            String resendingLog = "Resending uuid " + expiredNode->uuid;
            if (this->logContext != NULL)
                this->logContext->logError(resendingLog);
            // resend the message
            if (this->masterContext != NULL)
                this->masterContext->send(expiredNode->message, true, false);
            //sender(expiredNode->message, true, false);
            // update dropped node sent millis
            expiredNode->timeSent = millis();
        }
        else
        {
            // echo has dropped max number of times
            this->removeEcho(expiredNode->uuid);
            String maxDropLog = "No echo received for " + expiredNode->uuid + ", message: " + expiredNode->message;
            if (this->logContext != NULL)
                this->logContext->logError(maxDropLog);
            //logger(maxDropLog);
        }
    }
};

void Echo::attachLogContext(Logger *lContext)
{
    this->logContext = lContext;
};

void Echo::attachMasterContext(Master *mContext)
{
    this->masterContext = mContext;
};
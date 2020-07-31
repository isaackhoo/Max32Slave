#pragma once

#ifndef ECHOCONSTANTS_H
#define ECHOCONSTANTS_H

#include "common.h"

namespace EchoConstants
{
    extern const unsigned int DEFAULT_ECHO_TIMEOUT;
    extern const unsigned int DEFAULT_MAX_DROPS;
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
} // namespace EchoConstants

#endif
#pragma once

#ifndef HBRIDGE_H
#define HBRIDGE_H

#include "Comms/Digital/DigitalComms.h"

// forward Declaration
class DigitalComms;

class HBridge
{
    private:
        DigitalComms *lane1;
        DigitalComms *lane2;

    public:
        HBridge(int, int);

    public:
        void switchHL();
        void switchLH();
        void switchHH();
        void switchLL();
};

#endif
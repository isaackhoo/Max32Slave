#pragma once

#ifndef POWER_H
#define POWER_H

#include "Comms/Digital/DigitalComms.h"

// forward declaration
class DigitalComms;

class Power
{
    private:
        DigitalComms *powerCtrl;

    public:
        Power(int);
        void on();
        void off();
};

#endif
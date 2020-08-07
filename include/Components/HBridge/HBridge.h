#pragma once

#ifndef HBRIDGE_H
#define HBRIDGE_H

#include <Arduino.h>
#include "Comms/Digital/DigitalComms.h"

class HBridge
{
public:
    HBridge();
    HBridge(int, int);

    void switchHL();
    void switchLH();
    void switchHH();
    void switchLL();

private:
    DigitalComms lane1;
    DigitalComms lane2;
};

#endif
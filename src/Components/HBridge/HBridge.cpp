#include "HBridge.h"

// --------------------------
// HBridge Public Methods
// --------------------------
HBridge::HBridge(int pin1, int pin2)
{
    this->lane1 = new DigitalComms(pin1);
    this->lane2 = new DigitalComms(pin2);
};

void HBridge::switchHL()
{
    lane1->dWriteHigh();
    lane2->dWriteLow();
}

void HBridge::switchLH()
{
    lane1->dWriteLow();
    lane2->dWriteHigh();
}

void HBridge::switchHH()
{
    lane1->dWriteHigh();
    lane2->dWriteHigh();
}

void HBridge::switchLL()
{
    lane1->dWriteLow();
    lane2->dWriteLow();
}
#include "Components/HBridge/HBridge.h"

// --------------------------------
// HBRIDGE PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// HBRIDGE PUBLIC METHODS
// --------------------------------
HBridge::HBridge(){};

HBridge::HBridge(int dPin1, int dPin2)
{
    this->lane1 = DigitalComms(dPin1);
    this->lane2 = DigitalComms(dPin2);
};

void HBridge::switchHL()
{
    this->lane1.dWriteHigh();
    this->lane2.dWriteLow();
};

void HBridge::switchLH()
{
    this->lane1.dWriteLow();
    this->lane2.dWriteHigh();
};

void HBridge::switchHH()
{
    this->lane1.dWriteHigh();
    this->lane2.dWriteHigh();
};

void HBridge::switchLL()
{
    this->lane1.dWriteLow();
    this->lane2.dWriteLow();
};

// --------------------------------
// HBRIDGE PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// HBRIDGE PRIVATE METHODS
// --------------------------------
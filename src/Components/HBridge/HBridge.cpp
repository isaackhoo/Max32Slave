#include "HBridge.h"

// --------------------------
// HBridge Public Methods
// --------------------------
HBridge::HBridge(int pin1, int pin2)
{
    this->lane1 = new DigitalComms(pin1);
    this->lane2 = new DigitalComms(pin2);
};
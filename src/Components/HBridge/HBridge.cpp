#include "Components/HBridge/HBridge.h"

// --------------------------------
// HBRIDGE PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// HBRIDGE PUBLIC METHODS
// --------------------------------
HBridge::HBridge(){};

HBridge::HBridge(int dPin1, int dPin2, int powerPassthroughPin, int pwmPin) : Power(powerPassthroughPin)
{
    this->lane1 = DigitalComms(dPin1, OUTPUT);
    this->lane2 = DigitalComms(dPin2, OUTPUT);

    this->pwm = DigitalComms(pwmPin, OUTPUT);

    this->pwm.dWriteHigh(); // max pwm
    this->on();             // power on
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

int HBridge::readPwm()
{
    return this->pwm.dRead();
};

int HBridge::readLanes() {
    int res = 0;

    int res1 = this->lane1.dRead();
    int res2 = this->lane2.dRead();

    res = res1;
    res *= 10;
    res += res2;

    return res;
}

// --------------------------------
// HBRIDGE PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// HBRIDGE PRIVATE METHODS
// --------------------------------
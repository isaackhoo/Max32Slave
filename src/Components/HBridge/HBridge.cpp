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

    Serial.print("power: ");
    Serial.print(Power::dRead());
    Serial.print(" pwm: ");
    Serial.print(this->pwm.dRead());
    Serial.print(" lane 1: ");
    Serial.print(this->lane1.dRead());
    Serial.print(" lane 2: ");
    Serial.println(this->lane2.dRead());
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
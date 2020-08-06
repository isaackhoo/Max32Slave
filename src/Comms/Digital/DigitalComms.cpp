#include "DigitalComms.h"

// -----------------------------
// DigitalComms Public Methods
// -----------------------------
DigitalComms::DigitalComms(int pin)
{
    this->pin = pin;
};

int DigitalComms::dRead() 
{
    int val;

    val = digitalRead(this->pin);

    return val;
};

int DigitalComms::dWriteHigh()
{
    digitalWrite(this->pin, HIGH);    
};

int DigitalComms::dWriteLow()
{
    digitalWrite(this->pin, LOW);
};
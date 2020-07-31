#include "Power.h"

// --------------------------
// Power Public Methods
// --------------------------
Power::Power(int pin)
{
    this->powerCtrl = new DigitalComms(pin);
};

void Power::on()
{
    
};

void Power::off()
{

};
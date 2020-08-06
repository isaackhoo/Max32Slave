#pragma once

#ifndef CURRENTSENSOR_H
#define CURRENTSENSOR_H

#include "Comms/Analog/AnalogComms.h"
#include <Adafruit_INA219.h>

// Instantiate ina219 object here?
Adafruit_INA219 frontFingersCS(0x45); // i assume your front means the part facing into the rack?
Adafruit_INA219 backFingersCS(0x41); // then this would be your rear...

class CurrentSensor
{
    private:
        int bit;

        // takes 1st bit of finger to identify which current sensor to use
        //Front fingers sensor > 30 || 31 > Adafruit_INA219 ina219_D(0x45)
        int finger0FirstBit = 30;
        int finger1FirstBit = 31;

        //Back fingers sensor > 36 || 39 > Adafruit_INA219 ina219_B(0x41)
        int finger2FirstBit = 36;
        int finger3FirstBit = 39;

    public:
        CurrentSensor(int);
        double readCurrent();
};

#endif
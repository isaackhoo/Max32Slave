#pragma once

#ifndef DIGITALSENSOR_H
#define DIGITALSENSOR_H

#include <Arduino.h>
#include "Comms/Digital/DigitalComms.h"

class DigitalSensor : public DigitalComms
{
public:
    DigitalSensor();
    DigitalSensor(int);
};

#endif
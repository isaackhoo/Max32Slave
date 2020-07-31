#pragma once

#ifndef DIGITALSENSOR_H
#define DIGITALSENSOR_H

#include "Comms/Digital/DigitalComms.h"

class DigitalSensor : public DigitalComms
{
    public:
        DigitalSensor(int);
};

#endif
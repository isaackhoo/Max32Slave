#pragma once

#ifndef ANALOGCOMMS_H
#define ANALOGCOMMS_H

class AnalogComms
{
    private:
        int pin;

    public:
        AnalogComms(int);
        double aRead();
};

#endif
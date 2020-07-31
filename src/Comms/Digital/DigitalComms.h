#pragma once

#ifndef DIGITALCOMMS_H
#define DIGITALCOMMS_H

class DigitalComms
{
    private:
        int pin;

    public:
        DigitalComms(int);
        int dRead();
        int dWrite(int);
};

#endif
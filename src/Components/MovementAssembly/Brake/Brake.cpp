#include "Brake.h"

// ---------------------------
// HBridgeBrake Public Methods
// ---------------------------
Brake::Brake(int pin1, int pin2) : HBridge(pin1, pin2){};

// for brake... DISENGAGE BRAKE > pin1 HIGH / pin2 LOW
//              ENGAGE    BRAKE > pin1 LOW  / pin2 LOW

void Brake::engage()
{
    switchLL();
};

void Brake::disengage()
{
    switchHL();
};

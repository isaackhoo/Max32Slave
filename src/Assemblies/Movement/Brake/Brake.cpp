#include "Assemblies/Movement/Brake/Brake.h"

// --------------------------------
// BRAKE PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// BRAKE PUBLIC METHODS
// --------------------------------
Brake::Brake(){};

Brake::Brake(int p1, int p2) : HBridge(p1, p2){};

void Brake::engage()
{
    this->switchLL();
};

void Brake::disengage()
{
    this->switchHL();
};

// --------------------------------
// BRAKE PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// BRAKEPAIR PRIVATE METHODS
// --------------------------------
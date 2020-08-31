#include "Assemblies/Movement/Brake/Brake.h"
#include "Logger/Logger.h"

// --------------------------------
// BRAKE PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// BRAKE PUBLIC METHODS
// --------------------------------
Brake::Brake(){};

Brake::Brake(int p1, int p2, int pwrPin, int pwmPin) : HBridge(p1, p2, pwrPin, pwmPin){};

void Brake::engage()
{
    this->switchLL();
    logger.out("Brake engaged");
};

void Brake::disengage()
{
    this->switchHL();
    logger.out("Brake disengaged");
};

// --------------------------------
// BRAKE PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// BRAKEPAIR PRIVATE METHODS
// --------------------------------
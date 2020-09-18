#include "Assemblies/Movement/Brake/Brake.h"
#include "Logger/Logger.h"

// --------------------------------
// BRAKE PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// BRAKE PUBLIC METHODS
// --------------------------------
Brake::Brake(){};

Brake::Brake(int p1, int p2, int pwrPin, int pwmPin) : HBridge(p1, p2, pwrPin, pwmPin)
{
    this->isEngaged = false;
};

void Brake::engage()
{
    this->switchLL();
    this->isEngaged = true;
    logger.out("Brake engaged");
};

void Brake::disengage()
{
    this->switchHL();
    this->isEngaged = false;
    logger.out("Brake disengaged");
};

bool Brake::getIsEngaged()
{
    // read sensors to confirm brake state
    int val = this->readLanes();
    if (val == 0)
    {
        this->isEngaged = true;
    }
    else
    {
        this->isEngaged = false;
    }

    return this->isEngaged;
};

// --------------------------------
// BRAKE PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// BRAKEPAIR PRIVATE METHODS
// --------------------------------
#include "Shuttle.h"

// -----------------------------
// Shuttle Public Methods
// -----------------------------
Shuttle::Shuttle()
{
    this->moveMotor = new MoveMotor(&Serial2, 1, 2);
};
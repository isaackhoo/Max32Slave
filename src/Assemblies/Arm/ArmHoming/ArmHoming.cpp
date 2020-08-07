#include "Assemblies/Arm/ArmHoming/ArmHoming.h"

// --------------------------------
// ARMHOMING PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// ARMHOMING PUBLIC METHODS
// --------------------------------
ArmHoming::ArmHoming(){};

ArmHoming::ArmHoming(int leftReadPin, int leftLaserPin, int rightReadPin, int rightLaserPin)
{
    this->leftReceiver = DigitalSensor(leftReadPin);
    this->leftLaser = Power(leftLaserPin);

    this->rightReceiver = DigitalSensor(rightReadPin);
    this->rightLaser = Power(rightLaserPin);
};

bool ArmHoming::isClear(){

};

// --------------------------------
// ARMHOMING PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// ARMHOMING PRIVATE METHODS
// --------------------------------
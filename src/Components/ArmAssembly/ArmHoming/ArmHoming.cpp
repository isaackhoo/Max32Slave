#include "ArmHoming.h"

// -------------------------
// ArmHoming Public Methods
// -------------------------
ArmHoming::ArmHoming(int dPin, int pwrPin) : DigitalSensor(dPin), Power(pwrPin){};
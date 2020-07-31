#include "EStop.h"

// -------------------------------
// EStop Public Methods
// -------------------------------
EStop::EStop(int aPin, int dPin1, int dPin2) : AnalogComms(aPin), HBridge(dPin1, dPin2){};
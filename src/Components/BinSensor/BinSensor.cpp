#include "BinSensor.h"

// ------------------------
// BinSensor Public Methods
// ------------------------
BinSensor::BinSensor(int dPin, int aPin) : Power(dPin), AnalogComms(aPin){};
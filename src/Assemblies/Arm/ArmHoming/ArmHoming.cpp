#include "Assemblies/Arm/ArmHoming/ArmHoming.h"
#include "Logger/Logger.h"

// --------------------------------
// ARMHOMING PUBLIC VARIABLES
// --------------------------------

// --------------------------------
// ARMHOMING PUBLIC METHODS
// --------------------------------
ArmHoming::ArmHoming(){};

ArmHoming::ArmHoming(int leftReadPin, int leftLaserPin, uint8_t leftCsPin, int rightReadPin, int rightLaserPin, uint8_t rightCsPin)
{
    this->leftReceiver = DigitalSensor(leftReadPin, INPUT);
    this->leftLaser = Power(leftLaserPin);
    this->leftCs = CurrentSensor(leftCsPin);

    this->rightReceiver = DigitalSensor(rightReadPin, INPUT);
    this->rightLaser = Power(rightLaserPin);
    this->rightCs = CurrentSensor(rightCsPin);

    this->isCheckingHome = false;
};

char *ArmHoming::run()
{
    char *res = NULL;

    if (millis() - this->timeStart <= ARM_HOMING_CHECK_TIMEOUT_DURATION)
    {
        if (this->isClear())
        {
            res = this->armCountResult;
            logger.out("Arm homing clear");
        }
    }
    else
        res = NAKSTR "Arm Homing timed out";

    return res;
};

bool ArmHoming::laserOn()
{
    // check if arm homing lasers are powered
    if (this->leftCs.readBusVoltage() <= ARMHOMING_MIN_OPERATIONAL_BUS_VOLTAGE || this->rightCs.readBusVoltage() <= ARMHOMING_MIN_OPERATIONAL_BUS_VOLTAGE)
        return false;

    this->leftLaser.on();
    this->rightLaser.on();
    return true;
};

bool ArmHoming::laserOff()
{
    this->leftLaser.off();
    this->rightLaser.off();
};

bool ArmHoming::isClear()
{
    bool alwaysClear = true;

    if (alwaysClear)
    {
        logger.out("arm homing forced clear");
        return true;
    }

    int leftRead = this->leftReceiver.dRead();
    int rightRead = this->rightReceiver.dRead();

    if (leftRead == LASER_READ && rightRead == LASER_READ)
        return true;
    return false;
};

bool ArmHoming::startCheckingHome(const char *res)
{
    strcpy(this->armCountResult, res);
    this->isCheckingHome = true;
    this->timeStart = millis();
    this->laserOn();
};

void ArmHoming::clearIsCheckingHome()
{
    this->isCheckingHome = false;
    this->laserOff();
};

bool ArmHoming::getIsCheckingHome()
{
    return this->isCheckingHome;
};

// --------------------------------
// ARMHOMING PRIVATE VARIABLES
// --------------------------------

// --------------------------------
// ARMHOMING PRIVATE METHODS
// --------------------------------
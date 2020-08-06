#include <Arduino.h>
#include "Logger/Logger.h"
#include "Master/Master.h"

Master master;

void setup()
{
  bool initRes;
  // initialize logger
  initRes = Logger::init(&Serial);

  // initialize master
  initRes = master.init(&Serial1);

  // login to master
  master.login();

  // attach references
  Logger::attachMasterInstance(&master);

  if (!initRes)
    while (true)
      ;
  else
    Logger::log(F("Slave initialized"));
}

void loop()
{
  master.run();
}
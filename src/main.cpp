#include <Arduino.h>
#include "Logger/Logger.h"
#include "Master/Master.h"
#include "Shuttle/Shuttle.h"

Master master;
Shuttle shuttle;

void setup()
{
  bool initRes;
  // initialize logger
  initRes = Logger::init(&Serial);

  // initialize master
  initRes = master.init(&Serial1);

  // login to master
  master.login();

  // initialize shuttle
  initRes = shuttle.init(&Serial2, &Serial3);

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
  shuttle.run();
}
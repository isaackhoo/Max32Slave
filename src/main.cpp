#include <Arduino.h>
#include "Logger/Logger.h"
#include "Master/Master.h"
#include "Shuttle/Shuttle.h"

Master *master;
Shuttle *shuttle;

void setup()
{
  // init logger
  logger->init(&Serial);
  logger->log("Logger initialized");

  // init Master
  master = new Master();
  master->init(&Serial1);
  logger->log("master initialized");
  // attach callbacks to logger
  logger->attachContext(master);

  // initialise shuttle components
  shuttle = new Shuttle();

  // log in to master
  master->login();



  logger->log("Slave chip initialized");
}

void loop()
{
  master->run();
}
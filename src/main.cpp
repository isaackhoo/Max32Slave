#include <Arduino.h>
#include "Logger/Logger.h"
#include "Master/Master.h"

void setup()
{
  // init logger
  logger->init(&Serial);

  // init Master
  master->init(&Serial1);
  // attach callbacks to logger
  logger->attachContext(master);
}

void loop()
{
  master->run();
}
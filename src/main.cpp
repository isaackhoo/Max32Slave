#include <Arduino.h>
#include "Helper/Helper.h"
#include "ControlCharacters/ControlCharacters.h"
#include "Logger/Logger.h"
#include "Master/Master.h"
#include "Shuttle/Shuttle.h"

void setup()
{
  bool initRes = true;

  // init logger
  initRes = logger.init(&Serial);

  // init master
  initRes = master.init(&Serial1);

  // init shuttle
  initRes = shuttle.init(&Serial2, &Serial3);

  // share references
  logger.setMasterInstance(&master);
  master.setShuttleInstance(&shuttle);
  shuttle.setMasterInstance(&master);

  // login to server
  master.login();

  if (!initRes)
    while (true)
      ;
}

void loop()
{
  master.run();
}
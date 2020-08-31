#include <Arduino.h>
#include "Helper/Helper.h"
#include "ControlCharacters/ControlCharacters.h"
#include "Logger/Logger.h"
#include "Master/Master.h"
#include "Shuttle/Shuttle.h"

#include "Master/Constants.h"
#include "Shuttle/Constants.h"

void setup()
{
  bool initRes = true;

  // init logger
  initRes = logger.init(&Serial);

  // init master
  initRes = master.init(&Serial3);

  // init shuttle
  initRes = shuttle.init(&Serial1, &Serial2);

  // share references
  logger.setMasterInstance(&master);
  master.setShuttleInstance(&shuttle);
  shuttle.setMasterInstance(&master);

  // login to server
  master.login();

  if (!initRes)
  {
    executeSoftReset(RUN_SKETCH_ON_BOOT);
  }
  else
    logger.out("Slave chip initialized");
}

void loop()
{
  master.run();
  shuttle.run();
}
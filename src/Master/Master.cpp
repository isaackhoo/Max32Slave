#include "Master.h"

const char *SOH = "\x01";
const char *STX = "\x02";
const char *ETX = "\x03";
const char *EOT = "\x04";
const char *NAK = "\x25";

const String HEADER_DELIMITER = ",";
const String BODY_DELIMITER = "-";

Master *master = new Master();
const unsigned int Master::pingTimeoutDuration = 1000 * 35;

// --------------------------
// Master Private Methods
// --------------------------
void Master::pingMasterChip(){
    // todo
};

void Master::runPing(){
    // todo
};

void Master::handleSerialInput(){

};

// --------------------------
// Master Public Methods
// --------------------------
Master::Master(){};

void Master::init(HardwareSerial *ss)
{
    this->serial = new SerialComms(ss);
    this->echos = new Echo();

    this->echos->attachLogContext(logger);
    this->echos->attachMasterContext(this);

    this->lastPingMillis = 0;
};

void Master::run()
{
    // check for master message
    if (this->serial->read())
        this->handleSerialInput();

    // check pings
    this->runPing();

    // check echos
    this->echos->run();
};

bool Master::logToMasterChip(String){
    // todo
};

bool Master::logErrorToMasterChip(String){
    // todo
};

bool Master::send(String msg, bool shouldLog, bool awaitEcho)
{

};
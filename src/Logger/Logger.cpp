#include "Logger.h"

// -----------------------
// Logger Public Variables
// -----------------------
Logger *logger = new Logger();

// --------------------------
// Logger Public Methods
// --------------------------
Logger::Logger(){};

void Logger::init(HardwareSerial *ss)
{
    this->serial = new SerialComms(ss);
    this->masterInstance = NULL;
}

void Logger::log(String log, bool forwardToMaster)
{
    this->serial->send(log + "\n");
    if (forwardToMaster && this->masterInstance != NULL)
    {
        this->masterInstance->logToMasterChip(log);
    };
};

void Logger::log(String log)
{
    this->log(log, true);
};

void Logger::logError(String err, bool forwardToMaster)
{
    this->serial->send(err + "\n");
    if (forwardToMaster && this->masterInstance != NULL)
    {
        this->masterInstance->logErrorToMasterChip(err);
    };
};

void Logger::logError(String err)
{
    this->logError(err, true);
};

void Logger::attachContext(Master *ctxt)
{
    this->masterInstance = ctxt;
};
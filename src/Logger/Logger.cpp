#include "Logger.h"

Logger *logger = new Logger();
// --------------------------
// Logger Private Methods
// --------------------------

// --------------------------
// Logger Public Methods
// --------------------------
Logger::Logger(){};

void Logger::init(HardwareSerial *ss)
{
    this->serial = new SerialComms(ss);
    
    this->context = NULL;
}

void Logger::log(String log)
{
    this->serial->send(log);
    if (this->context != NULL)
        this->context->logToMasterChip(log);
};

void Logger::logError(String err)
{
    this->serial->send(err);
    if (this->context != NULL)
        this->context->logErrorToMasterChip(err);
};


void Logger::attachContext(Master *ctxt)
{
    this->context = ctxt;
};
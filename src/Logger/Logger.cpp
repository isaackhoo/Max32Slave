#include "Logger/Logger.h"

// ------------------------------
// LOGGER PUBLIC VARIABLES
// ------------------------------
Logger logger;

// ------------------------------
// LOGGER PUBLIC METHODS
// ------------------------------
Logger::Logger()
{
    this->masterInstance = NULL;
};

bool Logger::init(HardwareSerial *ss)
{
    return this->serial.init(ss);
};

void Logger::setMasterInstance(Master *context)
{
    this->masterInstance = context;
};

void Logger::log(const char *str, bool forward)
{
    if (forward && this->masterInstance != NULL)
    {
        this->masterInstance->forwardLog(str);
    }

    this->serial.send(str);
    // check if cstring is terminated with carriage return
    if (str[strlen(str) - 1] != '\n')
        this->serial.send("\n");
};

void Logger::log(const char *str)
{
    this->log(str, true);
};

void Logger::log(int i)
{
    char iStr[64];
    itoa(i, iStr, 10);
    strcat(iStr, "\n");
    this->serial.send(iStr);
};

void Logger::logError(const char *err, bool forward)
{
    if (forward && this->masterInstance != NULL)
    {
        this->masterInstance->forwardErrorLog(err);
    }

    this->serial.send(err);
    // check if cstring is terminated with carriage return
    if (err[strlen(err) - 1] != '\n')
        this->serial.send("\n");
};

void Logger::logError(const char *err)
{
    this->logError(err, true);
};

// ------------------------------
// LOGGER PRIVATE VARIABLES
// ------------------------------

// ------------------------------
// LOGGER PRIVATE METHODS
// ------------------------------
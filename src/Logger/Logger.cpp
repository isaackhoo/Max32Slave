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

void Logger::logCpy(const char *str)
{
    strcpy(this->logStr, str);
};

void Logger::logCpy(int i)
{
    char buf[DEFAULT_CHARARR_BLOCK_SIZE];
    itoa(i, buf, 10);
    this->logCpy(buf);
};

void Logger::logCpy(double d)
{
    this->logCpy(this->dtoa(d));
};

void Logger::logCat(const char *str)
{
    strcat(this->logStr, str);
};

void Logger::logCat(int i)
{
    char buf[DEFAULT_CHARARR_BLOCK_SIZE];
    itoa(i, buf, 10);
    this->logCat(buf);
};

void Logger::logCat(double d)
{
    this->logCat(this->dtoa(d));
};

void Logger::out(bool forward)
{
    if (forward && this->masterInstance != NULL)
    {
        this->masterInstance->forwardLog(this->logStr);
    }

    this->serial.send(this->logStr);
    // check if cstring is terminated with carriage return
    if (this->logStr[strlen(this->logStr) - 1] != '\n')
        this->serial.send("\n");
};

void Logger::out()
{
    this->out(true);
};

void Logger::out(const char *str, bool forward)
{
    this->logCpy(str);
    this->out(forward);
};

void Logger::out(const char *str)
{
    this->out(str, true);
};

void Logger::errOut(bool forward)
{
    if (forward && this->masterInstance != NULL)
    {
        this->masterInstance->forwardErrorLog(this->logStr);
    }

    this->serial.send(this->logStr);
    // check if cstring is terminated with carriage return
    if (this->logStr[strlen(this->logStr) - 1] != '\n')
        this->serial.send("\n");
};

void Logger::errOut()
{
    this->errOut(true);
};

void Logger::errOut(const char *str)
{
    this->logCpy(str);
    this->errOut();
}

// void Logger::log(const char *str, bool forward)
// {
//     if (forward && this->masterInstance != NULL)
//     {
//         this->masterInstance->forwardLog(str);
//     }

//     this->serial.send(str);
//     // check if cstring is terminated with carriage return
//     if (str[strlen(str) - 1] != '\n')
//         this->serial.send("\n");
// };

// void Logger::log(const char *str)
// {
//     this->log(str, true);
// };

// void Logger::log(int i)
// {
//     char iStr[64];
//     itoa(i, iStr, 10);
//     strcat(iStr, "\n");
//     this->serial.send(iStr);
// };

// void Logger::logError(const char *err, bool forward)
// {
//     if (forward && this->masterInstance != NULL)
//     {
//         this->masterInstance->forwardErrorLog(err);
//     }

//     this->serial.send(err);
//     // check if cstring is terminated with carriage return
//     if (err[strlen(err) - 1] != '\n')
//         this->serial.send("\n");
// };

// void Logger::logError(const char *err)
// {
//     this->logError(err, true);
// };

// ------------------------------
// LOGGER PRIVATE VARIABLES
// ------------------------------

// ------------------------------
// LOGGER PRIVATE METHODS
// ------------------------------
char *Logger::dtoa(double d, size_t s)
{
    int mul = 1;
    for (int dp = 0; dp < s; ++dp)
    {
        mul *= 10;
    }

    char dStr[DEFAULT_CHARARR_BLOCK_SIZE] = {'\0'};
    char numStr[DEFAULT_CHARARR_BLOCK_SIZE];
    int i = (int)(d * mul);

    int quo = i / mul;
    int rem = abs(i % mul);

    // add quo
    itoa(quo, numStr, 10);
    strcat(dStr, numStr);

    // add decimal
    strcat(dStr, ".");

    // add rem
    itoa(rem, numStr, 10);
    strcat(dStr, numStr);

    return dStr;
};

char *Logger::dtoa(double d)
{
    size_t DEFAULT_DECIMAL_PLACE = 2;
    return this->dtoa(d, DEFAULT_DECIMAL_PLACE);
}
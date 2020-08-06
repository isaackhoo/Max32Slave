#include "Logger/Logger.h"

namespace Logger
{
    namespace
    {
        SerialComms logSerial;
        Master *masterInstance;
    }; // namespace

    bool init(HardwareSerial *serial)
    {
        logSerial.init(serial);

        masterInstance = NULL;
        return true;
    };

    void attachMasterInstance(Master *context)
    {
        masterInstance = context;
    };

    void log(String toLog, bool forwardToMaster)
    {
        if (!toLog.endsWith("\n"))
            toLog += '\n';

        logSerial.send(toLog);
        if (forwardToMaster && masterInstance != NULL)
            masterInstance->forwardLog(toLog);
    };

    void log(String l) { log(l, true); };

    void logError(String err, bool forwardToMaster)
    {
        if (!err.endsWith("\n"))
            err += '\n';

        logSerial.send(err);
        if (forwardToMaster && masterInstance != NULL)
            masterInstance->forwardErrorLog(err);
    };

    void logError(String e) { logError(e, true); };
}; // namespace Logger
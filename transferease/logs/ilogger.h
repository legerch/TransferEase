#ifndef TEASE_LOGS_ILOGGER_H
#define TEASE_LOGS_ILOGGER_H

#include "logcontext.h"

#include <string>

namespace tease{

class ILogger
{

public:
    enum Level
    {
        LOG_LVL_FATAL = 0,
        LOG_LVL_ERROR,
        LOG_LVL_WARNING,
        LOG_LVL_INFO,
        LOG_LVL_DEBUG
    };

public:
    virtual void log(Level level, const LogContext &context, const std::string &msg) = 0;
};

} // namespace tease

#endif // TEASE_LOGS_ILOGGER_H
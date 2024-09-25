#ifndef TEASE_LOGS_ABSTRACTLOGGER_H
#define TEASE_LOGS_ABSTRACTLOGGER_H

#include "transferease/transferease_global.h"

#include <string>

/*****************************/
/* Macro definitions         */
/*****************************/

/* Use to retrieve context informations */
#define _TEASE_LOG_CTX_FILE   static_cast<const char *>(TEASE_FILE)
#define _TEASE_LOG_CTX_LINE   TEASE_LINE
#define _TEASE_LOG_CTX_FCT    static_cast<const char *>(TEASE_FCTNAME)

/* Create macros for easier log usage */
#define _TEASE_LOG_GENERIC(level, msg)    tease::LogManager::registerLog(level, _TEASE_LOG_CTX_FILE, _TEASE_LOG_CTX_LINE, _TEASE_LOG_CTX_FCT, msg)

#define TEASE_LOG_FATAL(msg)  _TEASE_LOG_GENERIC(tease::ILogger::LOG_LVL_FATAL, msg)
#define TEASE_LOG_ERROR(msg)  _TEASE_LOG_GENERIC(tease::ILogger::LOG_LVL_ERROR, msg)
#define TEASE_LOG_WARN(msg)   _TEASE_LOG_GENERIC(tease::ILogger::LOG_LVL_WARNING, msg)
#define TEASE_LOG_INFO(msg)   _TEASE_LOG_GENERIC(tease::ILogger::LOG_LVL_INFO, msg)
#define TEASE_LOG_DEBUG(msg)  _TEASE_LOG_GENERIC(tease::ILogger::LOG_LVL_DEBUG, msg)

/*****************************/
/* Start namespace           */
/*****************************/
namespace tease{

/*****************************/
/*     Class definitions     */
/*        LogContext         */
/*****************************/
class TEASE_EXPORT LogContext
{
    TEASE_DISABLE_COPY(LogContext)

public:
    constexpr LogContext(const char *filename, int lineNumber, const char *fctName) :
        line(lineNumber), file(filename), function(fctName){};

public:
    int line = 0;
    const char *file = nullptr;
    const char *function = nullptr;
};

/*****************************/
/*     Class definitions     */
/*        ILogger            */
/*****************************/
class TEASE_EXPORT ILogger
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

/*****************************/
/*     Class definitions     */
/*        LogManager         */
/*****************************/
class TEASE_EXPORT LogManager
{
public:
    static void setLogger(ILogger *logger);

public:
    static void registerLog(ILogger::Level level, const char *filename, int lineNumber, const char *fctName, const std::string &msg);

private:
    static ILogger *m_refLogger;
};

} // namespace tease

#endif // TEASE_LOGS_ABSTRACTLOGGER_H

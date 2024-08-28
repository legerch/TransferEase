#ifndef TEASE_LOGS_LOGMANAGER_H
#define TEASE_LOGS_LOGMANAGER_H

#include "ilogger.h"

/*****************************/
/* Macro definitions         */
/*****************************/

/* Use to retrieve context informations */
#define _TEASE_LOG_CTX_FILE   static_cast<const char *>(__FILE__)
#define _TEASE_LOG_CTX_LINE   __LINE__
#define _TEASE_LOG_CTX_FCT    static_cast<const char *>(TEASE_FCTNAME)

/* Create macros for easier log usage */
#define _TEASE_LOG_GENERIC(level, msg)    tease::LogManager::registerLog(level, _TEASE_LOG_CTX_FILE, _TEASE_LOG_CTX_LINE, _TEASE_LOG_CTX_FCT, msg)

#define TEASE_LOG_FATAL(msg)  _TEASE_LOG_GENERIC(tease::ILogger::LOG_LVL_FATAL, msg)
#define TEASE_LOG_ERROR(msg)  _TEASE_LOG_GENERIC(tease::ILogger::LOG_LVL_ERROR, msg)
#define TEASE_LOG_WARN(msg)   _TEASE_LOG_GENERIC(tease::ILogger::LOG_LVL_WARNING, msg)
#define TEASE_LOG_INFO(msg)   _TEASE_LOG_GENERIC(tease::ILogger::LOG_LVL_INFO, msg)
#define TEASE_LOG_DEBUG(msg)  _TEASE_LOG_GENERIC(tease::ILogger::LOG_LVL_DEBUG, msg)

/*****************************/
/* Namespace definitions     */
/*****************************/
namespace tease{

/*****************************/
/* Class definitions         */
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

#endif // TEASE_LOGS_LOGMANAGER_H
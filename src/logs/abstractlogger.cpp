#include "transferease/logs/abstractlogger.h"

/*****************************/
/* Class documentations      */
/*****************************/

/*****************************/
/* Start namespace           */
/*****************************/

namespace tease
{

/*****************************/
/* Static initialization     */
/*****************************/
ILogger* LogManager::m_refLogger = nullptr;

/*****************************/
/* Functions implementation  */
/*        LogManager         */
/*****************************/

void LogManager::setLogger(ILogger *logger)
{
    m_refLogger = logger;
}

void LogManager::registerLog(ILogger::Level level, const char *filename, int lineNumber, const char *fctName, const std::string &msg)
{
    /* Verify that logger interface is valid */
    if(!m_refLogger){
        return;
    }

    /* Set context informations */
    const LogContext ctx(filename, lineNumber, fctName);

    /* Register log */
    m_refLogger->log(level, ctx, msg);
}


/*****************************/
/* End namespace             */
/*****************************/

} // namespace tease

/*****************************/
/* End file                  */
/*****************************/

#include "logmanager.h"

/*****************************/
/* Class documentations      */
/*****************************/

/*****************************/
/* Enum documentations       */
/*****************************/

/*****************************/
/* Structure documentations  */
/*****************************/

/*****************************/
/* Signals documentations    */
/*****************************/

/*****************************/
/* Macro definitions         */
/*****************************/

/*****************************/
/* Start namespace           */
/*****************************/

namespace tease{

/*****************************/
/* Constants definitions     */
/*****************************/

/*****************************/
/* Static initialization     */
/*****************************/
ILogger* LogManager::m_refLogger = nullptr;

/*****************************/
/* Functions implementations */
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
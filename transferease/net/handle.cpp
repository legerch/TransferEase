#include "handle.h"

#include <curl/curl.h>

#include "logs/logmanager.h"
#include "tools/stringhelper.h"

/*****************************/
/* Class documentations      */
/*****************************/

/*!
 * \class tease::Handle
 * \brief Handle use for any transfer manager entity
 * \details
 * This class allow to manage necessary initialization/desinitialization
 * of used libraries, which should not be called more than once for the
 * all program. \n
 * This class use the \b singleton pattern.
 */

/*****************************/
/* Macro definitions         */
/*****************************/

/*****************************/
/* Start namespace           */
/*****************************/

namespace tease
{

/*****************************/
/* Functions implementation  */
/*****************************/
Handle& Handle::instance()
{
    static Handle instance;
    return instance;
}

Handle::Handle()
{
    const CURLcode idCode = curl_global_init(CURL_GLOBAL_ALL);
    if(idCode != CURLE_OK){
        const std::string err = StringHelper::format("Failed to initialise curl globally [id-err: %d]", idCode);
        TEASE_LOG_FATAL(err);

        throw std::runtime_error(err);
    }
}

Handle::~Handle()
{
    curl_global_cleanup();
}

/*****************************/
/* Constants definitions     */
/*****************************/

/*****************************/
/* End namespace             */
/*****************************/

} // namespace tease

/*****************************/
/* End file                  */
/*****************************/

#include "transfermanager.h"

#include <curl/curl.h>

#include "logs/logmanager.h"
#include "net/handle.h"
#include "tools/stringhelper.h"

/*****************************/
/* Class documentations      */
/*****************************/

/*!
 * \class tease::Url
 * \brief Use to manage URLs
 * \details
 * Some developer useful docs:
 * - https://everything.curl.dev/
 * - https://curl.se/libcurl/c/
 */

/*****************************/
/* Macro definitions         */
/*****************************/
#define DEFAULT_NB_MAX_TRIALS   1

/*****************************/
/* Start namespace           */
/*****************************/

namespace tease
{

/*****************************/
/* Functions definitions     */
/*      Private Class        */
/*****************************/

class TransferManager::Impl final
{

public:
    explicit Impl(TransferManager *parent);
    ~Impl();

public:
    void init();

private:
    static void defaultCbStarted(Request::TypeTransfer typeTransfer);
    static void defaultCbProgress(Request::TypeTransfer typeTransfer, size_t transferTotal, size_t transferNow);
    static void defaultCbCompleted(Request::TypeTransfer typeTransfer);
    static void defaultCbFailed(Request::TypeTransfer typeTransfer);

public:
    CURLM* m_handleMulti = nullptr;

    int m_nbMaxTrials;

    CbStarted m_cbStarted;
    CbProgress m_cbProgress;
    CbCompleted m_cbCompleted;
    CbFailed m_cbFailed;

    TransferManager *m_parent;
};

/*****************************/
/* Functions implementation  */
/*      Private Class        */
/*****************************/

TransferManager::Impl::Impl(TransferManager *parent)
{
    /* Manage library handle */
    Handle::instance();

    /* Set properties */
    m_handleMulti = curl_multi_init();
    if(!m_handleMulti){
        const std::string err = "Failed to initialise curl multi instance";
        TEASE_LOG_FATAL(err);

        throw std::runtime_error(err);
    }

    m_nbMaxTrials = DEFAULT_NB_MAX_TRIALS;
    m_parent = parent;
}

TransferManager::Impl::~Impl()
{
    curl_multi_cleanup(m_handleMulti);
}

void TransferManager::Impl::init()
{
    /* Set default callbacks */
    m_parent->setCbStarted(defaultCbStarted);
    m_parent->setCbProgress(defaultCbProgress);
    m_parent->setCbCompleted(defaultCbCompleted);
    m_parent->setCbFailed(defaultCbFailed);
}

void TransferManager::Impl::defaultCbStarted(Request::TypeTransfer typeTransfer)
{
    const std::string str = StringHelper::format("Default callback \"started\" [type-transfer: %d]", typeTransfer);
    TEASE_LOG_INFO(str);
}

void TransferManager::Impl::defaultCbProgress(Request::TypeTransfer typeTransfer, size_t transferTotal, size_t transferNow)
{
    const std::string str = StringHelper::format("Default callback \"progress\" [type-transfer: %d, total: %zu, now: %zu]", typeTransfer, transferTotal, transferNow);
    TEASE_LOG_INFO(str);
}

void TransferManager::Impl::defaultCbCompleted(Request::TypeTransfer typeTransfer)
{
    const std::string str = StringHelper::format("Default callback \"completed\" [type-transfer: %d]", typeTransfer);
    TEASE_LOG_INFO(str);
}

void TransferManager::Impl::defaultCbFailed(Request::TypeTransfer typeTransfer)
{
    const std::string str = StringHelper::format("Default callback \"failed\" [type-transfer: %d]", typeTransfer);
    TEASE_LOG_INFO(str);
}

/*****************************/
/* Functions implementation  */
/*      Public Class         */
/*****************************/

TransferManager::TransferManager() :
    d_ptr(std::make_unique<Impl>(this))
{
    d_ptr->init();
}

TransferManager::~TransferManager() = default;

void TransferManager::setCbStarted(CbStarted fct)
{
    d_ptr->m_cbStarted = fct;
}

void TransferManager::setCbProgress(CbProgress fct)
{
    d_ptr->m_cbProgress = fct;
}

void TransferManager::setCbCompleted(CbCompleted fct)
{
    d_ptr->m_cbCompleted = fct;
}

void TransferManager::setCbFailed(CbFailed fct)
{
    d_ptr->m_cbFailed = fct;
}

double TransferManager::transferProgressToPercent(size_t transferTotal, size_t transferNow)
{
    return (static_cast<double>(transferNow) / transferTotal) * 100.0;
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

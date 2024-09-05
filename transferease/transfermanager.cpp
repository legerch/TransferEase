#include "transfermanager.h"

#include <curl/curl.h>
#include <future>
#include <mutex>

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
    using Thread = std::future<void>;
    using Locker = std::lock_guard<std::mutex>;

public:
    explicit Impl(TransferManager *parent);
    ~Impl();

public:
    void init();

private:
    static size_t curlCbWrite(char *ptr, size_t size, size_t nmemb, void *userdata);
    static size_t curlCbRead(char *buffer, size_t size, size_t nitems, void *userdata);
    static int curlCbProgress(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);

private:
    static void defaultCbStarted(Request::TypeTransfer typeTransfer);
    static void defaultCbProgress(Request::TypeTransfer typeTransfer, size_t transferTotal, size_t transferNow);
    static void defaultCbCompleted(Request::TypeTransfer typeTransfer);
    static void defaultCbFailed(Request::TypeTransfer typeTransfer);

public:
    CURLM* m_handleMulti = nullptr;

    std::string m_username;
    std::string m_userpwd;
    int m_nbMaxTrials;

    Thread m_threadTransfer;
    std::mutex m_mutex;

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

size_t TransferManager::Impl::curlCbWrite(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    /* Cast elements */
    Request *req = static_cast<Request*>(userdata);
    BytesArray::Byte *bufferData = reinterpret_cast<BytesArray::Byte*>(ptr);

    /* Fill request data */
    const size_t bufferSize = size * nmemb;
    req->getData().pushBack(bufferData, bufferSize);

    return bufferSize;
}

size_t TransferManager::Impl::curlCbRead(char *buffer, size_t size, size_t nitems, void *userdata)
{
    /* Cast elements */
    Request *req = static_cast<Request*>(userdata);

    /* Read request data */
    const size_t bufferSize = size * nitems;
    return req->ioRead(buffer, bufferSize);
}

int TransferManager::Impl::curlCbProgress(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
    /* Cast elements */
    Request *req = static_cast<Request*>(clientp);

    /* Update transfer status */
    switch(req->getTypeTransfer())
    {
        case Request::TRANSFER_DOWNLOAD:{
            req->ioSetSizeTotal(dltotal);
            req->ioSetSizeCurrent(dlnow);
        }break;

        case Request::TRANSFER_UPLOAD:{
            req->ioSetSizeTotal(ultotal);
            req->ioSetSizeCurrent(ulnow);
        }break;

        default: break;
    }

    return 0; // Return 0 to continue the transfer, non-zero to abort
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

bool TransferManager::transferIsInProgress() const
{
    /* Do thread is set ? */
    if(!d_ptr->m_threadTransfer.valid()){
        return false;
    }

    /* Do thread is currently running ? */
    return d_ptr->m_threadTransfer.wait_for(std::chrono::seconds(0)) != std::future_status::ready;
}

void TransferManager::setUserInfos(const std::string &username, const std::string &passwd)
{
    Impl::Locker locker(d_ptr->m_mutex);

    d_ptr->m_username = username;
    d_ptr->m_userpwd = passwd;
}

void TransferManager::setNbMaxTrials(int nbTrials)
{
    Impl::Locker locker(d_ptr->m_mutex);

    nbTrials = std::max(0, nbTrials);
    d_ptr->m_nbMaxTrials = nbTrials;
}

void TransferManager::setCbStarted(CbStarted fct)
{
    Impl::Locker locker(d_ptr->m_mutex);
    d_ptr->m_cbStarted = fct;
}

void TransferManager::setCbProgress(CbProgress fct)
{
    Impl::Locker locker(d_ptr->m_mutex);
    d_ptr->m_cbProgress = fct;
}

void TransferManager::setCbCompleted(CbCompleted fct)
{
    Impl::Locker locker(d_ptr->m_mutex);
    d_ptr->m_cbCompleted = fct;
}

void TransferManager::setCbFailed(CbFailed fct)
{
    Impl::Locker locker(d_ptr->m_mutex);
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

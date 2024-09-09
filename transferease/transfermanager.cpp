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
 * \class tease::TransferManager
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

    void jobDownload();

private:
    bool downloadPrepare();

    void updateProgress(Request::TypeTransfer typeTransfer);
    bool manageStatus(Request::TypeTransfer typeTransfer, IdError &idErr);
    bool errorAllowRetry(CURLcode curlErr, IdError &idErr);

    void cleanHandles();
    void cleanRequests();

    void configureHandleDl(CURL *handle, Request *req);

private:
    static size_t curlCbWrite(char *ptr, size_t size, size_t nmemb, void *userdata);
    static size_t curlCbRead(char *buffer, size_t size, size_t nitems, void *userdata);
    static int curlCbProgress(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);

private:
    static void defaultCbStarted(Request::TypeTransfer typeTransfer);
    static void defaultCbProgress(Request::TypeTransfer typeTransfer, size_t transferTotal, size_t transferNow);
    static void defaultCbCompleted(Request::TypeTransfer typeTransfer);
    static void defaultCbFailed(Request::TypeTransfer typeTransfer, IdError idErr);

public:
    CURLM* m_handleMulti = nullptr;

    Request::List m_listReqs;

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
    cleanHandles();
    cleanRequests();

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

void TransferManager::Impl::jobDownload()
{
    IdError failureStatus = ERR_NO_ERROR;
    int stillRunning = 0;

    /* Inform that download is started */
    m_cbStarted(Request::TRANSFER_DOWNLOAD);

    /* Perform download preparation */
    bool succeed = downloadPrepare();
    if(!succeed){
        failureStatus = ERR_INTERNAL;
        goto stat_clean;
    }

    /* Perform download */
    do{
        // Execute download
        CURLMcode idErr = curl_multi_perform(m_handleMulti, &stillRunning);
        if(idErr != CURLM_OK){
            failureStatus = ERR_INTERNAL;

            const std::string err = StringHelper::format("Error when trying to perform on multi handle [id-err: %d]", idErr);
            TEASE_LOG_ERROR(err);
            goto stat_clean;
        }

        // Update progress
        updateProgress(Request::TRANSFER_DOWNLOAD);

        // Manage status
        bool canContinue = manageStatus(Request::TRANSFER_DOWNLOAD, failureStatus);
        if(!canContinue){
            goto stat_clean;
        }

        // Perform polling
        curl_multi_wait(m_handleMulti, nullptr, 0, 1000, nullptr);
    }while(stillRunning);

    /* Clean used ressources */
stat_clean:
    cleanHandles();
    cleanRequests();

    /* Inform user about transfer status */
    if(failureStatus == ERR_NO_ERROR){
        m_cbCompleted(Request::TRANSFER_DOWNLOAD);
    }else{
        m_cbFailed(Request::TRANSFER_DOWNLOAD, failureStatus);
    }
}

bool TransferManager::Impl::downloadPrepare()
{
    Locker locker(m_mutex);

    /* Reset any current handle */
    cleanHandles();

    /* Prepare curl handle from request */
    for(auto &req : m_listReqs){
        // Create handle
        CURL *handle = curl_easy_init();
        if(!handle){
            TEASE_LOG_ERROR("Failed to initialize easy handle");
            return false;
        }

        // Configure it
        configureHandleDl(handle, req.get());
        curl_multi_add_handle(m_handleMulti, handle);
    }

    return true;
}

void TransferManager::Impl::updateProgress(Request::TypeTransfer typeTransfer)
{
    /* Calculate list progress */
    size_t sizeTotal = 0, sizeCurrent = 0;
    for(auto req : m_listReqs){
        sizeTotal += req->ioGetSizeTotal();
        sizeCurrent += req->ioGetSizeCurrent();
    }

    /* Inform user */
    m_cbProgress(typeTransfer, sizeTotal, sizeCurrent);
}

bool TransferManager::Impl::manageStatus(Request::TypeTransfer typeTransfer, IdError &idErr)
{
    CURLMsg *msg = nullptr;
    int nbMsgLeft = 0;

    /* Manage status of each request */
    while((msg = curl_multi_info_read(m_handleMulti, &nbMsgLeft))){
        // Ignore request not finished
        if(msg->msg != CURLMSG_DONE){
            continue; // Read status of next request
        }

        // Ignore requests which succeed
        const CURLcode curlErr = msg->data.result;
        if(curlErr == CURLE_OK){
            continue;
        }

        // Do error allow us to a retry ? */
        const bool retryAllowed = errorAllowRetry(curlErr, idErr);
        if(!retryAllowed){
            return false;
        }

        // Retrieve current request informations
        CURL *handle = msg->easy_handle;
        Request *req = nullptr;
        curl_easy_getinfo(handle, CURLINFO_PRIVATE, &req);

        // Have we reach maximal number of retry for this request ?
        if(req->ioGetNbTrials() >= m_nbMaxTrials){
            const std::string err = StringHelper::format("Reached maximum number of trials [url: %s, curl-err: %d]", req->getUrl().toString().c_str(), curlErr);
            TEASE_LOG_WARN(err);

            idErr = ERR_MAX_TRIALS;
            return false;
        }

        // Prepare new trial for current request
        req->ioReset();

        curl_multi_remove_handle(m_handleMulti, handle);
        curl_easy_reset(handle);

        switch(typeTransfer){
            case Request::TRANSFER_DOWNLOAD: configureHandleDl(handle, req); break;
            default: break;
        }

        curl_multi_add_handle(m_handleMulti, handle);
    }

    return true;
}

bool TransferManager::Impl::errorAllowRetry(CURLcode curlErr, IdError &idErr)
{
    switch(curlErr)
    {
        case CURLE_UNSUPPORTED_PROTOCOL:
        case CURLE_NOT_BUILT_IN:
        case CURLE_OUT_OF_MEMORY:{
            const std::string err = StringHelper::format("Received internal error which require attention [curl-err: %d]", curlErr);
            TEASE_LOG_FATAL(err);

            idErr = ERR_INTERNAL;
        }break;

        case CURLE_REMOTE_DISK_FULL:        idErr = ERR_MEMORY_FULL_REMOTE; break;
        case CURLE_URL_MALFORMAT:           idErr = ERR_INVALID_REQUEST;    break;
        case CURLE_REMOTE_FILE_NOT_FOUND:   idErr = ERR_CONTENT_NOT_FOUND;  break;
        case CURLE_LOGIN_DENIED:            idErr = ERR_INVALID_LOGIN;      break;

        default:{
            return true; // Any other errors allow to perform new try
        }break;
    }

    return false;
}

void TransferManager::Impl::cleanHandles()
{
    CURL **list = curl_multi_get_handles(m_handleMulti);
    if(list){
        // Clean any handle
        for(int i = 0; list[i]; ++i){
            curl_multi_remove_handle(m_handleMulti, list[i]);
            curl_easy_cleanup(list[i]);
        }

        // Clean array
        curl_free(list);
    }
}

void TransferManager::Impl::cleanRequests()
{
    m_listReqs.clear();
}

void TransferManager::Impl::configureHandleDl(CURL *handle, Request *req)
{
    /* URL informations */
    const Url &url = req->getUrl();
    curl_easy_setopt(handle, CURLOPT_URL, url.toString().c_str());

    switch(url.getIdScheme())
    {
        case Url::SCHEME_FTPS:{
            curl_easy_setopt(handle, CURLOPT_USE_SSL, CURLUSESSL_ALL);
        }TEASE_FALLTHROUGH;

        case Url::SCHEME_FTP:{
            curl_easy_setopt(handle, CURLOPT_USERNAME, m_username.c_str());
            curl_easy_setopt(handle, CURLOPT_PASSWORD, m_userpwd.c_str());
        }break;

        case Url::SCHEME_HTTPS:{
            curl_easy_setopt(handle, CURLOPT_USE_SSL, CURLUSESSL_ALL);
        }break;

        default: break;
    }

    /* Request datas */
    curl_easy_setopt(handle, CURLOPT_PRIVATE, req);

    /* Write callback */
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, curlCbWrite);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, req);

    /* Progress callback */
    curl_easy_setopt(handle, CURLOPT_XFERINFOFUNCTION, curlCbProgress);
    curl_easy_setopt(handle, CURLOPT_XFERINFODATA, req);
    curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 0L); // Enable progress meter
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

void TransferManager::Impl::defaultCbFailed(Request::TypeTransfer typeTransfer, IdError idErr)
{
    const std::string str = StringHelper::format("Default callback \"failed\" [type-transfer: %d, id-err: %d]", typeTransfer, idErr);
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

TransferManager::IdError TransferManager::startDownload(const Request::List &listReqs)
{
    /* Verify that a transfer is not already running */
    if(transferIsInProgress()){
        TEASE_LOG_ERROR("Unable to start download, transfer already in progress");
        return ERR_BUSY;
    }

    /* Verify that list is not empty */
    if(listReqs.empty()){
        TEASE_LOG_ERROR("List of requests is empty, no download process to perform");
        return ERR_INVALID_REQUEST;
    }

    /* Verify requests validity */
    for(const auto &req : listReqs){
        if(req->getTypeTransfer() != Request::TRANSFER_DOWNLOAD){
            TEASE_LOG_ERROR("Receive request with a transfer type different than download, not allowed when trying to perform download");
            return ERR_INVALID_REQUEST;
        }

        const Url &url = req->getUrl();
        if(!url.isValid()){
            const std::string err = StringHelper::format("Receive invalid URL [id-scheme: %d, host: %s, path: %s]", url.getIdScheme(), url.getHost().c_str(), url.getPath().c_str());
            TEASE_LOG_ERROR(err);
            return ERR_INVALID_REQUEST;
        }
    }

    /* Register requests */
    d_ptr->m_listReqs = listReqs;

    /* Start download process */
    d_ptr->m_threadTransfer = std::async(std::launch::async, &Impl::jobDownload, d_ptr.get());

    return ERR_NO_ERROR;
}

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

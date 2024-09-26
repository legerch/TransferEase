#include "transferease/transfermanager.h"

#include <curl/curl.h>
#include <future>
#include <mutex>

#include "transferease/logs/abstractlogger.h"

#include "net/handle.h"
#include "tools/stringhelper.h"

/*****************************/
/* Class documentations      */
/*****************************/

/*!
 * \class tease::TransferManager
 * \brief Use to perform download/upload
 * of ressources easily.
 * \details
 * This class will allow to easily perform download/upload
 * ressources from/to a remote. \n
 * A simple example used to download/upload a list of request:
 * \include{lineno} transfer-usage.cpp
 *
 * \note
 * For a more \em real-world example, we can refer
 * to application using this library at: \n
 * https://github.com/legerch/TransferEaseApp
 *
 * This class allow to register custom callbacks using
 * \c std::function type. We have multiple ways to register
 * a callback function:
 * \include callback-registration.cpp
 *
 * \note
 * Some developer useful docs:
 * - https://everything.curl.dev/
 * - https://curl.se/libcurl/c/
 *
 * \sa startDownload()
 * \sa tease::Url
 */

/*****************************/
/* Callbacks documentations  */
/*****************************/

/*!
 * \typedef TransferManager::CbStarted
 * \brief Callback called when transfer has been started
 *
 * \param[in] typeTransfer
 * Type of transfer being started.
 *
 * \sa setCbStarted()
 * \sa startDownload()
 */

/*!
 * \typedef TransferManager::CbProgress
 * \brief Callback called during transfer
 *
 * \param[in] typeTransfer
 * Type of transfer being started.
 * \param[in] transferTotal
 * Total size of the transfer in bytes
 * \param[in] transferNow
 * Current values of transfered data in bytes
 *
 * \sa setCbProgress()
 * \sa startDownload()
 */

/*!
 * \typedef TransferManager::CbCompleted
 * \brief Callback called when transfer finished
 * and succeed
 *
 * \param[in] typeTransfer
 * Type of transfer which succeeded to complete.
 *
 * \sa setCbCompleted()
 * \sa startDownload()
 */

/*!
 * \typedef TransferManager::CbFailed
 * \brief Callback called when transfer finished
 * due to an error
 *
 * \param[in] typeTransfer
 * Type of transfer which failed to complete.
 *
 * \sa setCbFailed()
 * \sa startDownload()
 */

/*****************************/
/* Macro definitions         */
/*****************************/
#define DEFAULT_NB_MAX_TRIALS       1
#define DEFAULT_TIMEOUT_CONNECT     10L /**< Unit in seconds */
#define DEFAULT_TIMEOUT_TRANSFER    10L /**< Unit in seconds */

#define MIN_SPEED_LIMIT             30L /**< Unit in bytes/sec */

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

    IdError jobPrepare(Request::TypeTransfer typeTransfer, const Request::List &listReqs);
    void jobPerform();

private:
    bool transferPrepare();
    bool performTransfer(IdError &idErr);
    void updateProgress();
    IdError manageStatus(int &counterReqsDone);
    bool errorAllowRetry(CURLcode curlErr, IdError &idErr);

    void cleanHandles();
    void cleanRequests();

    void configureHandle(CURL *handle, Request *req);

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

    Request::TypeTransfer m_typeTransfer;
    Request::List m_listReqs;

    std::string m_username;
    std::string m_userpwd;
    int m_nbMaxTrials;
    long m_timeoutConnect;
    long m_timeoutTransfer;
    FlagOption m_options;

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
    m_timeoutConnect = DEFAULT_TIMEOUT_CONNECT;
    m_timeoutTransfer = DEFAULT_TIMEOUT_TRANSFER;
    m_options = FlagOption::OPT_NONE;
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

TransferManager::IdError TransferManager::Impl::jobPrepare(Request::TypeTransfer typeTransfer, const Request::List &listReqs)
{
    /* Verify that a transfer is not already running */
    if(m_parent->transferIsInProgress()){
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
        // Do all requests are expected transfer type ?
        if(req->getTypeTransfer() != typeTransfer){
            const std::string err = StringHelper::format("Receive request with a transfer type different than expected [type-req: %d, type-exp: %d]", req->getTypeTransfer(), typeTransfer);
            TEASE_LOG_ERROR(err);
            return ERR_INVALID_REQUEST;
        }

        // Do URL is valid ?
        const Url &url = req->getUrl();
        if(!url.isValid()){
            const std::string err = StringHelper::format("Receive invalid URL [id-scheme: %d, host: %s, path: %s]", url.getIdScheme(), url.getHost().c_str(), url.getPath().c_str());
            TEASE_LOG_ERROR(err);
            return ERR_INVALID_REQUEST;
        }

        // Verify that datas are not empty for upload transfer
        if(typeTransfer == Request::TRANSFER_UPLOAD){
            const BytesArray &data = req->getData();
            if(data.isEmpty()){
                const std::string err = StringHelper::format("Receive empty data request for upload [id-scheme: %d, host: %s, path: %s]", url.getIdScheme(), url.getHost().c_str(), url.getPath().c_str());
                TEASE_LOG_ERROR(err);
                return ERR_INVALID_REQUEST;
            }
        }
    }

    /* Register requests */
    m_typeTransfer = typeTransfer;
    m_listReqs = listReqs;

    return ERR_NO_ERROR;
}

void TransferManager::Impl::jobPerform()
{
    const int nbReqsTodo = m_listReqs.size();
    int nbReqsDone = 0;

    IdError failureStatus = ERR_NO_ERROR;

    /* Inform that transfer is started */
    m_cbStarted(m_typeTransfer);

    /* Perform transfer preparation */
    bool succeed = transferPrepare();
    if(!succeed){
        failureStatus = ERR_INTERNAL;
        goto stat_clean;
    }

    /* Do first call to perform transfer */
    succeed = performTransfer(failureStatus);
    if(!succeed){
        goto stat_clean;
    }

    /* Perform transfer */
    while(nbReqsDone < nbReqsTodo){
        // Perform polling
        curl_multi_wait(m_handleMulti, nullptr, 0, 1000, nullptr);
        succeed = performTransfer(failureStatus);
        if(!succeed){
            goto stat_clean;
        }

        // Update progress
        updateProgress();

        // Manage status
        failureStatus = manageStatus(nbReqsDone);
        if(failureStatus != ERR_NO_ERROR){
            goto stat_clean;
        }
    }

    /* Clean used ressources */
stat_clean:
    cleanHandles();
    cleanRequests();

    /* Inform user about transfer status */
    if(failureStatus == ERR_NO_ERROR){
        m_cbCompleted(m_typeTransfer);
    }else{
        m_cbFailed(m_typeTransfer, failureStatus);
    }
}

bool TransferManager::Impl::transferPrepare()
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
        configureHandle(handle, req.get());
        curl_multi_add_handle(m_handleMulti, handle);
    }

    return true;
}

bool TransferManager::Impl::performTransfer(IdError &idErr)
{
    int nbReqsRunning;

    CURLMcode curlErr = curl_multi_perform(m_handleMulti, &nbReqsRunning);
    if(curlErr != CURLM_OK){
        idErr = ERR_INTERNAL;

        const std::string err = StringHelper::format("Error when trying to perform on multi handle [curl-err: %d]", curlErr);
        TEASE_LOG_ERROR(err);
        return false;
    }

    return true;
}

void TransferManager::Impl::updateProgress()
{
    /* Calculate list progress */
    size_t sizeTotal = 0, sizeCurrent = 0;
    for(const auto &req : m_listReqs){
        sizeTotal += req->ioGetSizeTotal();
        sizeCurrent += req->ioGetSizeCurrent();
    }

    /* Inform user */
    m_cbProgress(m_typeTransfer, sizeTotal, sizeCurrent);
}

TransferManager::IdError TransferManager::Impl::manageStatus(int &counterReqsDone)
{
    IdError idErr = ERR_NO_ERROR;
    CURLMsg *msg = nullptr;
    int nbMsgLeft = 0;

    /* Manage status of each request */
    while((msg = curl_multi_info_read(m_handleMulti, &nbMsgLeft))){
        // Ignore request not finished
        if(msg->msg != CURLMSG_DONE){
            continue; // Read status of next request
        }

        // Count requests which succeed
        const CURLcode curlErr = msg->data.result;
        if(curlErr == CURLE_OK){
            ++counterReqsDone;
            continue;
        }

        // Do error allow us to a retry ? */
        const bool retryAllowed = errorAllowRetry(curlErr, idErr);
        if(!retryAllowed){
            return idErr;
        }

        // Retrieve current request informations
        CURL *handle = msg->easy_handle;
        Request *req = nullptr;
        curl_easy_getinfo(handle, CURLINFO_PRIVATE, &req);

        // Have we reach maximal number of retry for this request ?
        if(req->ioGetNbTrials() >= m_nbMaxTrials){
            const std::string err = StringHelper::format("Reached maximum number of trials [url: %s, curl-err: %d]", req->getUrl().toString().c_str(), curlErr);
            TEASE_LOG_WARN(err);

            return ERR_MAX_TRIALS;
        }

        // Prepare new trial for current request
        const std::string logTrial = StringHelper::format("Perform new trial for request [url: %s, nb-trials: %d, curl-err: %d]", req->getUrl().toString().c_str(), req->ioGetNbTrials(), curlErr);
        TEASE_LOG_DEBUG(logTrial);

        req->ioRegisterTry();

        curl_multi_remove_handle(m_handleMulti, handle);
        curl_easy_reset(handle);

        configureHandle(handle, req);
        curl_multi_add_handle(m_handleMulti, handle);
    }

    return ERR_NO_ERROR;
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

void TransferManager::Impl::configureHandle(CURL *handle, Request *req)
{
    /* URL informations */
    const Url &url = req->getUrl();
    curl_easy_setopt(handle, CURLOPT_URL, url.toString().c_str());

    /* Manage protocols behaviours */
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

    /* Manage configurations options related to the transfer type */
    switch(m_typeTransfer)
    {
        case Request::TRANSFER_DOWNLOAD:{
            // Manage write callbacks
            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, curlCbWrite);
            curl_easy_setopt(handle, CURLOPT_WRITEDATA, req);
        }break;

        case Request::TRANSFER_UPLOAD:{
            // Manage upload configuration
            curl_easy_setopt(handle, CURLOPT_UPLOAD, 1L);
            curl_easy_setopt(handle, CURLOPT_INFILESIZE_LARGE, static_cast<curl_off_t>(req->getData().getSize()));

            // Manage read callbacks
            curl_easy_setopt(handle, CURLOPT_READFUNCTION, curlCbRead);
            curl_easy_setopt(handle, CURLOPT_READDATA, req);

            // Manage available options
            if(m_options & FlagOption::OPT_FTP_CREATE_DIRS){
                curl_easy_setopt(handle, CURLOPT_FTP_CREATE_MISSING_DIRS, CURLFTP_CREATE_DIR);
            }
        }break;

        default: break;
    }

    /* Progress callback */
    curl_easy_setopt(handle, CURLOPT_XFERINFOFUNCTION, curlCbProgress);
    curl_easy_setopt(handle, CURLOPT_XFERINFODATA, req);
    curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 0L); // Enable progress meter

    /* Manage timeouts */
    // Maximum time allowed to connect to host
    curl_easy_setopt(handle, CURLOPT_CONNECTTIMEOUT, m_timeoutConnect);

    // Minimum allowed speed (if transfer rate is below the limit for the configured timeout transfer, transfer will timeout)
    curl_easy_setopt(handle, CURLOPT_LOW_SPEED_LIMIT, MIN_SPEED_LIMIT);
    curl_easy_setopt(handle, CURLOPT_LOW_SPEED_TIME, m_timeoutTransfer);
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

/*!
 * \brief Use to start downloading list of requests
 *
 * \param[in, out] listReqs
 * List of requests to download. \n
 * This argument is a list of request pointers, those will be directly
 * filled with downloaded datas, so pointers must remains valid. \n
 * Once transfer is finished, user can read request content directly
 *
 * \note
 * This method is \em thread-safe
 * \note
 * This method is asynchronous, so please use dedicated callbacks
 * to manage transfer status.
 *
 * \return
 * Returns \c TransferManager::ERR_NO_ERROR if download succeed to be prepared. \n
 * This method will return \c TransferManager::ERR_BUSY error if a transfer is already
 * running or if called from a callback.
 *
 * \sa startUpload()
 */
TransferManager::IdError TransferManager::startDownload(const Request::List &listReqs)
{
    /* Perform pre-job verifications */
    const IdError idErr = d_ptr->jobPrepare(Request::TRANSFER_DOWNLOAD, listReqs);
    if(idErr != ERR_NO_ERROR){
        return idErr;
    }

    /* Start download process */
    d_ptr->m_threadTransfer = std::async(std::launch::async, &Impl::jobPerform, d_ptr.get());

    return ERR_NO_ERROR;
}

/*!
 * \brief Use to start upload list of requests
 *
 * \param[in, out] listReqs
 * List of requests to upload. \n
 * This argument is a list of request pointers, those will be directly
 * read in order to upload datas, so pointers must remains valid. \n
 * Once transfer is finished, user can still use request content.
 *
 * \note
 * This method is \em thread-safe
 * \note
 * This method is asynchronous, so please use dedicated callbacks
 * to manage transfer status.
 *
 * \return
 * Returns \c TransferManager::ERR_NO_ERROR if upload succeed to be prepared. \n
 * This method will return \c TransferManager::ERR_BUSY error if a transfer is already
 * running or if called from a callback.
 *
 * \sa startDownload()
 */
TransferManager::IdError TransferManager::startUpload(const Request::List &listReqs)
{
    /* Perform pre-job verifications */
    const IdError idErr = d_ptr->jobPrepare(Request::TRANSFER_UPLOAD, listReqs);
    if(idErr != ERR_NO_ERROR){
        return idErr;
    }

    /* Start upload process */
    d_ptr->m_threadTransfer = std::async(std::launch::async, &Impl::jobPerform, d_ptr.get());

    return ERR_NO_ERROR;
}

/*!
 * \brief Verify is a tranfer is in progress
 * or not
 *
 * \note
 * This method is \em thread-safe
 *
 * \return
 * Returns \c true if a transfer is currently running
 */
bool TransferManager::transferIsInProgress() const
{
    Impl::Locker locker(d_ptr->m_mutex);

    /* Do thread is set ? */
    if(!d_ptr->m_threadTransfer.valid()){
        return false;
    }

    /* Do thread is currently running ? */
    return d_ptr->m_threadTransfer.wait_for(std::chrono::seconds(0)) != std::future_status::ready;
}

/*!
 * \brief Retrieve maximum number of trials
 * currently set.
 *
 * \note
 * This method is \em thread-safe
 *
 * \return
 * Returns maximum number of trials currently set.
 *
 * \sa setNbMaxTrials()
 */
int TransferManager::getNbMaxTrials() const
{
    Impl::Locker locker(d_ptr->m_mutex);
    return d_ptr->m_nbMaxTrials;
}

/*!
 * \brief Retrieve connection timeout.
 *
 * \note
 * This method is \em thread-safe
 *
 * \return
 * Returns connection timeout in seconds
 * currently set.
 *
 * \sa setTimeoutConnection()
 */
long TransferManager::getTimeoutConnection() const
{
    Impl::Locker locker(d_ptr->m_mutex);
    return d_ptr->m_timeoutConnect;
}

/*!
 * \brief Retrieve transfer timeout.
 *
 * \note
 * This method is \em thread-safe
 *
 * \return
 * Returns transfer timeout in seconds
 * currently set.
 *
 * \sa setTimeoutTransfer()
 */
long TransferManager::getTimeoutTransfer() const
{
    Impl::Locker locker(d_ptr->m_mutex);
    return d_ptr->m_timeoutTransfer;
}

/*!
 * \brief Retrieve transfer options.
 *
 * \note
 * This method is \em thread-safe
 *
 * \return
 * Returns transfer options.
 *
 * \sa setOptions()
 */
TransferManager::FlagOption TransferManager::getOptions() const
{
    Impl::Locker locker(d_ptr->m_mutex);
    return d_ptr->m_options;
}

/*!
 * \brief Use to set user informations
 * \details
 * Can be useful if server require authentication.
 *
 * \param[in] username
 * Login username to use.
 * \param[in] passwd
 * Login password to use.
 *
 * \note
 * This method is \em thread-safe
 * \note
 * If credentials are invalid, transfer will failed
 * with error TransferManager::ERR_INVALID_LOGIN
 */
void TransferManager::setUserInfos(const std::string &username, const std::string &passwd)
{
    Impl::Locker locker(d_ptr->m_mutex);

    d_ptr->m_username = username;
    d_ptr->m_userpwd = passwd;
}

/*!
 * \brief Use to set maximum number
 * of trials
 * \details
 * If a request fail, it will be restarted until
 * max number of trials is reached. \n
 * Default value is: \c 1
 *
 * \param[in] nbTrials
 * Maximum number of trials allowed
 *
 * \note
 * This method is \em thread-safe
 *
 * \sa getNbMaxTrials()
 */
void TransferManager::setNbMaxTrials(int nbTrials)
{
    Impl::Locker locker(d_ptr->m_mutex);

    nbTrials = std::max(0, nbTrials);
    d_ptr->m_nbMaxTrials = nbTrials;
}

/*!
 * \brief Use to set the maximum time in seconds
 * that allow connection phase to take
 * \details
 * This timeout only limits the connection phase, it has
 * no impact once connection has been done. \n
 * The connection phase includes the name resolve (DNS)
 * and all protocol handshakes and negotiations until
 * there is an established connection with the remote side.
 *
 * \param[in] timeout
 * Timeout in seconds. \n
 * To disable it, use value <tt>0</tt>.
 * Default value is: \c 10
 *
 * \note
 * This method is \em thread-safe
 *
 * \sa getTimeoutConnection()
 */
void TransferManager::setTimeoutConnection(long timeout)
{
    Impl::Locker locker(d_ptr->m_mutex);

    timeout = std::max(0L, timeout);
    d_ptr->m_timeoutConnect = timeout;
}

/*!
 * \brief Use to set the maximum time in seconds
 * to wait when no data is received before considering
 * a timeout.
 * \details
 * This timeout is used when connection to host has been made,
 * it will check for average speed transfer. \n
 * If transfer speed is below \c 20 bytes/sec for
 * \c timeout time, request is aborted (and retried if available).
 *
 * \param[in] timeout
 * Timeout in seconds. \n
 * To disable it, use value <tt>0</tt>.
 * Default value is: \c 10
 *
 * \note
 * This method is \em thread-safe
 *
 * \sa getTimeoutTransfer()
 */
void TransferManager::setTimeoutTransfer(long timeout)
{
    Impl::Locker locker(d_ptr->m_mutex);

    timeout = std::max(0L, timeout);
    d_ptr->m_timeoutTransfer = timeout;
}

/*!
 * \brief Use to set options of the transfer manager
 *
 * \param[in] options
 * Option flag(s) to use. \n
 * Default value is: \c TransferManager::OPT_NONE
 *
 * \note
 * This method is \em thread-safe
 *
 * \sa getOptions()
 */
void TransferManager::setOptions(FlagOption options)
{
    Impl::Locker locker(d_ptr->m_mutex);
    d_ptr->m_options = options;
}

/*!
 * \brief Use to set started transfer callback
 * \details
 * Default callback will simply log a message. \n
 * See TransferManager documentation for more details
 * on how to set the callback.
 *
 * \param[in] fct
 * Callback function to use when transfer is started
 *
 * \note
 * This method is \em thread-safe
 */
void TransferManager::setCbStarted(CbStarted fct)
{
    Impl::Locker locker(d_ptr->m_mutex);
    d_ptr->m_cbStarted = fct;
}

/*!
 * \brief Use to set progress transfer callback
 * \details
 * Default callback will simply log a message. \n
 * See TransferManager documentation for more details
 * on how to set the callback.
 *
 * \param[in] fct
 * Callback function to use for transfer progress
 *
 * \note
 * This method is \em thread-safe
 */
void TransferManager::setCbProgress(CbProgress fct)
{
    Impl::Locker locker(d_ptr->m_mutex);
    d_ptr->m_cbProgress = fct;
}

/*!
 * \brief Use to set completed transfer callback
 * \details
 * Default callback will simply log a message. \n
 * See TransferManager documentation for more details
 * on how to set the callback.
 *
 * \param[in] fct
 * Callback function to use when transfer is completed
 *
 * \note
 * This method is \em thread-safe
 */
void TransferManager::setCbCompleted(CbCompleted fct)
{
    Impl::Locker locker(d_ptr->m_mutex);
    d_ptr->m_cbCompleted = fct;
}

/*!
 * \brief Use to set failed transfer callback
 * \details
 * Default callback will simply log a message. \n
 * See TransferManager documentation for more details
 * on how to set the callback.
 *
 * \param[in] fct
 * Callback function to use when transfer has failed
 *
 * \note
 * This method is \em thread-safe
 */
void TransferManager::setCbFailed(CbFailed fct)
{
    Impl::Locker locker(d_ptr->m_mutex);
    d_ptr->m_cbFailed = fct;
}

/*!
 * \brief Use to convert progress data to a percentage
 *
 * \param[in] transferTotal
 * Total size of the transfer
 * \param[in] transferNow
 * Current size of transferred datas
 *
 * \return
 * Return progress percentage.
 */
double TransferManager::transferProgressToPercent(size_t transferTotal, size_t transferNow)
{
    return (static_cast<double>(transferNow) / transferTotal) * 100.0;
}

/*!
 * \brief Use to convert flags option to a string
 *
 * \param[in] options
 * Options to convert to string
 * \param[in] separator
 * Character separator to use between each options
 *
 * \return
 * Returns string of options. \n
 * Example:
 * \code{.cpp}
 * const FlagOption options = (TransferManager::OPT_FTP_CREATE_DIRS | TransferManager::OPT_NOT_YET_IMPL)
 * std::cout << TransferManager::flagOptionToStr(options, '|'); // Will display: "OPT_FTP_CREATE_DIRS|OPT_NOT_YET_IMPL"
 * \endcode
 */
std::string TransferManager::flagOptionToStr(FlagOption options, char separator)
{
    /* Define string equivalent only once */
    static const std::unordered_map<FlagOption, std::string> MAP_FLAG_OPT_TO_STR =
    {
        {FlagOption::OPT_NONE, "OPT_NONE"},
        {FlagOption::OPT_NONE, "OPT_FTP_CREATE_DIRS"}
    };

    /* Convert flags to string */
    return flagEnumToString(options, MAP_FLAG_OPT_TO_STR, separator);
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

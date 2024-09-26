#ifndef TEASE_TRANSFERMANAGER_H
#define TEASE_TRANSFERMANAGER_H

#include "transferease_global.h"
#include "net/request.h"

#include <functional>

namespace tease
{

class TEASE_EXPORT TransferManager final
{
    TEASE_DISABLE_COPY_MOVE(TransferManager)

public:
    /*!
     * \brief List of errors identifiers
     */
    enum IdError
    {
        ERR_NO_ERROR = 0,       /**< Sucess return code, no error detected */

        ERR_INTERNAL,           /**< Internal error mainly due to underlying library, please refer to logs if this error is triggered */
        ERR_INVALID_LOGIN,      /**< Login informations used where wrong */
        ERR_INVALID_REQUEST,    /**< Receive an invalid request : can be an unsupported protocol and a misformatted request */
        ERR_INVALID_SSL,        /**< Provided SSL informations are invalid */
        ERR_BUSY,               /**< Manager is already performing requests transfers */
        ERR_USER_ABORT,         /**< User abort current transfer */
        ERR_MAX_TRIALS,         /**< Maximum number of trials were reached */
        ERR_MEMORY_FULL_HOST,   /**< Trying to download a ressource to host which have his memory full */
        ERR_MEMORY_FULL_REMOTE, /**< Trying to upload a ressource to remote which have his memory full */
        ERR_HOST_NOT_FOUND,     /**< Host server informations are either invalid or unreachable */
        ERR_HOST_REFUSED,       /**< Host server refused connection */
        ERR_CONTENT_NOT_FOUND   /**< Ressource could not be found */
    };

public:
    using CbStarted = std::function<void(Request::TypeTransfer typeTransfer)>;
    using CbProgress = std::function<void(Request::TypeTransfer typeTransfer, size_t transferTotal, size_t transferNow)>;
    using CbCompleted = std::function<void(Request::TypeTransfer typeTransfer)>;
    using CbFailed = std::function<void(Request::TypeTransfer typeTransfer, IdError idErr)>;

public:
    TransferManager();
    ~TransferManager();

public:
    IdError startDownload(const Request::List &listReqs);
    IdError startUpload(const Request::List &listReqs);
    bool transferIsInProgress() const;

    int getNbMaxTrials() const;
    long getTimeoutConnection() const;
    long getTimeoutTransfer() const;

public:
    void setUserInfos(const std::string &username, const std::string &passwd);
    void setNbMaxTrials(int nbTrials);
    void setTimeoutConnection(long timeout);
    void setTimeoutTransfer(long timeout);

public:
    void setCbStarted(CbStarted fct);
    void setCbProgress(CbProgress fct);
    void setCbCompleted(CbCompleted fct);
    void setCbFailed(CbFailed fct);

public:
    static double transferProgressToPercent(size_t transferTotal, size_t transferNow);

private:
    class Impl;
    std::unique_ptr<Impl> d_ptr;
};

} // namespace tease

#endif // TEASE_TRANSFERMANAGER_H

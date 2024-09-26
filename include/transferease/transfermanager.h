#ifndef TEASE_TRANSFERMANAGER_H
#define TEASE_TRANSFERMANAGER_H

#include "transferease_global.h"
#include "net/request.h"
#include "tools/enumflag.h"

#include <functional>

/*****************************/
/* Namespace instructions    */
/*****************************/
namespace tease
{

/*****************************/
/* Class definitions         */
/*****************************/
class TEASE_EXPORT TransferManager final
{
    TEASE_DISABLE_COPY_MOVE(TransferManager)

public:
    /*!
     * \brief List of errors identifiers
     */
    enum IdError
    {
        ERR_NO_ERROR = 0,       /**< Success return code, no error detected */

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

    /*!
     * \brief List of available options
     *
     * \sa setOptions()
     * \sa flagOptionToStr()
     */
    enum FlagOption : std::uint32_t
    {
        OPT_NONE = 0,                   /**< No options defined, use this value to reset flags */

        OPT_FTP_CREATE_DIRS = 1 << 0    /**< When uploading ressource via FTP protocol, missing directories will be automatically created. \n Note that this option will be ignored for any other protocol. */
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
    FlagOption getOptions() const;

public:
    void setUserInfos(const std::string &username, const std::string &passwd);
    void setNbMaxTrials(int nbTrials);
    void setTimeoutConnection(long timeout);
    void setTimeoutTransfer(long timeout);
    void setOptions(FlagOption options);

public:
    void setCbStarted(CbStarted fct);
    void setCbProgress(CbProgress fct);
    void setCbCompleted(CbCompleted fct);
    void setCbFailed(CbFailed fct);

public:
    static double transferProgressToPercent(size_t transferTotal, size_t transferNow);
    static std::string flagOptionToStr(FlagOption options, char separator = '|');

private:
    class Impl;
    std::unique_ptr<Impl> d_ptr;
};

/*****************************/
/* End namespaces            */
/*****************************/

} // namespace tease

/*****************************/
/* Enable flag enum options  */
/*****************************/

/*!
 * \cond INTERNAL
 */

template<>
struct enable_bitmask_operators<tease::TransferManager::FlagOption>{
    static constexpr bool enable = true;
};

/*!
 * \endcond
 */

/*****************************/
/* End file                  */
/*****************************/

#endif // TEASE_TRANSFERMANAGER_H

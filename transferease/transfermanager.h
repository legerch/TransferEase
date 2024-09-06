#ifndef TEASE_TRANSFERMANAGER_H
#define TEASE_TRANSFERMANAGER_H

#include "transferease_global.h"
#include "net/request.h"

#include <functional>

namespace tease
{

class TransferManager final
{
    TEASE_DISABLE_COPY_MOVE(TransferManager)

public:
    enum IdError
    {
        ERR_NO_ERROR = 0,

        ERR_INTERNAL,
        ERR_INVALID_LOGIN,
        ERR_INVALID_REQUEST,
        ERR_INVALID_SSL,
        ERR_BUSY,
        ERR_USER_ABORT,
        ERR_MAX_TRIALS,
        ERR_MEMORY_FULL_HOST,
        ERR_MEMORY_FULL_REMOTE,
        ERR_HOST_NOT_FOUND,
        ERR_HOST_REFUSED,
        ERR_CONTENT_NOT_FOUND
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
    bool transferIsInProgress() const;

public:
    void setUserInfos(const std::string &username, const std::string &passwd);
    void setNbMaxTrials(int nbTrials);

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

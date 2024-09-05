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
        ERR_USER_ABORT
    };

public:
    using CbStarted = std::function<void(Request::TypeTransfer typeTransfer)>;
    using CbProgress = std::function<void(Request::TypeTransfer typeTransfer, size_t transferTotal, size_t transferNow)>;
    using CbCompleted = std::function<void(Request::TypeTransfer typeTransfer)>;
    using CbFailed = std::function<void(Request::TypeTransfer typeTransfer)>;

public:
    TransferManager();
    ~TransferManager();

public:
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

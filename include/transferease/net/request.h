#ifndef TEASE_NET_REQUEST_H
#define TEASE_NET_REQUEST_H

#include "bytesarray.h"
#include "url.h"

namespace tease
{

class TEASE_EXPORT Request
{
    TEASE_DISABLE_COPY_MOVE(Request)

public:
    /*!
     * \brief List of types of transfers
     */
    enum TypeTransfer
    {
        TRANSFER_UNK = 0,   /**< Unknown type of transfer, mainly used to represent an unitialized transfer */

        TRANSFER_DOWNLOAD,  /**< Ressources are downloaded from \b remote to \b host */
        TRANSFER_UPLOAD     /**< Ressources are uploaded from \b host to \b remote */
    };

public:
    using PtrShared = std::shared_ptr<Request>; /**< Request shared pointer type alias */
    using List = std::vector<PtrShared>;        /**< Alias representing a list of requests */

public:
    Request();
    virtual ~Request();

public:
    void clear();

    void configureDownload(const Url &targetUrl);

    void configureUpload(const Url &dstUrl, const BytesArray &inputData);
    void configureUpload(const Url &dstUrl, BytesArray &&inputData);

public:
    TypeTransfer getTypeTransfer() const;
    const Url& getUrl() const;

    BytesArray& getData();
    const BytesArray& getData() const;

public:
    size_t ioRead(char *buffer, size_t nbBytes);

    void ioSetSizeTotal(size_t size);
    void ioSetSizeCurrent(size_t size);
    void ioRegisterTry();
    void ioReset();

    size_t ioGetSizeTotal() const;
    size_t ioGetSizeCurrent() const;
    int ioGetNbTrials() const;

private:
    class Impl;
    std::unique_ptr<Impl> d_ptr;
};

} // namespace tease

#endif // TEASE_NET_REQUEST_H

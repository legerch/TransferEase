#ifndef TEASE_NET_REQUEST_H
#define TEASE_NET_REQUEST_H

#include "net/bytesarray.h"
#include "net/url.h"

namespace tease
{

class Request final
{

public:
    enum TypeTransfer
    {
        TRANSFER_UNK = 0,

        TRANSFER_DOWNLOAD,
        TRANSFER_UPLOAD
    };

public:
    using PtrShared = std::shared_ptr<Request>;
    using List = std::vector<PtrShared>;

public:
    Request();
    ~Request();

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
    void ioReset();

private:
    class Impl;
    std::unique_ptr<Impl> d_ptr;
};

} // namespace tease

#endif // TEASE_NET_REQUEST_H

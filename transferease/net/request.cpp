#include "request.h"

#include <cstring>

/*****************************/
/* Class documentations      */
/*****************************/

/*!
 * \class tease::Request
 * \brief Do doc
 */

/*****************************/
/* Macro definitions         */
/*****************************/

/*****************************/
/* Start namespace           */
/*****************************/

namespace tease
{

/*****************************/
/* Functions definitions     */
/*      Private Class        */
/*****************************/

class Request::Impl final
{
public:
    void configureTransfer(TypeTransfer idType, const Url &url);

public:
    size_t ioReadFromBytesArray(char *buffer, size_t nbBytes);
    void ioReset();

    void clear();

public:
    TypeTransfer m_idType;

    Url m_url;

    BytesArray m_data;
    size_t m_dataNbRead;

    size_t m_ioTotal;
    size_t m_ioCurrent;
    int m_ioNbTrials;
};

/*****************************/
/* Functions implementation  */
/*      Private Class        */
/*****************************/

void Request::Impl::configureTransfer(TypeTransfer idType, const Url &url)
{
    /* Reset any IO interactions */
    ioReset();

    /* Set common properties */
    m_idType = idType;
    m_url = url;
}

size_t Request::Impl::ioReadFromBytesArray(char *buffer, size_t nbBytes)
{
    const size_t nbBytesRemaining = m_data.getSize() - m_dataNbRead;
    const size_t nbBytesToRead = std::min(nbBytes, nbBytesRemaining);

    std::memcpy(buffer, m_data.dataConst() + m_dataNbRead, nbBytesToRead);
    m_dataNbRead += nbBytesToRead;

    return nbBytesToRead;
}

void Request::Impl::ioReset()
{
    m_dataNbRead = 0;

    m_ioTotal = 0;
    m_ioCurrent = 0;
    m_ioNbTrials = 0;
}

void Request::Impl::clear()
{
    m_idType = TRANSFER_UNK;

    m_url.clear();
    m_data.clear();

    ioReset();
}

/*****************************/
/* Functions implementation  */
/*      Public Class         */
/*****************************/

Request::Request() :
    d_ptr(std::make_unique<Impl>())
{
    clear();
}

Request::~Request() = default;

void Request::clear()
{
    d_ptr->clear();
}

void Request::configureDownload(const Url &targetUrl)
{
    d_ptr->configureTransfer(TRANSFER_DOWNLOAD, targetUrl);
    d_ptr->m_data.clear();
}

void Request::configureUpload(const Url &dstUrl, const BytesArray &inputData)
{
    d_ptr->configureTransfer(TRANSFER_UPLOAD, dstUrl);
    d_ptr->m_data = inputData;
}

void Request::configureUpload(const Url &dstUrl, BytesArray &&inputData)
{
    d_ptr->configureTransfer(TRANSFER_UPLOAD, dstUrl);
    d_ptr->m_data = std::move(inputData);
}

Request::TypeTransfer Request::getTypeTransfer() const
{
    return d_ptr->m_idType;
}

const Url& Request::getUrl() const
{
    return d_ptr->m_url;
}

BytesArray& Request::getData()
{
    return d_ptr->m_data;
}

const BytesArray& Request::getData() const
{
    return d_ptr->m_data;
}

size_t Request::ioRead(char *buffer, size_t nbBytes)
{
    return d_ptr->ioReadFromBytesArray(buffer, nbBytes);
}

void Request::ioSetSizeTotal(size_t size)
{
    d_ptr->m_ioTotal = size;
}

void Request::ioSetSizeCurrent(size_t size)
{
    d_ptr->m_ioCurrent = size;
}

void Request::ioRegisterTry()
{
    ++d_ptr->m_ioNbTrials;
}

void Request::ioReset()
{
    d_ptr->ioReset();
}

size_t Request::ioGetSizeTotal() const
{
    return d_ptr->m_ioTotal;
}

size_t Request::ioGetSizeCurrent() const
{
    return d_ptr->m_ioCurrent;
}

int Request::ioGetNbTrials() const
{
    return d_ptr->m_ioNbTrials;
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

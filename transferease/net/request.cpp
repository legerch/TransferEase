#include "request.h"

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
    TypeTransfer m_idType;

    Url m_url;
    BytesArray m_data;
};

/*****************************/
/* Functions implementation  */
/*      Private Class        */
/*****************************/

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
    d_ptr->m_idType = TRANSFER_UNK;

    d_ptr->m_url.clear();
    d_ptr->m_data.clear();
}

void Request::configureDownload(const Url &targetUrl)
{
    d_ptr->m_idType = TRANSFER_DOWNLOAD;

    d_ptr->m_url = targetUrl;
    d_ptr->m_data.clear();
}

void Request::configureUpload(const Url &dstUrl, const BytesArray &inputData)
{
    d_ptr->m_idType = TRANSFER_UPLOAD;

    d_ptr->m_url = dstUrl;
    d_ptr->m_data = inputData;
}

void Request::configureUpload(const Url &dstUrl, BytesArray &&inputData)
{
    d_ptr->m_idType = TRANSFER_UPLOAD;

    d_ptr->m_url = dstUrl;
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

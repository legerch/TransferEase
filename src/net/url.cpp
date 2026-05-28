#include "transferease/net/url.h"

#include "transferease/logs/abstractlogger.h"
#include "tools/stringhelper.h"

#include <algorithm>
#include <unordered_map>

#include <curl/urlapi.h>

/*****************************/
/* Class documentations      */
/*****************************/

/*!
 * \class tease::Url
 * \brief Use to manage URLs
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

class Url::Impl final
{

public:
    explicit Impl(Url *parent);
    ~Impl();

    Impl(const Impl &other);
    Impl& operator=(const Impl &other);

    Impl(Impl &&other) noexcept;
    Impl& operator=(Impl &&other) noexcept;

public:
    void setUrl(CURLU *url);
    bool parseUrl(const std::string &url);

    std::string getPart(CURLUPart idPart, uint flags) const;

public:
    CURLU *m_url = nullptr;
    Url *m_parent = nullptr;

public:
    /*!
     * \brief Map containing scheme ID string equivalent
     * \note
     * Since we also search by value, we could have used a bimap to reduce complexity,
     * but since we will have less than 10 values in it, looking by value will be fine.
     */
    static const std::unordered_map<IdScheme, std::string> MAP_ID_SCHEME_TO_STRING;
};

/*****************************/
/* Functions implementation  */
/*      Private Class        */
/*****************************/

Url::Impl::Impl(Url *parent)
{
    m_url = curl_url();
    m_parent = parent;
}

Url::Impl::~Impl()
{
    setUrl(nullptr);
    m_parent = nullptr;
}

Url::Impl::Impl(const Url::Impl &other)
{
    if(other.m_url){
        setUrl(curl_url_dup(other.m_url));
    }
}

Url::Impl &Url::Impl::operator=(const Impl &other)
{
    /* Verify that entity is not already the same */
    if(this == &other){
        return *this;
    }

    /* Copy members */
    if(other.m_url){
        setUrl(curl_url_dup(other.m_url));
    }

    return *this;
}

Url::Impl::Impl(Impl &&other) noexcept
    : m_url(other.m_url)
{
    other.m_url = nullptr;
}

Url::Impl &Url::Impl::operator=(Impl &&other) noexcept
{
    /* Verify that entity is not already the same */
    if(this == &other){
        return *this;
    }

    /* Move members */
    setUrl(other.m_url);
    other.m_url = nullptr;

    return *this;
}

void Url::Impl::setUrl(CURLU *url)
{
    /* Verify that URL do not already exists */
    if(m_url){
        curl_url_cleanup(m_url);
    }

    /* Set URL */
    m_url = url;
}

bool Url::Impl::parseUrl(const std::string &url)
{
    /* Set URL from string */
    const CURLUcode idErr = curl_url_set(m_url, CURLUPART_URL, url.c_str(), CURLU_URLENCODE);
    if(idErr != CURLUE_OK){
        const std::string err = StringHelper::format("Unable to set URL from string [url: '%s', id-err: %d]", url.c_str(), idErr);
        TEASE_LOG_ERROR(err);
        return false;
    }

    /* Verify that scheme is supported */
    const IdScheme idScheme = m_parent->getIdScheme();
    if(idScheme == SCHEME_UNK){
        return false;
    }

    return true;
}

std::string Url::Impl::getPart(CURLUPart idPart, uint flags) const
{
    /* Retrieve URL needed part */
    char *string = nullptr;
    const CURLUcode idErr = curl_url_get(m_url, idPart, &string, flags);
    if(idErr != CURLUE_OK){
        return std::string();
    }

    /* Convert string and clean ressources */
    const std::string part(string);
    curl_free(string);

    return part;
}

/*****************************/
/* Functions implementation  */
/*      Public Class         */
/*****************************/

/*!
 * \brief Build a empty URL which will be invalid
 *
 * \sa isValid()
 */
Url::Url() :
    d_ptr(std::make_unique<Impl>(this))
{
    clear();
}

/*!
 * \brief Create an URL from an already encoded string
 *
 * \param[in] url
 * URL to set, it must be already properly encoded (use
 * \c setPath() as alternative to encode an URL path). \n
 * If invalid or protocol unsupported, URL will be
 * cleared.
 *
 * \sa isValid(), setPath()
 * \sa clear()
 */
Url::Url(const std::string &url) :
    d_ptr(std::make_unique<Impl>(this))
{
    setUrl(url);
}

Url::Url(const Url &other) :
    d_ptr(std::make_unique<Impl>(*other.d_ptr))
{
    d_ptr->m_parent = this;
}

Url& Url::operator=(const Url &other)
{
    /* Verify that value actually differs */
    if(this == &other){
        return *this;
    }

    /* Perform copy assignment */
    d_ptr = std::make_unique<Impl>(*other.d_ptr);
    d_ptr->m_parent = this; // Be sure that proper parent stay

    return *this;
}

Url::Url(Url &&other) noexcept :
    d_ptr(std::move(other.d_ptr))
{
    d_ptr->m_parent = this;
};

Url& Url::operator=(Url &&other) noexcept
{
    d_ptr = std::move(other.d_ptr);
    d_ptr->m_parent = this; // Be sure that proper parent stay

    return *this;
}

Url::~Url() = default;

/*!
 * \brief Use to reset an URL
 *
 * \sa isValid()
 */
void Url::clear()
{
    d_ptr->setUrl(curl_url());
}

/*!
 * \brief Use to set URL from a string
 * \details
 * This method will parse the provided string
 * to URL format. Examples:
 * \code{.cpp}
 * url1.setUrl("https://example.com:8080/path/to/resource.zip");
 * url2.setUrl("https://example.com/path/to/resource.zip");
 * \endcode
 * Properties of those URLs will be:
 * - IdScheme: `Url::SCHEME_HTTPS`
 * - Host: `example.com`
 * - Port: \c 8080 for `url1` and `0` for `url2` (\c 0 simply means that we don't provide port info)
 * - Path: `path/to/resource.zip`
 *
 * \param[in] url
 * URL to parse. \n
 * If invalid or protocol unsupported, URL will be
 * cleared.
 */
void Url::setUrl(const std::string &url)
{
    bool succeed = d_ptr->parseUrl(url);
    if(!succeed){
        clear();
    }
}

void Url::setIdScheme(IdScheme idScheme)
{
    const std::string scheme = idSchemeToString(idScheme);
    curl_url_set(d_ptr->m_url, CURLUPART_SCHEME, scheme.c_str(), 0);
}

void Url::setHost(const std::string &host)
{
    curl_url_set(d_ptr->m_url, CURLUPART_HOST, host.c_str(), 0);
}

void Url::setPort(uint16_t port)
{
    /* Do we need to disable explicit port info ? */
    if(port == 0){
        curl_url_set(d_ptr->m_url, CURLUPART_PORT, nullptr, 0);
        return;
    }

    /* Set port to use */
    const std::string portStr = std::to_string(port);
    curl_url_set(d_ptr->m_url, CURLUPART_PORT, portStr.c_str(), 0);
}

void Url::setPath(const std::string &path)
{
    curl_url_set(d_ptr->m_url, CURLUPART_PATH, path.c_str(), CURLU_URLENCODE);
}

/*!
 * \brief Use to know if URL is valid
 * \details
 * URL is considered valid if scheme is supported
 * and host is set.
 *
 * \return
 * Returns \c true if URL is valid
 */
bool Url::isValid() const
{
    /* Verify scheme validity */
    const IdScheme idScheme = getIdScheme();
    if(idScheme <= SCHEME_UNK || idScheme >= SCHEME_NB_SUPPORTED){
        return false;
    }

    /* Verify fields validity */
    return !getHost().empty();
}

/*!
 * \brief Use to generate properly formatted URL
 *
 * \return
 * Returns formatted URL. \n
 * Returned value can be empty if URL is invalid.
 *
 * \sa isValid()
 */
std::string Url::toString() const
{
    return d_ptr->getPart(CURLUPART_URL, 0);
}

Url::IdScheme Url::getIdScheme() const
{
    const std::string scheme = d_ptr->getPart(CURLUPART_SCHEME, 0);
    return idSchemeFromString(scheme);
}

const std::string Url::getHost() const
{
    return d_ptr->getPart(CURLUPART_HOST, 0);
}

uint16_t Url::getPort() const
{
    const std::string portStr = d_ptr->getPart(CURLUPART_PORT, 0);
    return StringHelper::toInt(portStr, 10);
}

const std::string Url::getPath() const
{
    return d_ptr->getPart(CURLUPART_PATH, CURLU_URLDECODE);
}

std::string Url::idSchemeToString(IdScheme idScheme)
{
    /* Verify that ID is valid */
    if(idScheme <= SCHEME_UNK || idScheme >= SCHEME_NB_SUPPORTED){
        return "unknown";
    }

    /* Return associated string */
    return Url::Impl::MAP_ID_SCHEME_TO_STRING.at(idScheme);
}

Url::IdScheme Url::idSchemeFromString(const std::string &idScheme)
{
    /* Search scheme by string value */
    const std::string scheme = StringHelper::toLower(idScheme);
    auto it = std::find_if(Url::Impl::MAP_ID_SCHEME_TO_STRING.cbegin(), Url::Impl::MAP_ID_SCHEME_TO_STRING.cend(), [scheme](const auto &pair){
        return pair.second == scheme;
    });

    /* Do we have found a scheme */
    if(it == Url::Impl::MAP_ID_SCHEME_TO_STRING.cend()){
        const std::string err = StringHelper::format("No supported scheme ID match string '%s'", idScheme.c_str());
        TEASE_LOG_WARN(err);
        return SCHEME_UNK;
    }

    /* Return matching scheme ID */
    return it->first;
}

bool operator==(const Url &left, const Url &right)
{
    return left.toString() == right.toString();
}

bool operator!=(const Url &left, const Url &right)
{
    return !(left == right);
}

/*****************************/
/* Constants definitions     */
/*****************************/
const std::unordered_map<Url::IdScheme, std::string> Url::Impl::MAP_ID_SCHEME_TO_STRING =
{
    {SCHEME_FTP, "ftp"},
    {SCHEME_FTPS, "ftps"},
    {SCHEME_HTTP, "http"},
    {SCHEME_HTTPS, "https"}
};

/*****************************/
/* End namespace             */
/*****************************/

} // namespace tease

/*****************************/
/* End file                  */
/*****************************/

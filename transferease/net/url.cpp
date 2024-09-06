#include "url.h"

#include <regex>
#include <unordered_map>

#include "logs/logmanager.h"
#include "tools/stringhelper.h"

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

public:
    bool parseUrl(const std::string &url);

public:
    IdScheme m_idScheme;
    std::string m_host;
    uint16_t m_port;
    std::string m_path;

    Url *m_parent;

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
    m_parent = parent;
}

/*!
 * \brief Use to parse URL
 * \details
 * Regex used was defined this way:
 * - <tt>(\w+)</tt>: Matches the scheme (e.g., http, https)
 * - <tt>:\/\/</tt>: Matches the literal string "://"
 * - <tt>([^\/:]+))</tt>: Matches the host (e.g., example.com), which consists of characters other than \c / and <tt>:</tt>.
 * - <tt>(?::(\d+))?</tt>: Optionally matches the port, capturing a sequence of digits following a <tt>:</tt>. \n
 * The \c ? makes the port optional.
 * - <tt>(\/.*)?</tt>: Optionally matches the path, capturing everything following a \c /
 *
 * \param[in] url
 * URL to parse.
 *
 * \return
 * Returns \c true if succeed to parse.
 */
bool Url::Impl::parseUrl(const std::string &url)
{
    /* Define URI regex parser */
    const std::regex uriRegex(R"(^(\w+):\/\/([^\/:]+)(?::(\d+))?(\/.*)?$)");
    static constexpr int expMatches = 5; // 1 for full match, 4 for submatches

    /* Verify regex matches */
    std::smatch matchesRes;
    bool hasMatches = std::regex_match(url, matchesRes, uriRegex);
    if(!hasMatches){
        const std::string err = StringHelper::format("No matches found when trying to parse URL [url: '%s']", url.c_str());
        TEASE_LOG_ERROR(err);
        return false;
    }

    /* Do we have enough fields ? */
    if(matchesRes.size() < expMatches){
        const std::string err = StringHelper::format("Not enough fields inside URL [url: '%s']", url.c_str());
        TEASE_LOG_ERROR(err);
        return false;
    }

    /* Fill URL properties */
    // Mandatory fields
    const IdScheme idScheme = Url::idSchemeFromString(matchesRes[1].str());
    if(idScheme == SCHEME_UNK){
        return false;
    }

    m_idScheme = idScheme;
    m_host = matchesRes[2].str();
    m_path = matchesRes[4].str();

    // Optional fields
    if(matchesRes[3].matched){
        m_port = StringHelper::toInt(matchesRes[3].str());
    }else{
        m_port = 0;
    }

    return m_parent->isValid();
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
 * \brief Create an URL from a string which will
 * be parsed
 *
 * \param[in] url
 * URL to parse. \n
 * If invalid or protocol unsupported, URL will be
 * cleared.
 *
 * \sa isValid(), setUrl()
 * \sa clear()
 */
Url::Url(const std::string &url) :
    d_ptr(std::make_unique<Impl>(this))
{
    setUrl(url);
}

Url::Url(const Url &other) :
    d_ptr(std::make_unique<Impl>(*other.d_ptr)){}

Url::Url(Url &&other) noexcept :
    d_ptr(std::move(other.d_ptr)){};

Url::~Url() = default;

/*!
 * \brief Use to reset an URL
 *
 * \sa isValid()
 */
void Url::clear()
{
    d_ptr->m_idScheme = SCHEME_UNK;
    d_ptr->m_host.clear();
    d_ptr->m_port = 0;
    d_ptr->m_path.clear();
}

/*!
 * \brief Url::setUrl
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
    d_ptr->m_idScheme = idScheme;
}

void Url::setHost(const std::string &host)
{
    d_ptr->m_host = host;
}

void Url::setPort(uint16_t port)
{
    d_ptr->m_port = port;
}

void Url::setPath(const std::string &path)
{
    d_ptr->m_path = path;
}

/*!
 * \brief Use to know if URL is valid
 * \details
 * URL is considered valid if scheme is supported
 * and associated fields are set.
 *
 * \return
 * Returns \c true if URL is valid
 */
bool Url::isValid() const
{
    /* Verify scheme validity */
    if(d_ptr->m_idScheme <= SCHEME_UNK || d_ptr->m_idScheme >= SCHEME_NB_SUPPORTED){
        return false;
    }

    /* Verify fields validity */
    if(d_ptr->m_host.empty() || d_ptr->m_path.empty()){
        return false;
    }

    return true;
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
    /* Verify that URL is valid */
    if(!isValid()){
        return std::string();
    }

    /* Create URL */
    std::string url = idSchemeToString(d_ptr->m_idScheme) + "://" + d_ptr->m_host;

    /* Do we have port information ? */
    if(d_ptr->m_port != 0){
        url += ":" + std::to_string(d_ptr->m_port);
    }

    /* Add path information */
    url += d_ptr->m_path;

    return url;
}

Url::IdScheme Url::getIdScheme() const
{
    return d_ptr->m_idScheme;
}

const std::string& Url::getHost() const
{
    return d_ptr->m_host;
}

uint16_t Url::getPort() const
{
    return d_ptr->m_port;
}

const std::string& Url::getPath() const
{
    return d_ptr->m_path;
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
        const std::string err = StringHelper::format("No supported scheme ID match string '%s'", idScheme);
        TEASE_LOG_WARN(err);
        return SCHEME_UNK;
    }

    /* Return matching scheme ID */
    return it->first;
}

Url& Url::operator=(const Url &other)
{
    /* Verify that value actually differs */
    if(this == &other){
        return *this;
    }

    /* Perform copy assignment */
    d_ptr = std::make_unique<Impl>(*other.d_ptr);
    return *this;
}

Url& Url::operator=(Url &&other) noexcept
{
    d_ptr = std::move(other.d_ptr);
    return *this;
}

bool operator==(const Url &left, const Url &right)
{
    return left.d_ptr->m_idScheme == right.d_ptr->m_idScheme
        && left.d_ptr->m_host == right.d_ptr->m_host
        && left.d_ptr->m_port == right.d_ptr->m_port
        && left.d_ptr->m_path == right.d_ptr->m_path;
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

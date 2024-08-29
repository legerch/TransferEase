#include "url.h"

#include <regex>

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

class Url::UrlImpl final
{

public:
    bool parseUrl(const std::string &url);

public:
    IdScheme m_idScheme;
    std::string m_host;
    uint16_t m_port;
    std::string m_path;

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

/*!
 * \brief Use to parse URL
 * \details
 * Regex used was defined this way:
 * - <tt>(\w+)</tt>: Matches the scheme (e.g., http, https)
 * - <tt>://</tt>: Matches the literal string "://"
 * - <tt>([^/:]+)</tt>: Matches the host (e.g., example.com), which consists of characters other than \c / and <tt>:</tt>.
 * - <tt>(?::(\d+))?</tt>: Optionally matches the port, capturing a sequence of digits following a <tt>:</tt>. \n
 * The \c ? makes the port optional.
 * - <tt>(/.*)?</tt>: Optionally matches the path, capturing everything following a \c /
 *
 * \param[in] url
 * URL to parse.
 *
 * \return
 * Returns \c true if succeed to parse.
 */
bool Url::UrlImpl::parseUrl(const std::string &url)
{
    /* Define URI regex parser */
    static const std::regex uriRegex(R"(^(\w+)://([^/:]+)(?::(\d+))?(/.*)?$)", std::regex::extended);
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

    return true;
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
    d_ptr(std::make_unique<UrlImpl>())
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
    d_ptr(std::make_unique<UrlImpl>())
{
    setUrl(url);
}

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

std::string Url::getUrl() const
{
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
    return UrlImpl::MAP_ID_SCHEME_TO_STRING.at(idScheme);
}

Url::IdScheme Url::idSchemeFromString(const std::string &idScheme)
{
    /* Search scheme by string value */
    const std::string scheme = StringHelper::toLower(idScheme);
    auto it = std::find_if(UrlImpl::MAP_ID_SCHEME_TO_STRING.cbegin(), UrlImpl::MAP_ID_SCHEME_TO_STRING.cend(), [scheme](const auto &pair){
        return pair.second == scheme;
    });

    /* Do we have found a scheme */
    if(it == UrlImpl::MAP_ID_SCHEME_TO_STRING.cend()){
        const std::string err = StringHelper::format("No supported scheme ID match string '%s'", idScheme);
        TEASE_LOG_WARN(err);
        return SCHEME_UNK;
    }

    /* Return matching scheme ID */
    return it->first;
}

/*****************************/
/* Constants definitions     */
/*****************************/
const std::unordered_map<Url::IdScheme, std::string> Url::UrlImpl::MAP_ID_SCHEME_TO_STRING =
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

#ifndef TEASE_NET_URL_H
#define TEASE_NET_URL_H

#include "transferease/transferease_global.h"

#include <memory>
#include <string>

namespace tease
{

class TEASE_EXPORT Url
{

public:
    /*!
     * \brief List of supported schemes
     *
     * \sa idSchemeToString(), idSchemeFromString()
     */
    enum IdScheme
    {
        SCHEME_UNK = 0, /**< Unknown/unsupported protocol */

        SCHEME_FTP,     /**< Plain, unencrypted FTP that defaults over port 21 */
        SCHEME_FTPS,    /**< Implicit SSL/TLS encrypted FTP that works just like HTTPS. \n
                             Security is enabled with SSL as soon as the connection starts. \n
                             The default FTPS port is 990. This protocol was the first version
                             of encrypted FTP available, and while considered deprecated, is
                             still widely used */

        SCHEME_HTTP,    /**< Plain, unencrypted HTTP */
        SCHEME_HTTPS,   /**< HTTPS protocol which use SSL/TLS layer */

        SCHEME_NB_SUPPORTED /**< Number of protocol supported */
    };

public:
    Url();
    explicit Url(const std::string &url);

    Url(const Url &other);
    Url(Url &&other) noexcept;

    virtual ~Url();

public:
    void clear();
    void setUrl(const std::string &url);

    void setIdScheme(IdScheme idScheme);
    void setHost(const std::string &host);
    void setPort(uint16_t port);
    void setPath(const std::string &path);

public:
    bool isValid() const;

    std::string toString() const;

    IdScheme getIdScheme() const;
    const std::string& getHost() const;
    uint16_t getPort() const;
    const std::string& getPath() const;

public:
    static std::string idSchemeToString(IdScheme idScheme);
    static IdScheme idSchemeFromString(const std::string &idScheme);

public:
    Url& operator=(const Url &other);
    Url& operator=(Url &&other) noexcept;

public:
    TEASE_EXPORT friend bool operator==(const Url &left, const Url &right);
    TEASE_EXPORT friend bool operator!=(const Url &left, const Url &right);

private:
    class Impl;
    std::unique_ptr<Impl> d_ptr;
};

} // namespace tease

#endif // TEASE_NET_URL_H

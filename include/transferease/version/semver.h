#ifndef SEMVER_H
#define SEMVER_H

#include "transferease/transferease_global.h"

#include <memory>
#include <string>

/*****************************/
/* Namespace instructions    */
/*****************************/
namespace tease{

/*****************************/
/* Class definitions         */
/*****************************/
class TEASE_EXPORT Semver
{

public:
    /*!
     * \brief Field of semantic versionning
     */
    enum Field
    {
        SEMVER_MAJOR = 0,   /**< Major field, mainly used to represent version that \b break API/ABI compatibility */
        SEMVER_MINOR,       /**< Minor field, mainly used to represent new features or change behaviour that are backward compatible */
        SEMVER_PATCH,       /**< Patch field, mainly used to represent bug fixes */

        SEMVER_NB_FIELDS    /**< Enum value used to track number of available fields */
    };

public:
    Semver();
    Semver(int major, int minor, int patch);
    Semver(const std::string &semver, char delimiter);

    Semver(const Semver &other);
    Semver(Semver &&other) noexcept;

    ~Semver();

public:
    void setField(Field idField, int value);
    void clear();

public:
    int getField(Field idField) const;
    bool isValid() const;

public:
    std::string toString(char delimiter, int widthField) const;
    bool parseString(const std::string &version, char delimiter);

public:
    Semver& operator=(const Semver &other);
    Semver& operator=(Semver &&other) noexcept;

public:
    TEASE_EXPORT friend bool operator==(const Semver &left, const Semver &right);
    TEASE_EXPORT friend bool operator!=(const Semver &left, const Semver &right);
    TEASE_EXPORT friend bool operator<(const Semver &left, const Semver &right);
    TEASE_EXPORT friend bool operator<=(const Semver &left, const Semver &right);
    TEASE_EXPORT friend bool operator>(const Semver &left, const Semver &right);
    TEASE_EXPORT friend bool operator>=(const Semver &left, const Semver &right);

public:
    static const Semver& getLibraryVersion();

private:
    class Impl;
    std::unique_ptr<Impl> d_ptr;
};

/*****************************/
/* End namespaces            */
/*****************************/

} // namespace tease

/*****************************/
/* End file                  */
/*****************************/

#endif // SEMVER_H

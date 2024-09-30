#include "transferease/version/semver.h"

#include <numeric>

#include "tools/stringhelper.h"

/*****************************/
/* Class documentations      */
/*****************************/

/*!
 * \class tease::Semver
 * \brief Allow to manage semantic version
 * informations.
 * \details
 * This class follow principles of semantic
 * versionning, more details can be found at:
 * https://semver.org
 *
 * \sa getLibraryVersion()
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

class Semver::Impl final
{
public:
    Impl() = default;

public:
    void setField(Field idField, int value);
    bool parseString(const std::string &version, char delimiter);

public:
    std::array<int, SEMVER_NB_FIELDS> m_infos;
};

/*****************************/
/* Functions implementation  */
/*      Private Class        */
/*****************************/

void Semver::Impl::setField(Field idField, int value)
{
    m_infos[idField] = value;
}

bool Semver::Impl::parseString(const std::string &version, char delimiter)
{
    /* Try to parse string by delimiter */
    const std::vector<std::string> fieldsList = StringHelper::split(version, std::string(1, delimiter));
    if(fieldsList.size() < m_infos.size()){
        return false;
    }

    /* Convert version parsed infos */
    bool succeed = false;
    for(size_t i = 0; i < m_infos.size(); ++i){
        // Convert substring info to integer
        const int value = StringHelper::toInt(fieldsList[i], 10, &succeed);
        if(!succeed){
            return false;
        }

        // Set semver info
        setField(static_cast<Field>(i), value);
    }

    return true;
}

/*****************************/
/* Functions implementation  */
/*      Public Class         */
/*****************************/

/*!
 * \brief Default semver constructor
 * \details
 * All fields will be set to \c 0.
 *
 * \sa isValid()
 */
Semver::Semver()
    : Semver(0, 0, 0){}

/*!
 * \overload
 */
Semver::Semver(int major, int minor, int patch)
    : d_ptr(std::make_unique<Impl>())
{
    setField(SEMVER_MAJOR, major);
    setField(SEMVER_MINOR, minor);
    setField(SEMVER_PATCH, patch);
}

/*!
 * \overload
 * \brief Construct semver from a string
 * \details
 * See parseString() method for more details.
 */
Semver::Semver(const std::string &semver, char delimiter)
    : d_ptr(std::make_unique<Impl>())
{
    parseString(semver, delimiter);
}

/*!
 * \overload
 */
Semver::Semver(const Semver &other)
    : d_ptr(std::make_unique<Impl>(*other.d_ptr)){}

/*!
 * \overload
 */
Semver::Semver(Semver &&other) noexcept
    : d_ptr(std::move(other.d_ptr)){}

Semver::~Semver() = default;

void Semver::setField(Field idField, int value)
{
    d_ptr->setField(idField, value);
}

/*!
 * \brief Use to reset semver informations
 * \details
 * All fields will be reset to \c 0.
 *
 * \sa isValid()
 */
void Semver::clear()
{
    d_ptr->m_infos.fill(0);
}

int Semver::getField(Field idField) const
{
    return d_ptr->m_infos.at(idField);
}

/*!
 * \brief Use to know if semver is valid
 * \details
 * A semantic version is considered valid
 * if sum of all fields are strictly superior
 * to \c 0.
 *
 * \return
 * Returns \c true if valid
 */
bool Semver::isValid() const
{
    const int sum = std::accumulate(d_ptr->m_infos.cbegin(), d_ptr->m_infos.cend(), 0);
    return sum > 0;
}

/*!
 * \brief Use to create a string from semantic version
 * value
 *
 * \param[in] delimiter
 * Delimiter to use between semantic properties.
 * \param[in] widthField
 * Minimum size field to use. If value represented in string
 * is inferior to this size, leading zeros will be used. \n
 * For example:
 * \code{.cpp}
 * const Semver semver(2, 10, 4);
 *
 * std::cout << semver.toString('.', 2); // Print: "02.10.04"
 * std::cout << semver.toString('.', 1); // Print: "2.10.4"
 * \endcode
 *
 * \return
 * Return generated string from semantic version value. \n
 * This value can be empty if invalid.
 *
 * \sa isValid()
 * \sa parseString()
 */
std::string Semver::toString(char delimiter, int widthField) const
{
    /* Verify validity */
    if(!isValid()){
        return std::string();
    }

    /* Build a string from semver */
    return StringHelper::format("%.*d%c%.*d%c%.*d",
        widthField, getField(SEMVER_MAJOR), delimiter,
        widthField, getField(SEMVER_MINOR), delimiter,
        widthField, getField(SEMVER_PATCH), delimiter
    );
}

/*!
 * \brief Set semver informations from a string
 *
 * \param[in] version
 * String value containing semver informations.
 * \param[in] delimiter
 * Delimiter to use to parse semver informations
 *
 * \return
 * Return \c true if parsing succeed. \n
 * In case of failure, semver will be \c clear()
 *
 * \sa clear(), isValid()
 * \sa toString()
 */
bool Semver::parseString(const std::string &version, char delimiter)
{
    bool succeed = d_ptr->parseString(version, delimiter);
    if(!succeed){
        clear();
    }

    return succeed;
}

/*!
 * \brief Retrieve library semantic version at
 * runtime.
 *
 * \return
 * Return constant reference to semantic
 * version of the library
 */
const Semver& Semver::getLibraryVersion()
{
    static const Semver semver(TEASE_VERSION_MAJOR, TEASE_VERSION_MINOR, TEASE_VERSION_PATCH);
    return semver;
}

Semver& Semver::operator=(const Semver &other)
{
    /* Verify that value actually differs */
    if(this == &other){
        return *this;
    }

    /* Perform copy assignment */
    d_ptr = std::make_unique<Impl>(*other.d_ptr);
    return *this;
}

Semver& Semver::operator=(Semver &&other) noexcept
{
    d_ptr = std::move(other.d_ptr);
    return *this;
}

bool operator==(const Semver &left, const Semver &right)
{
    return left.d_ptr->m_infos == right.d_ptr->m_infos;
}

bool operator!=(const Semver &left, const Semver &right)
{
    return !(left == right);
}

bool operator<(const Semver &left, const Semver &right)
{
    return left.d_ptr->m_infos < right.d_ptr->m_infos;
}

bool operator<=(const Semver &left, const Semver &right)
{
    return (left == right || left < right);
}

bool operator>(const Semver &left, const Semver &right)
{
    return left.d_ptr->m_infos > right.d_ptr->m_infos;
}

bool operator>=(const Semver &left, const Semver &right)
{
    return (left == right || left > right);
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

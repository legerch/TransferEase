#include "stringhelper.h"

#include <algorithm>
#include <locale>

#include "transferease/logs/abstractlogger.h"

/*****************************/
/* Class documentations      */
/*****************************/

/*****************************/
/* Macro definitions         */
/*****************************/

/*****************************/
/* Start namespace           */
/*****************************/
namespace tease{

/*****************************/
/* Standard functions        */
/*****************************/

/*!
 * \brief Use to transform a string to all lowercase
 * \param[in] str
 * String to use for transformation to lowercase. \n
 * This method will properly manage string containing positive and negative values
 *
 * \warning
 * Currently, special characters (like accents or symbol) are not properly handle
 *
 * \return
 * Return lowercase string copy
 */
std::string StringHelper::toLower(const std::string &str)
{
    std::string strLower(str);
    std::locale locale;

    auto to_lower = [&locale] (char ch) { return std::use_facet<std::ctype<char>>(locale).tolower(ch); };

    std::transform(strLower.begin(), strLower.end(), strLower.begin(), to_lower);
    return strLower;
}

/*!
 * \brief Use to transform a string to all upercase
 * \param[in] str
 * String to use for transformation to uppercase. \n
 * This method will properly manage string containing positive and negative values
 *
 * \warning
 * Currently, special characters (like accents or symbol) are not properly handle
 *
 * \return
 * Return uppercase string copy
 */
std::string StringHelper::toUpper(const std::string &str)
{
    std::string strUpper(str);
    std::locale locale;

    auto to_upper = [&locale] (char ch) { return std::use_facet<std::ctype<char>>(locale).toupper(ch); };

    std::transform(strUpper.begin(), strUpper.end(), strUpper.begin(), to_upper);
    return strUpper;
}

/*!
 * \brief Use to convert a string value to integer
 * \details
 * For more details, please refer to: https://en.cppreference.com/w/cpp/string/basic_string/stol
 *
 * \param str
 * String value to convert. \n
 * Optional prefix are authorized depending on base :
 * - <b>base 10:</b> Plus or minus sign
 * - <b>base 16 (hexadecimal base):</b> Prefix \c 0x
 * - <b>base 8 (octal base):</b> Prefix \c 0
 *
 * \param base
 * Base to use, must be in interval <tt>[0;36]</tt>
 * \param succeed
 * Optional argument which will be set to \c true if method succeed, \c false otherwise. \n
 * This value can be \c nullptr
 *
 * \return
 * Return \c 0 if fail, otherwise converted integer is returned.
 */
int StringHelper::toInt(const std::string &str, int base, bool *succeed)
{
    bool convertionSucceed = true;
    int tmpInt = 0;

    /* Try to cast sting to integer */
    try{
        tmpInt = std::stoi(str, nullptr, base);

    }catch(std::invalid_argument const& ex){
        convertionSucceed = false;

        const std::string err = format("Invalid argument: [str: %s; base: %d; exception: %s]", str.c_str(), base, ex.what());
        TEASE_LOG_ERROR(err);

    }catch(std::out_of_range const& ex){
        convertionSucceed = false;

        const std::string err = format("Out of range: [str: %s; base: %d; exception: %s]", str.c_str(), base, ex.what());
        TEASE_LOG_ERROR(err);
    }

    /* Reset integer to return if failure */
    if(!convertionSucceed){
        tmpInt = 0;
    }

    /* Update success status if needed */
    if(succeed){
        *succeed = convertionSucceed;
    }

    return tmpInt;
}

/*****************************/
/* End namespace             */
/*****************************/

} // namespace tease

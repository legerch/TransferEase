#ifndef TEASE_TOOLS_STRINGHELPER_H
#define TEASE_TOOLS_STRINGHELPER_H

#include <stdexcept>
#include <string>

/*****************************/
/* Namespace instructions    */
/*****************************/
namespace tease{

/*****************************/
/* Class definitions         */
/*****************************/
class StringHelper
{
public:
    static std::string toLower(const std::string &str);
    static std::string toUpper(const std::string &str);

    static int toInt(const std::string &str, int base = 10, bool *succeed = nullptr);

public:
    template<typename ... Args>
    static std::string format(const std::string& format, Args ... args);
};

/*****************************/
/* Template implementation   */
/*****************************/

template<typename ... Args>
std::string StringHelper::format(const std::string &format, Args ... args)
{
    /* Count how many char we need to store string */
    size_t size = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1;
    if(size <= 0){
        throw std::runtime_error("Failed to format std::string");
    }

    /* Allocate and fill buffer */
    char *buffer = new char[size];
    std::snprintf(buffer, size, format.c_str(), args ...);

    /* Convert buffer to std::string */
    std::string strResult(buffer);
    delete[] buffer;

    return strResult;
}

/*****************************/
/* End namespaces            */
/*****************************/

} // namespace tease

/*****************************/
/* End file                  */
/*****************************/

#endif // TEASE_TOOLS_STRINGHELPER_H

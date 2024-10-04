#ifndef TEASE_TOOLS_FILESYSTEMHELPER_H
#define TEASE_TOOLS_FILESYSTEMHELPER_H

#include <filesystem>

/*****************************/
/* Namespace instructions    */
/*****************************/
namespace tease{

/*****************************/
/* Class definitions         */
/*****************************/
class FileSystemHelper
{

public:
    static std::filesystem::path getFilePathDir(const std::string &filepath);
    static bool createDirectories(const std::filesystem::path &pathDirectories);
};

/*****************************/
/* Template implementation   */
/*****************************/

/*****************************/
/* End namespaces            */
/*****************************/

} // namespace tease

/*****************************/
/* End file                  */
/*****************************/

#endif // TEASE_TOOLS_FILESYSTEMHELPER_H

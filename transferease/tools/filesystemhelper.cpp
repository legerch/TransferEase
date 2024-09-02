#include "filesystemhelper.h"

#include "logs/logmanager.h"
#include "stringhelper.h"

std::filesystem::__cxx11::path tease::FileSystemHelper::getFilePathDir(const std::string &filepath)
{
    return std::filesystem::path(filepath).parent_path();
}

bool tease::FileSystemHelper::createDirectories(const std::filesystem::path &pathDirectories)
{
    std::error_code errId;

    /* Verify if directories exists */
    if(std::filesystem::exists(pathDirectories)){
        return true;
    }

    /* Create directories */
    bool succeed = std::filesystem::create_directories(pathDirectories, errId);
    if(!succeed){
        const std::string err = StringHelper::format("Unable to create directories [path: %s, error: %s]", pathDirectories.c_str(), errId.message().c_str());
        TEASE_LOG_ERROR(err);
        return false;
    }

    return true;
}

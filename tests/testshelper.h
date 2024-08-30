#ifndef TEASE_TESTSHELPER_H
#define TEASE_TESTSHELPER_H

#include <string>

#include "transferease/net/url.h"

using Url = tease::Url;

class TestsHelper
{

public:
    static std::string getPathExternalRsc(const std::string &filename);
};

#endif // TEASE_TESTSHELPER_H

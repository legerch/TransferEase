#ifndef TEASE_TESTSHELPER_H
#define TEASE_TESTSHELPER_H

#include <string>

#include "transferease/net/bytesarray.h"
#include "transferease/net/url.h"
#include "transferease/version/semver.h"

using BytesArray = tease::BytesArray;
using Semver = tease::Semver;
using Url = tease::Url;

class TestsHelper
{

public:
    static std::string getPathExternalRsc(const std::string &filename);
};

#endif // TEASE_TESTSHELPER_H

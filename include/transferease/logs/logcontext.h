#ifndef TEASE_LOGS_LOGCONTEXT_H
#define TEASE_LOGS_LOGCONTEXT_H

#include "transferease/transferease_global.h"

namespace tease{

class LogContext
{
    TEASE_DISABLE_COPY(LogContext)

public:
    constexpr LogContext(const char *filename, int lineNumber, const char *fctName) :
        line(lineNumber), file(filename), function(fctName){};

public:
    int line = 0;
    const char *file = nullptr;
    const char *function = nullptr;
};

} // namespace tease

#endif // TEASE_LOGS_LOGCONTEXT_H

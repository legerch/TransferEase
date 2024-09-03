#ifndef HANDLE_H
#define HANDLE_H

#include "transferease_global.h"

namespace tease
{

class Handle final
{
    TEASE_DISABLE_COPY_MOVE(Handle)

public:
    static Handle& instance();

private:
    Handle();
    ~Handle();
};

} // namespace tease

#endif // HANDLE_H

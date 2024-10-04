#ifndef TEASE_NET_HANDLE_H
#define TEASE_NET_HANDLE_H

#include "transferease/transferease_global.h"

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

#endif // TEASE_NET_HANDLE_H

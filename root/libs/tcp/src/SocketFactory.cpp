#include "tcp/ISocket.h"
#include "tcp/TcpSocketLinux.h"
#include "tcp/TcpSocketWin.h"
#include <memory>

namespace tcp
{

    std::unique_ptr<ISocket> createSocket()
    {
#if defined(_WIN32)
        return std::make_unique<TcpSocketWin>();
#else
        return std::make_unique<TcpSocketLinux>();
#endif
    }

}
#pragma once
#include "tcp/ISocket.h"
#include <memory>
namespace tcp
{
    std::unique_ptr<ISocket> createSocket();
}
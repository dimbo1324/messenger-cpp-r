#pragma once
#include <string>
#include <cstddef>
namespace tcp
{
    class ISocket
    {
    public:
        virtual ~ISocket() = default;
        virtual bool connect(const std::string &host, int port) = 0;
        virtual std::size_t send(const char *data, std::size_t length) = 0;
        virtual std::size_t receive(char *buffer, std::size_t maxlen) = 0;
        virtual void close() = 0;
    };
}
#pragma once

#include "tcp/ISocket.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace tcp
{
    constexpr std::uint32_t kFrameHeaderSize = 4;
    constexpr std::size_t kDefaultMaxFrameSize = 64 * 1024;

    enum class FrameStatus
    {
        Ready,
        NeedMoreData,
        TooLarge,
        Invalid
    };

    enum class ReceiveFrameStatus
    {
        Ok,
        Closed,
        Error,
        TooLarge,
        Invalid
    };

    std::vector<char> encodeFrame(const std::string &payload);

    class FrameDecoder
    {
    public:
        explicit FrameDecoder(std::size_t maxFrameSize = kDefaultMaxFrameSize);

        void append(const char *data, std::size_t length);
        FrameStatus nextFrame(std::string &payload);
        std::size_t bufferedBytes() const;
        void clear();

    private:
        std::size_t maxFrameSize_;
        std::vector<char> buffer_;
    };

    bool sendAll(ISocket &socket, const char *data, std::size_t length);
    bool sendFrame(ISocket &socket,
                   const std::string &payload,
                   std::size_t maxFrameSize = kDefaultMaxFrameSize);
    ReceiveFrameStatus receiveFrame(ISocket &socket,
                                    std::string &payload,
                                    std::size_t maxFrameSize = kDefaultMaxFrameSize);
}

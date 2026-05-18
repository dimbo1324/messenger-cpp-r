#include "tcp/Protocol.h"

#include <algorithm>
#include <array>
#include <cstring>

namespace
{
    std::uint32_t readBigEndian32(const char *data)
    {
        const auto b0 = static_cast<unsigned char>(data[0]);
        const auto b1 = static_cast<unsigned char>(data[1]);
        const auto b2 = static_cast<unsigned char>(data[2]);
        const auto b3 = static_cast<unsigned char>(data[3]);
        return (static_cast<std::uint32_t>(b0) << 24) |
               (static_cast<std::uint32_t>(b1) << 16) |
               (static_cast<std::uint32_t>(b2) << 8) |
               static_cast<std::uint32_t>(b3);
    }

    void writeBigEndian32(std::uint32_t value, char *out)
    {
        out[0] = static_cast<char>((value >> 24) & 0xFF);
        out[1] = static_cast<char>((value >> 16) & 0xFF);
        out[2] = static_cast<char>((value >> 8) & 0xFF);
        out[3] = static_cast<char>(value & 0xFF);
    }
}

namespace tcp
{
    std::vector<char> encodeFrame(const std::string &payload)
    {
        std::vector<char> frame(kFrameHeaderSize + payload.size());
        writeBigEndian32(static_cast<std::uint32_t>(payload.size()), frame.data());
        std::copy(payload.begin(), payload.end(), frame.begin() + kFrameHeaderSize);
        return frame;
    }

    FrameDecoder::FrameDecoder(std::size_t maxFrameSize)
        : maxFrameSize_(maxFrameSize)
    {
    }

    void FrameDecoder::append(const char *data, std::size_t length)
    {
        if (data == nullptr || length == 0)
        {
            return;
        }
        buffer_.insert(buffer_.end(), data, data + length);
    }

    FrameStatus FrameDecoder::nextFrame(std::string &payload)
    {
        payload.clear();
        if (buffer_.size() < kFrameHeaderSize)
        {
            return FrameStatus::NeedMoreData;
        }

        const std::uint32_t payloadSize = readBigEndian32(buffer_.data());
        if (payloadSize > maxFrameSize_)
        {
            return FrameStatus::TooLarge;
        }

        const std::size_t totalSize = kFrameHeaderSize + static_cast<std::size_t>(payloadSize);
        if (buffer_.size() < totalSize)
        {
            return FrameStatus::NeedMoreData;
        }

        payload.assign(buffer_.begin() + kFrameHeaderSize, buffer_.begin() + totalSize);
        buffer_.erase(buffer_.begin(), buffer_.begin() + totalSize);
        return FrameStatus::Ready;
    }

    std::size_t FrameDecoder::bufferedBytes() const
    {
        return buffer_.size();
    }

    void FrameDecoder::clear()
    {
        buffer_.clear();
    }

    bool sendAll(ISocket &socket, const char *data, std::size_t length)
    {
        std::size_t sent = 0;
        while (sent < length)
        {
            const std::size_t chunk = socket.send(data + sent, length - sent);
            if (chunk == 0)
            {
                return false;
            }
            sent += chunk;
        }
        return true;
    }

    bool sendFrame(ISocket &socket, const std::string &payload, std::size_t maxFrameSize)
    {
        if (payload.size() > maxFrameSize)
        {
            return false;
        }
        const auto frame = encodeFrame(payload);
        return sendAll(socket, frame.data(), frame.size());
    }

    ReceiveFrameStatus receiveFrame(ISocket &socket,
                                    std::string &payload,
                                    std::size_t maxFrameSize)
    {
        payload.clear();

        std::array<char, kFrameHeaderSize> header{};
        std::size_t received = 0;
        while (received < header.size())
        {
            const std::size_t n = socket.receive(header.data() + received, header.size() - received);
            if (n == 0)
            {
                return received == 0 ? ReceiveFrameStatus::Closed : ReceiveFrameStatus::Error;
            }
            received += n;
        }

        const std::uint32_t payloadSize = readBigEndian32(header.data());
        if (payloadSize > maxFrameSize)
        {
            return ReceiveFrameStatus::TooLarge;
        }

        payload.resize(payloadSize);
        received = 0;
        while (received < payload.size())
        {
            const std::size_t n = socket.receive(&payload[received], payload.size() - received);
            if (n == 0)
            {
                return ReceiveFrameStatus::Error;
            }
            received += n;
        }

        return ReceiveFrameStatus::Ok;
    }
}

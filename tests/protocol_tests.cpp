#include "tcp/Protocol.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

class MemorySocket : public tcp::ISocket
{
public:
    explicit MemorySocket(std::vector<char> input = {})
        : input_(std::move(input))
    {
    }

    bool connect(const std::string &, int) override
    {
        return true;
    }

    std::size_t send(const char *data, std::size_t length) override
    {
        if (failSend_ || data == nullptr || length == 0)
        {
            return 0;
        }
        const std::size_t chunk = std::min<std::size_t>(length, maxSendChunk_);
        output_.insert(output_.end(), data, data + chunk);
        return chunk;
    }

    std::size_t receive(char *buffer, std::size_t maxlen) override
    {
        if (buffer == nullptr || maxlen == 0 || readOffset_ >= input_.size())
        {
            return 0;
        }
        const std::size_t available = input_.size() - readOffset_;
        const std::size_t chunk = std::min({available, maxlen, maxReceiveChunk_});
        std::memcpy(buffer, input_.data() + readOffset_, chunk);
        readOffset_ += chunk;
        return chunk;
    }

    void close() override {}

    std::vector<char> input_;
    std::vector<char> output_;
    std::size_t readOffset_{0};
    std::size_t maxSendChunk_{2};
    std::size_t maxReceiveChunk_{2};
    bool failSend_{false};
};

int main()
{
    {
        const auto frame = tcp::encodeFrame("hello");
        tcp::FrameDecoder decoder;
        std::string payload;
        decoder.append(frame.data(), 2);
        assert(decoder.nextFrame(payload) == tcp::FrameStatus::NeedMoreData);
        decoder.append(frame.data() + 2, frame.size() - 2);
        assert(decoder.nextFrame(payload) == tcp::FrameStatus::Ready);
        assert(payload == "hello");
    }

    {
        const auto first = tcp::encodeFrame("one");
        const auto second = tcp::encodeFrame("two");
        tcp::FrameDecoder decoder;
        std::string payload;
        decoder.append(first.data(), first.size());
        decoder.append(second.data(), second.size());
        assert(decoder.nextFrame(payload) == tcp::FrameStatus::Ready);
        assert(payload == "one");
        assert(decoder.nextFrame(payload) == tcp::FrameStatus::Ready);
        assert(payload == "two");
        assert(decoder.nextFrame(payload) == tcp::FrameStatus::NeedMoreData);
    }

    {
        const auto frame = tcp::encodeFrame("too-large");
        tcp::FrameDecoder decoder(3);
        std::string payload;
        decoder.append(frame.data(), frame.size());
        assert(decoder.nextFrame(payload) == tcp::FrameStatus::TooLarge);
    }

    {
        tcp::FrameDecoder decoder;
        std::string payload;
        assert(decoder.nextFrame(payload) == tcp::FrameStatus::NeedMoreData);
    }

    {
        MemorySocket socket;
        assert(tcp::sendFrame(socket, "hello", 5));
        tcp::FrameDecoder decoder;
        std::string payload;
        decoder.append(socket.output_.data(), socket.output_.size());
        assert(decoder.nextFrame(payload) == tcp::FrameStatus::Ready);
        assert(payload == "hello");
        assert(!tcp::sendFrame(socket, "too-large", 3));
    }

    {
        const auto frame = tcp::encodeFrame("receive");
        MemorySocket socket(frame);
        std::string payload;
        assert(tcp::receiveFrame(socket, payload, 64) == tcp::ReceiveFrameStatus::Ok);
        assert(payload == "receive");
    }

    {
        const auto frame = tcp::encodeFrame("too-large");
        MemorySocket socket(frame);
        std::string payload;
        assert(tcp::receiveFrame(socket, payload, 3) == tcp::ReceiveFrameStatus::TooLarge);
    }

    return 0;
}

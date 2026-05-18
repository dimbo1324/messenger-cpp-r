#include "tcp/Protocol.h"

#include <cassert>
#include <string>

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

    return 0;
}

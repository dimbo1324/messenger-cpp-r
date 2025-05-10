#include "LoggerThread.h"

LoggerThread::LoggerThread(std::mutex &_mutex, char (&_cmd_buffer)[CMD_BUFFER], std::fstream &_file_stream) : mutex(_mutex), file_stream(_file_stream)
{
    for (int i{0}; i < CMD_BUFFER; i++)
        cmd_buffer[i] = _cmd_buffer[i];
}

LoggerThread::~LoggerThread()
{
    if (thread_reader.joinable())
        thread_reader.join();
    if (thread_writer.joinable())
        thread_writer.join();
}

void LoggerThread::write()
{
    thread_writer = std::move(std::thread(&LoggerThread::_write, this));
}

void LoggerThread::read()
{
    thread_reader = std::move(std::thread(&LoggerThread::_read, this));
}

void LoggerThread::_write()
{
    mutex.lock();
    BufferActions buffer{cmd_buffer};
    std::string str = "[" + Misc::StampToTime(time(NULL)) + "] ";
    str += "[login: " + buffer.getDynDataS(LOGIN_COUNT) + "] ";
    str += "[page: " + buffer.getDynDataS(PAGE_TEXT_COUNT) + "] ";
    str += "[command: " + buffer.getDynDataS(CMD_TEXT_COUNT) + "]";
    uint s_find = 0;
    while (1)
    {
        s_find = str.find("\n");
        if (s_find != (uint)std::string::npos)
            str.replace(s_find, 1, ":");
        else
            break;
    }
    str += "\n";
    file_stream.write(str.data(), str.size());
    file_stream.flush();
    mutex.unlock();
}

void LoggerThread::_read()
{
    mutex.lock();
    int file_size = std::filesystem::file_size(LOG_FILE_NAME);
    std::string last_str;

    for (int i{file_size}; i > 0; i--)
    {
        std::streampos pos = i - 1;
        file_stream.seekp(pos);
        char ch[1];
        file_stream.read(ch, 1);
        if (i == file_size && *ch == '\n')
            continue;
        if (*ch != '\n')
            last_str.insert(0, 1, ch[0]);
        else
            break;
    }
    if (last_str.empty())
        last_str = "Не найдена последняя строка лога.";
    Misc::printMessage(last_str);
    Misc::printMessage("server> ");
    mutex.unlock();
}

#include <thread>
#include <fstream>
#include <filesystem>
#include <string>
#include <iostream>

#include "../../../Misc/BufferActions.h"

#define LOG_FILE_NAME "access.log"

class LoggerThread
{
private:
    std::mutex &mutex;
    char cmd_buffer[CMD_BUFFER];
    std::fstream &file_stream;

    std::thread thread_writer;
    std::thread thread_reader;

public:
    LoggerThread(std::mutex &_mutex, char (&_cmd_buffer)[CMD_BUFFER], std::fstream &_file_stream);
    ~LoggerThread();
    void write();
    void read();

private:
    void _write();
    void _read();
};
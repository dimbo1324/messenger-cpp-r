#pragma once
#include "LoggerThread.h"
#include "../../../Misc/BufferActions.h"

class Logger
{
private:
    std::mutex mutex;
    std::string log_file_name = "access.log";
    std::fstream file_stream;
    char (&cmd_buffer)[CMD_BUFFER];

public:
        Logger(char (&_cmd_buffer)[CMD_BUFFER]);
    ~Logger();

    void write();
    void read();
};
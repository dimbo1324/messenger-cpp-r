#include "Logger.h"

Logger::Logger(char (&_cmd_buffer)[CMD_BUFFER]) : cmd_buffer(_cmd_buffer)
{
    file_stream.open(log_file_name, file_stream.in | file_stream.out | file_stream.ate | file_stream.app | file_stream.binary);
}

Logger::~Logger()
{
    if (file_stream.is_open())
        file_stream.close();
}

void Logger::write()
{
    LoggerThread logger_thread(mutex, cmd_buffer, file_stream);
    logger_thread.write();
}

void Logger::read()
{
    LoggerThread logger_thread(mutex, cmd_buffer, file_stream);
    logger_thread.read();
}

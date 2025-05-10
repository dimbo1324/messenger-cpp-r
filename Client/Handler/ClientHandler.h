#pragma once
#include "../../Misc/Misc.h"
#include "../../Misc/BufferActions.h"
#include <vector>
#include <string>
#if defined(_WIN64) || defined(_WIN32)
#include "../../Misc/UserInputW.h"
#else
#include "../../Misc/UserInput.h"
#endif
#define DATA_BUFFER 4096
#define CMD_BUFFER 4096
typedef unsigned int uint;
typedef unsigned long long ullong;
const extern char clear[];
class ClientHandler
{
private:
    char (&cmd_buffer)[CMD_BUFFER];
    ullong session_key = 0;
    std::string login = "Guest";
    UserInput<int, int> userInputInt;
    UserInput<std::string, std::string> userInputStr;
    bool work = true;
    BufferActions buffer{cmd_buffer};
    std::string data_text;

public:
    explicit ClientHandler(char (&_cmd_buffer)[CMD_BUFFER]);
    ~ClientHandler() = default;
    void Initialise();
    void Run();
    bool getWork();
    void quit();
    void setDataText(const std::string &text);
    void setDataText(const std::string &&text);
};

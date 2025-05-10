#pragma once
#include "Flags.h"
#include "Misc.h"
#include <climits>
#define FLAGS_ADDR 0
#define DATA_PACKETS_ADDR 1
#define SESSION_KEY_ADDR 2
#define PG_MODE_ADDR 10
#define PG_START_ADDR 11
#define PG_PERPAGE_ADDR 15
#define USER_INPUT_COUNT_ADDR 19
#define PM_USER_ID 23
#define DYN_DATA_ADDR 31
#define LOGIN_COUNT 0
#define PAGE_TEXT_COUNT 1
#define CMD_TEXT_COUNT 2
#define CMD_BUFFER 4096
typedef unsigned int uint;
typedef unsigned long long ullong;
namespace sv
{
    enum options
    {
        get_string = 1,
        get_int = 2,
        write_session = 4,
        clear_console = 8,
        no_input = 16,
        cmd_buffer = 32,
        data_buffer = 64
    };
    enum pagination
    {
        message = 1,
        last_page = 2
    };
};
class BufferActions
{
private:
    sv::options flags = (sv::options)0;
    Flags<sv::options> f;
    uint flags_count = 0;
    uint dyndata_count = 0;
    char (&cmd_buffer)[CMD_BUFFER];

public:
    explicit BufferActions(char (&_cmd_buffer)[CMD_BUFFER]);
    ~BufferActions() = default;
    void createFlags() {}
    template <typename T, typename... Args>
    void createFlags(T value, Args... args);
    void addFlags() {}
    template <typename T, typename... Args>
    void addFlags(T value, Args... args);
    void removeFlag(sv::options option);
    bool hasFlag(sv::options option);
    void writeDynData() {}
    template <typename T, typename... Args>
    void writeDynData(T value, Args... args);
    void writeDynDataPos(std::string data, uint blockCount);
    void writeDynDataPos(uint value, uint blockCount);
    ullong getSessionKey();
    void setSessionKey(ullong key);
    sv::pagination getPaginationMode();
    void setPaginationMode(sv::pagination mode);
    uint getPgStart();
    void setPgStart(uint value);
    uint getPgPerPage();
    void setPgPerPage(uint value);
    uint getUserInputCount();
    void setUserInputCount(uint value);
    void pgClear();
    uint getDynDataSize(uint blockCount);
    std::string getDynDataS(uint blockCount);
    uint getDynDataI(uint blockCount);
    ullong getPmUserID();
    void setPmUserID(ullong id);
    void clearPmUserID();
    bool PmUserIsNotSelected();
    void setDataPacketsCount(unsigned char value);
};
template <typename T, typename... Args>
inline void BufferActions::createFlags(T value, Args... args)
{
    if (flags_count == 0)
        flags = (sv::options)0;
    flags_count++;
    flags = f.addFlag(flags, value);
    if (value == sv::get_string)
        flags = f.removeFlag(flags, sv::get_int);
    if (value == sv::get_int)
        flags = f.removeFlag(flags, sv::get_string);
    createFlags(args...);
    flags_count = 0;
    cmd_buffer[FLAGS_ADDR] = (char)flags;
}
template <typename T, typename... Args>
inline void BufferActions::addFlags(T value, Args... args)
{
    cmd_buffer[FLAGS_ADDR] = (char)f.addFlag((T)cmd_buffer[FLAGS_ADDR], value);
    addFlags(args...);
}
template <typename T, typename... Args>
inline void BufferActions::writeDynData(T value, Args... args)
{
    Misc::writeStringBuffer(value, cmd_buffer, Misc::findDynamicData(cmd_buffer, DYN_DATA_ADDR, dyndata_count));
    dyndata_count++;
    writeDynData(args...);
    dyndata_count = 0;
}

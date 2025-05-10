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

#define CMD_BUFFER 4096 // Размер буфера команд и заголовков

typedef unsigned int uint;
typedef unsigned long long ullong;

/*

    Командный пакет
    |флаги(1)|data packets (1)|session_key (8)|pg_mode(1)|pg_start(4)|pg_perPage(4)|inputs(4)| pm_user_id (8) |login_size(4)|login|page_size(4)|PAGE_TEXT|cmd_size(4)|CMD_TEXT|
    0        1                2               10         11          15            19        23               31
    |_________________________________________________________________________________________________________|
    |                                  ^                                                                      |
    |                               static                                                                    |  dynamic

    data packets - количество пакетов с текстом отправляемых клиенту
    inputs - количество байт для ввода пользователем.
*/

namespace sv
{
    enum options
    {
        // в одном байте максимальный флаг 128
        get_string = 1,    // Сообщает клиенту: надо ввести строку
        get_int = 2,       // Сообщает клиенту: надо ввести число
        write_session = 4, // Сообщает клиенту: записать в буфер сессионный ключ, пришедший с сервера.
        clear_console = 8, // Сообщает клиенту: очистить окно консоли
        no_input = 16,     // Сообщает клиенту: не производить ввод данных
        cmd_buffer = 32,   // Сообщает сокету, что это пакет команд
        data_buffer = 64   // Сообщает сокету что это пакет данных
    };

    enum pagination
    {
        message = 1,  // отобразить сообщение по его номеру в списке всех сообщений
        last_page = 2 // показать последние 10 сообщений
    };
};

/// @brief Класс работы с командным буфером
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

    /// @brief Удаляет флаг
    void removeFlag(sv::options option);

    /// @brief Проверяет наличие флага в сумме
    /// @param option
    /// @return
    bool hasFlag(sv::options option);

    void writeDynData() {}
    template <typename T, typename... Args>
    void writeDynData(T value, Args... args);

    /// @brief Записывает в буфер динамические данные в заданную позицию от первого блока
    /// @param data
    /// @param blockCount
    void writeDynDataPos(std::string data, uint blockCount);

    /// @brief Записывает в динамические данные буфера число в заданную позицию от первого блока
    /// @param value
    /// @param blockCount
    void writeDynDataPos(uint value, uint blockCount);

    /// @brief получить сессионный ключ из буфера
    /// @return
    ullong getSessionKey();
    /// @brief Изменить сессионный ключ в буфере
    /// @param key
    void setSessionKey(ullong key);

    /// @brief получить состояние пагинации
    /// @return
    sv::pagination getPaginationMode();
    /// @brief изменить состояние пагинации
    /// @param mode
    void setPaginationMode(sv::pagination mode);

    /// @brief Первая страница в пагинации
    /// @return
    uint getPgStart();
    /// @brief Установить страницу в пагинации
    /// @param value
    void setPgStart(uint value);
    /// @brief Количество сообщений на страницу в пагинации
    /// @return
    uint getPgPerPage();
    /// @brief Изменить количество сообщений в пагинации
    /// @param value
    void setPgPerPage(uint value);
    /// @brief последний элемент в пагинации
    /// @return
    uint getUserInputCount();
    /// @brief последний элемент в пагинации
    /// @param value
    void setUserInputCount(uint value);

    void pgClear();

    /// @brief получить размер подблока динамических данных
    /// @param blockCount номер подблока
    /// @return
    uint getDynDataSize(uint blockCount);

    /// @brief получить строку подблока динамических данных
    /// @param blockCount
    /// @return
    std::string getDynDataS(uint blockCount);

    /// @brief получить число в адресе подблока динамических данных
    /// @param blockCount
    /// @return
    uint getDynDataI(uint blockCount);

    /* в личных сообщениях значение PmUserID = ULLONG_MAX будет считаться как пользователь не выбран */

    /// @brief Получить ID собеседника ЛС
    /// @return
    ullong getPmUserID();

    /// @brief Изменить ID собеседника ЛС
    /// @param id
    void setPmUserID(ullong id);

    /// @brief Записывает в блок собеседника ЛС ULLONG_MAX, что значит пользователь не выбран
    void clearPmUserID();

    /// @brief Проверяет выбран ли пользователь.
    /// @return
    bool PmUserIsNotSelected();

    /// @brief Записывает количество пакетов с текстовыми данными
    void setDataPacketsCount(unsigned char value);
};

/// @brief Добавляет в буфер флаги, очищает буфер флагов
/// @tparam T
/// @tparam ...Args
/// @param value
/// @param ...args
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

/// @brief Добавляет в буфер флаги, не очищает буфер флагов
/// @tparam T
/// @tparam ...Args
/// @param value
/// @param ...args
template <typename T, typename... Args>
inline void BufferActions::addFlags(T value, Args... args)
{
    cmd_buffer[FLAGS_ADDR] = (char)f.addFlag((T)cmd_buffer[FLAGS_ADDR], value);
    addFlags(args...);
}

/// @brief Записывает в буфер динамические данные в порядке их следования от первого блока
/// @tparam T
/// @tparam ...Args
/// @param value
/// @param ...args
template <typename T, typename... Args>
inline void BufferActions::writeDynData(T value, Args... args)
{

    Misc::writeStringBuffer(value, cmd_buffer, Misc::findDynamicData(cmd_buffer, DYN_DATA_ADDR, dyndata_count));
    dyndata_count++;
    writeDynData(args...);
    dyndata_count = 0;
}

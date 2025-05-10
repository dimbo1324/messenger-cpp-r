#include "ClientHandler.h"

ClientHandler::ClientHandler(char (&_cmd_buffer)[CMD_BUFFER]) : cmd_buffer(_cmd_buffer) {}

void ClientHandler::Initialise()
{
    // При инициализации клиента надо создать нулевой сессионный ключ
    session_key = 0;

    // Статические данные
    buffer.createFlags(sv::get_string);
    buffer.setSessionKey(session_key);
    buffer.setPaginationMode(sv::last_page);
    buffer.setPgPerPage(10);
    buffer.setPgStart(1);
    buffer.setUserInputCount(1000);
    buffer.clearPmUserID();

    // Динамические данные
    buffer.writeDynData(login, "none", "none");

    data_text =
        "Вы запустили клиент чата.\n"
        "Команда /help - справка.\n"
        "Команда /hello - опрос сервера.\n"
        "Вы не авторизованы в чате.\n"
        "Команда: /auth:login:password - войти в чат\n"
        "Команда: /reg - регистрация в чате\n"
        "Введите команду: ";
}

void ClientHandler::Run()
{
    if (buffer.hasFlag(sv::clear_console))
        system(clear);
    buffer.removeFlag(sv::clear_console);

    if (buffer.hasFlag(sv::write_session))
    {
        session_key = buffer.getSessionKey();
    }
    buffer.removeFlag(sv::write_session);

    Misc::printMessage(data_text, false);

    // запишем в буфер текст который отобразится если сервер отвалится
    data_text = "Сервер не ответил на ваш запрос.\nВведите команду: ";

    // пишем ответ серверу

    if (buffer.hasFlag(sv::no_input))
    {
        // сервер не требует ввод пользователя для редиректа на другую страницу
        buffer.createFlags(sv::get_string);
        return;
    }

    if (buffer.hasFlag(sv::get_string))
    {
        std::string s;

        do
        {
            s = userInputStr.getStringIO();
            // ограничение ввода
            uint max_symbols = buffer.getUserInputCount();
            uint characters_entered = Misc::getSymbolsCount(s);
            if (characters_entered > max_symbols)
            {
                Misc::printMessage("Вы превысили максимально допустимое количество символов - " + std::to_string(max_symbols));
                Misc::printMessage("Введите допустимое количество символов: ", false);
                continue;
            }
            else
            {
                break;
            }

        } while (1);

        if (s.size() == 0)
        {
            // если введена пустая строка - записывается команда /update
            buffer.writeDynDataPos("/update", CMD_TEXT_COUNT);
        }
        else
        {
            // Предотвращение полного переполнения буфера
            uint cmd_pos = Misc::findDynamicData(cmd_buffer, DYN_DATA_ADDR, CMD_TEXT_COUNT);
            uint cmd_max_size = CMD_BUFFER - cmd_pos - 5;
            if (s.size() > cmd_max_size)
            {
                s.erase(0, s.size() - cmd_max_size);
            }
            buffer.setUserInputCount(1000); // сбросим ограничение на ввод
            buffer.writeDynDataPos(s, CMD_TEXT_COUNT);
        }

        if (s == "/quit")
        {
            quit();
        }
    }

    return;
}

bool ClientHandler::getWork()
{
    return work;
}

void ClientHandler::quit()
{
    work = false;
}

void ClientHandler::setDataText(const std::string &text)
{
    data_text = text;
}

void ClientHandler::setDataText(const std::string &&text)
{
    setDataText(text);
}

#pragma once

namespace serverMessages
{
    constexpr const char *WSASTARTUP_ERROR = "WSAStartup не удалась: ";
    constexpr const char *SERVER_SOCKET_ERROR = "Ошибка создания серверного сокета.";
    constexpr const char *BIND_ERROR = "Ошибка привязки сокета к порту ";
    constexpr const char *LISTEN_ERROR = "Ошибка прослушивания сокета.";
    constexpr const char *SERVER_STARTED = "Сервер запущен на порту ";
    constexpr const char *CLIENT_ACCEPT_ERROR = "Ошибка приема клиента.";
    constexpr const char *NEW_CLIENT_CONNECTED = "Новый клиент подключился: ";
    constexpr const char *CLIENT_DISCONNECTED = "Клиент отключился.";
    constexpr const char *SEND_ERROR = "Ошибка отправки данных клиенту.";
    constexpr const char *RECEIVE_ERROR = "Ошибка приема данных от клиента.";
    constexpr const char *SERVER_STOPPED = "Сервер остановлен.";
}

namespace clientMessages
{
    constexpr const char *WSASTARTUP_ERROR = "WSAStartup не удалась: ";
    constexpr const char *CLIENT_SOCKET_ERROR = "Ошибка создания клиентского сокета.";
    constexpr const char *INVALID_ADDRESS = "Некорректный адрес или адрес не поддерживается.";
    constexpr const char *CONNECT_ERROR = "Ошибка подключения к серверу.";
    constexpr const char *CONNECT_SUCCESS = "Подключение к серверу ";
    constexpr const char *DISCONNECT_SUCCESS = "Отключение от сервера выполнено.";
    constexpr const char *SEND_ERROR = "Ошибка отправки сообщения.";
    constexpr const char *RECEIVE_ERROR = "Ошибка приёма данных.";
    constexpr const char *SERVER_CLOSED = "Сервер закрыл соединение.";
}

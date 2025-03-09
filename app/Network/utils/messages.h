#pragma once
#include <string>
namespace SERVER_MESSAGES
{
    const std::string WINSOCK_INIT_ERROR = "Ошибка! Инициализация Winsock не удалась! Ошибка типа: ";
    const std::string SOCKET_CREATE_ERROR = "Ошибка! Создание сокета не удалось! Ошибка типа: ";
    const std::string BIND_ERROR = "Ошибка! Привязка сокета не удалась! Ошибка типа: ";
    const std::string LISTEN_ERROR = "Ошибка! Прослушивание сокета не удалось! Ошибка типа: ";
    const std::string ACCEPT_ERROR = "Ошибка! Принятие соединения не удалось! Ошибка типа: ";
    const std::string RECV_ERROR = "Ошибка при получении сообщения от клиента.";
    const std::string SEND_ERROR = "Ошибка при отправке ответа клиенту. Ошибка: ";
    const std::string SERVER_STARTED = "Сервер запущен! Прослушивание порта: ";
    const std::string CLIENT_CONNECTED = "Клиент подключился! IP: ";
    const std::string CLIENT_DISCONNECTED = "Клиент отключился.";
    const std::string MESSAGE_RECEIVED = "Получено сообщение от клиента: ";
    const std::string RESPONSE_SENT = "Ответ отправлен клиенту.";
}

/*
 * Задача: Обеспечить обертку для работы с ODBC:
 * Инициализация и завершение работы ODBC.
 * Открытие и закрытие соединения с PostgreSQL через DSN.
 * Функции для выполнения SQL-запросов (SELECT, INSERT, UPDATE, DELETE).
 * Функции для подготовки и выполнения параметризованных запросов.
 * Обработка ошибок (вывод подробных сообщений об ошибках).
 */
#pragma once
#include <string>
#include <vector>
#include <sqltypes.h>
struct QueryResult
{
    std::vector<std::string> rows;
};
class DatabaseManager
{
public:
    DatabaseManager();
    ~DatabaseManager();
    bool init();
    bool openConnection();
    bool executeNonQuery(const std::string &sql);
    bool executeQuery(const std::string &sql, QueryResult &result);
    void closeConnection();

private:
    void logDiagnostics(short handleType, void *handle);
    SQLHANDLE env_ = nullptr;
    SQLHANDLE dbc_ = nullptr;
};

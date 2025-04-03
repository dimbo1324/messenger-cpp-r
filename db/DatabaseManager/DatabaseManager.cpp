#include "DatabaseManager.h"
#include "../DBConfig.h"
#include <iostream>
#include <sql.h>
#include <sqlext.h>
DatabaseManager::DatabaseManager()
{
}
DatabaseManager::~DatabaseManager()
{
    closeConnection();
}
bool DatabaseManager::init()
{
    SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env_);
    if (!SQL_SUCCEEDED(ret))
    {
        std::cerr << "Ошибка: не удалось аллоцировать environment handle." << std::endl;
        return false;
    }
    ret = SQLSetEnvAttr(env_, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3, 0);
    if (!SQL_SUCCEEDED(ret))
    {
        std::cerr << "Ошибка: не удалось установить атрибут ODBC версии." << std::endl;
        SQLFreeHandle(SQL_HANDLE_ENV, env_);
        return false;
    }
    ret = SQLAllocHandle(SQL_HANDLE_DBC, env_, &dbc_);
    if (!SQL_SUCCEEDED(ret))
    {
        std::cerr << "Ошибка: не удалось аллоцировать connection handle." << std::endl;
        SQLFreeHandle(SQL_HANDLE_ENV, env_);
        return false;
    }
    std::cout << "ODBC инициализация прошла успешно." << std::endl;
    return true;
}
bool DatabaseManager::openConnection()
{
    std::string connStr = "DSN=";
    connStr += DSN;
    connStr += ";UID=";
    connStr += DB_USER;
    connStr += ";PWD=";
    connStr += DB_PASSWORD;
    connStr += ";DATABASE=";
    connStr += DB_NAME;
    connStr += ";";
    SQLCHAR outConnStr[1024];
    SQLSMALLINT outConnStrLen;
    SQLRETURN ret = SQLDriverConnect(
        dbc_,
        NULL,
        (SQLCHAR *)connStr.c_str(),
        SQL_NTS,
        outConnStr,
        sizeof(outConnStr),
        &outConnStrLen,
        SQL_DRIVER_COMPLETE);
    if (!SQL_SUCCEEDED(ret))
    {
        std::cerr << "Ошибка: не удалось установить соединение с базой данных." << std::endl;
        logDiagnostics(SQL_HANDLE_DBC, dbc_);
        return false;
    }
    std::cout << "Соединение установлено успешно." << std::endl;
    return true;
}
bool DatabaseManager::executeNonQuery(const std::string &sql)
{
    SQLHANDLE stmt;
    SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc_, &stmt);
    if (!SQL_SUCCEEDED(ret))
    {
        std::cerr << "Ошибка: не удалось аллоцировать statement handle." << std::endl;
        return false;
    }
    ret = SQLExecDirect(stmt, (SQLCHAR *)sql.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret))
    {
        std::cerr << "Ошибка при выполнении запроса: " << sql << std::endl;
        logDiagnostics(SQL_HANDLE_STMT, stmt);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return false;
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return true;
}
bool DatabaseManager::executeQuery(const std::string &sql, QueryResult &result)
{
    SQLHANDLE stmt;
    SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc_, &stmt);
    if (!SQL_SUCCEEDED(ret))
    {
        std::cerr << "Ошибка: не удалось аллоцировать statement handle для SELECT." << std::endl;
        return false;
    }
    ret = SQLExecDirect(stmt, (SQLCHAR *)sql.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret))
    {
        std::cerr << "Ошибка при выполнении SELECT запроса: " << sql << std::endl;
        logDiagnostics(SQL_HANDLE_STMT, stmt);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return false;
    }
    SQLCHAR buffer[256];
    while (SQLFetch(stmt) == SQL_SUCCESS)
    {
        SQLLEN indicator;
        ret = SQLGetData(stmt, 1, SQL_C_CHAR, buffer, sizeof(buffer), &indicator);
        if (SQL_SUCCEEDED(ret))
        {
            result.rows.push_back(std::string((char *)buffer));
        }
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return true;
}
void DatabaseManager::closeConnection()
{
    if (dbc_)
    {
        SQLDisconnect(dbc_);
        SQLFreeHandle(SQL_HANDLE_DBC, dbc_);
        dbc_ = nullptr;
    }
    if (env_)
    {
        SQLFreeHandle(SQL_HANDLE_ENV, env_);
        env_ = nullptr;
    }
    std::cout << "Соединение закрыто, ресурсы освобождены." << std::endl;
}
void DatabaseManager::logDiagnostics(short handleType, void *handle)
{
    SQLCHAR sqlState[1024];
    SQLCHAR message[1024];
    if (SQLGetDiagRec(handleType, handle, 1, sqlState, NULL, message, sizeof(message), NULL) == SQL_SUCCESS)
    {
        std::cerr << "SQLState: " << sqlState << " Message: " << message << std::endl;
    }
}

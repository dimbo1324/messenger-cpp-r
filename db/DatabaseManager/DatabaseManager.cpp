#include "DatabaseManager.h"
#include <iostream>
#include <sql.h>
#include <sqlext.h>
#include <sstream>
#include <ostream>
#include <DBConfig.h>

DatabaseManager::DatabaseManager() {}

DatabaseManager::~DatabaseManager()
{
    closeConnection();
}

bool DatabaseManager::init()
{
    SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env_);
    if (!SQL_SUCCEEDED(ret))
    {
        std::cerr << "Ошибка: не удалось создать environment handle." << std::endl;
        return false;
    }
    ret = SQLSetEnvAttr(env_, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3, 0);
    if (!SQL_SUCCEEDED(ret))
    {
        std::cerr << "Ошибка: не удалось установить версию ODBC." << std::endl;
        SQLFreeHandle(SQL_HANDLE_ENV, env_);
        return false;
    }
    ret = SQLAllocHandle(SQL_HANDLE_DBC, env_, &dbc_);
    if (!SQL_SUCCEEDED(ret))
    {
        std::cerr << "Ошибка: не удалось создать connection handle." << std::endl;
        SQLFreeHandle(SQL_HANDLE_ENV, env_);
        return false;
    }
    std::cout << "ODBC инициализирован успешно." << std::endl;
    return true;
}

bool DatabaseManager::openConnection()
{
    std::string connStr = "DSN=" + std::string(DSN) + ";UID=" + DB_USER + ";PWD=" + DB_PASSWORD + ";";
    SQLCHAR outConnStr[1024];
    SQLSMALLINT outConnStrLen;
    SQLRETURN ret = SQLDriverConnectA(
        dbc_, nullptr, (SQLCHAR *)connStr.c_str(), SQL_NTS,
        outConnStr, sizeof(outConnStr), &outConnStrLen, SQL_DRIVER_COMPLETE);
    if (!SQL_SUCCEEDED(ret))
    {
        std::cerr << "Ошибка: не удалось подключиться к базе данных." << std::endl;
        logDiagnostics(SQL_HANDLE_DBC, dbc_);
        return false;
    }
    std::cout << "Соединение с базой данных установлено." << std::endl;
    return true;
}

bool DatabaseManager::executeNonQuery(const std::string &sql)
{
    SQLHANDLE stmt;
    SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc_, &stmt);
    if (!SQL_SUCCEEDED(ret))
    {
        std::cerr << "Ошибка: не удалось создать statement handle." << std::endl;
        return false;
    }
    ret = SQLExecDirectA(stmt, (SQLCHAR *)sql.c_str(), SQL_NTS);
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
        std::cerr << "Ошибка: не удалось создать statement handle для SELECT." << std::endl;
        return false;
    }
    ret = SQLExecDirectA(stmt, (SQLCHAR *)sql.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret))
    {
        std::cerr << "Ошибка при выполнении SELECT: " << sql << std::endl;
        logDiagnostics(SQL_HANDLE_STMT, stmt);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return false;
    }
    SQLSMALLINT numCols;
    SQLNumResultCols(stmt, &numCols);
    while (SQLFetch(stmt) == SQL_SUCCESS)
    {
        std::vector<std::string> row;
        for (SQLSMALLINT i = 1; i <= numCols; ++i)
        {
            SQLCHAR buffer[256];
            SQLLEN indicator;
            ret = SQLGetData(stmt, i, SQL_C_CHAR, buffer, sizeof(buffer), &indicator);
            if (SQL_SUCCEEDED(ret) && indicator != SQL_NULL_DATA)
            {
                row.push_back(std::string((char *)buffer));
            }
            else
            {
                row.push_back("");
            }
        }
        result.rows.push_back(row);
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
    std::cout << "Соединение с базой данных закрыто." << std::endl;
}

void DatabaseManager::logDiagnostics(short handleType, void *handle)
{
    SQLCHAR sqlState[1024];
    SQLCHAR message[1024];
    if (SQLGetDiagRecA(handleType, handle, 1, sqlState, nullptr, message, sizeof(message), nullptr) == SQL_SUCCESS)
    {
        std::cerr << "SQLState: " << sqlState << " Message: " << message << std::endl;
    }
}

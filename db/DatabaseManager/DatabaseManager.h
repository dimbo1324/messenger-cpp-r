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

#pragma once
#include "../../Misc/Misc.h"
#include "MySQLAPI.h"
#if defined(_WIN64) || defined(_WIN32)
#include "ODBC.h"
#endif
#include <memory>

const extern char config_file[];

class DBClient
{
private:
    MySQLAPI mysqlapi;
#if defined(_WIN64) || defined(_WIN32)
    ODBC odbc;
#endif
    DBCore *_DBprovider;

public:
    DBCore *DBprovider();
    ~DBClient() = default;
    void initialise();
};

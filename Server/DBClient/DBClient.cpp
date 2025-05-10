#include "DBClient.h"

DBCore *DBClient::DBprovider()
{
    return _DBprovider;
}

void DBClient::initialise()
{
#if defined(_WIN64) || defined(_WIN32)
    if (Misc::getConfigValue(config_file, "DB", "db") == "mysqlapi")
    {
        _DBprovider = &mysqlapi;
        _DBprovider->provider = 0;
    }
    else
    {
        _DBprovider = &odbc;
        _DBprovider->provider = 1;
    }
#endif

#if defined(__linux__)
    _DBprovider = &mysqlapi;
#endif
}

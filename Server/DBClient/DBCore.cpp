#include "DBCore.h"

DBCore::DBCore() : server(Misc::getConfigValue(config_file, "DB", "server")),
                   port(Misc::getConfigValue(config_file, "DB", "port")),
                   dbuser(Misc::getConfigValue(config_file, "DB", "dbuser")),
                   dbpass(Misc::getConfigValue(config_file, "DB", "dbpass")),
                   dbname(Misc::getConfigValue(config_file, "DB", "dbname")),
                   db_character_set(Misc::getConfigValue(config_file, "DB", "db_character_set"))
{
    server = server.empty() ? "127.0.0.1" : server;
    port = port.empty() ? "3601" : port;
    dbuser = dbuser.empty() ? "root" : dbuser;
    dbpass = dbpass.empty() ? "pass" : dbpass;
    dbname = dbname.empty() ? "console_chat" : dbname;
    db_character_set = db_character_set.empty() ? "utf8mb4" : db_character_set;
}

void DBCore::hello()
{
    Misc::printMessage("Hello! Im SQL Base Class!");
}
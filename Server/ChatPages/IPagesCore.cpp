#include "IPagesCore.h"

IPagesCore::IPagesCore(char (&_cmd_buffer)[CMD_BUFFER], DBClient &_dbclient, std::shared_ptr<User> &authorizedUser)
    : cmd_buffer(_cmd_buffer), dbClient(_dbclient), AuthorizedUser(authorizedUser)
{

    login = buffer.getDynDataS(LOGIN_COUNT);
    session_key = buffer.getSessionKey();
    page_text = buffer.getDynDataS(PAGE_TEXT_COUNT);
    cmd_text = buffer.getDynDataS(CMD_TEXT_COUNT);
    pg_start = buffer.getPgStart();
    pg_per_page = buffer.getPgPerPage();
    pg_end = buffer.getUserInputCount();
    pg_mode = buffer.getPaginationMode();
    commands = Misc::stringExplode(cmd_text, ":");
    page_parsed = Misc::stringExplode(page_text, '\n');
    if (commands.size() == 0)
    {
        commands.push_back("none");
    }
    if (page_parsed.size() == 0)
    {
        page_parsed.push_back("none");
    }
}

std::string IPagesCore::getText()
{
    return data_text;
}

void IPagesCore::setAuthorizedUser(std::shared_ptr<User> &user)
{
    AuthorizedUser = user;
}

#include "ServerChatMap.h"

ServerChatMap::ServerChatMap()
{
    pages.insert("/chat");
    pages.insert("/private");
    pages.insert("/users");
    pages.insert("/profile");
}

std::string ServerChatMap::check(std::string page, std::string command)
{
    std::vector<std::string> v_page = Misc::stringExplode(page, '\n');
    std::vector<std::string> v_command = Misc::stringExplode(command, ":");
    if (v_command.size() > 0)
        command = v_command[0];
    if (v_page.size() > 0)
        page = v_page[0];

    if (pages.contains(command))
        return command;
    if (pages.contains(page))
        return page;
    return "none";
}

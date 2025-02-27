#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <iostream>
#include <thread>

#include "../enums/Section.h"
#include "./src/Logs/utils/parsers.h"

void loadFromTxt(std::unordered_map<std::string, std::shared_ptr<User>> &usersByLogin,
                 std::unordered_map<std::string, std::shared_ptr<User>> &usersByName,
                 std::vector<Message> &messages)
{
    std::ifstream txtFile("data.txt");
    if (!txtFile.is_open())
    {
        std::cerr << "Не удалось открыть файл data.txt для чтения." << std::endl;
        return;
    }

    Section currentSection = Section::None;
    std::string line;
    while (std::getline(txtFile, line))
    {
        if (line == "Users by Login:")
        {
            currentSection = Section::UsersByLogin;
        }
        else if (line == "Users by Name:")
        {
            currentSection = Section::UsersByName;
        }
        else if (line == "Messages:")
        {
            currentSection = Section::Messages;
        }
        else if (!line.empty())
        {
            if (currentSection == Section::UsersByLogin)
            {
                User user = parseUserByLogin(line);
                usersByLogin[user.login] = std::make_shared<User>(user);
            }
            else if (currentSection == Section::UsersByName)
            {
                auto [name, login] = parseUserByName(line);
                if (usersByLogin.find(login) != usersByLogin.end())
                {
                    usersByName[name] = usersByLogin[login];
                }
            }
            else if (currentSection == Section::Messages)
            {
                Message msg = parseMessage(line);
                messages.push_back(msg);
            }
        }
    }
    txtFile.close();
}
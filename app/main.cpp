#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>

#include "./src/Logs/enums/Section.h"
#include "./src/Logs/utils/parsers.h"

// Функция загрузки данных из файла .txt
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

// Функция вывода данных в консоль
void printData(const std::unordered_map<std::string, std::shared_ptr<User>> &usersByLogin,
               const std::unordered_map<std::string, std::shared_ptr<User>> &usersByName,
               const std::vector<Message> &messages)
{
    std::cout << "Users by Login:\n";
    for (const auto &pair : usersByLogin)
    {
        std::cout << "Login: " << pair.first << ", Name: " << pair.second->name
                  << ", Password: " << pair.second->password << "\n";
    }

    std::cout << "\nUsers by Name:\n";
    for (const auto &pair : usersByName)
    {
        std::cout << "Name: " << pair.first << ", Login: " << pair.second->login
                  << ", Password: " << pair.second->password << "\n";
    }

    std::cout << "\nMessages:\n";
    for (const auto &msg : messages)
    {
        std::cout << "From: " << msg.from << ", To: " << msg.to << ", Text: " << msg.text
                  << ", Timestamp: " << msg.timestamp << "\n";
    }
}

// Функция сохранения данных в файл .txt
void saveToTxt(const std::unordered_map<std::string, std::shared_ptr<User>> &usersByLogin,
               const std::unordered_map<std::string, std::shared_ptr<User>> &usersByName,
               const std::vector<Message> &messages)
{
    std::ofstream txtFile("data.txt");
    if (!txtFile.is_open())
    {
        std::cerr << "Не удалось открыть файл data.txt для записи." << std::endl;
        return;
    }

    txtFile << "Users by Login:\n";
    for (const auto &pair : usersByLogin)
    {
        txtFile << "Login: " << pair.first << ", Name: " << pair.second->name
                << ", Password: " << pair.second->password << "\n";
    }

    txtFile << "\nUsers by Name:\n";
    for (const auto &pair : usersByName)
    {
        txtFile << "Name: " << pair.first << ", Login: " << pair.second->login
                << ", Password: " << pair.second->password << "\n";
    }

    txtFile << "\nMessages:\n";
    for (const auto &msg : messages)
    {
        txtFile << "From: " << msg.from << ", To: " << msg.to << ", Text: " << msg.text
                << ", Timestamp: " << msg.timestamp << "\n";
    }

    txtFile.close();
}

int main()
{
    // Инициализация объектов
    std::unordered_map<std::string, std::shared_ptr<User>> usersByLogin = {
        {"ivan123", std::make_shared<User>(User{"ivan123", "Иван", "********"})},
        {"maria22", std::make_shared<User>(User{"maria22", "Мария", "********"})}};

    std::unordered_map<std::string, std::shared_ptr<User>> usersByName = {
        {"Иван", usersByLogin["ivan123"]},
        {"Мария", usersByLogin["maria22"]}};

    std::vector<Message> messages = {
        {"ivan123", "всем", "Привет, друзья!", "2023-05-20 14:32:45"},
        {"maria22", "ivan123", "Привет, Иван!", "2023-05-20 14:33:12"}};

    // Сохранение данных в файл
    saveToTxt(usersByLogin, usersByName, messages);

    // Очистка объектов
    usersByLogin.clear();
    usersByName.clear();
    messages.clear();

    // Задержка на 30 секунд
    std::this_thread::sleep_for(std::chrono::seconds(30));

    // Чтение данных из файла и заполнение тех же объектов
    loadFromTxt(usersByLogin, usersByName, messages);

    // Вывод данных в консоль
    printData(usersByLogin, usersByName, messages);

    return 0;
}
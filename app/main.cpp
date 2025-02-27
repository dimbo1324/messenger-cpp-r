#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <iostream>
#include <./include/nlohmann/json.hpp>

struct User
{
    std::string login;
    std::string name;
    std::string password;
};

struct Message
{
    std::string from;
    std::string to;
    std::string text;
    std::string timestamp;
};

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
        txtFile << "Login: " << pair.first << ", Name: " << pair.second->name << ", Password: " << pair.second->password << "\n";
    }

    txtFile << "\nUsers by Name:\n";
    for (const auto &pair : usersByName)
    {
        txtFile << "Name: " << pair.first << ", Login: " << pair.second->login << ", Password: " << pair.second->password << "\n";
    }

    txtFile << "\nMessages:\n";
    for (const auto &msg : messages)
    {
        txtFile << "From: " << msg.from << ", To: " << msg.to << ", Text: " << msg.text << ", Timestamp: " << msg.timestamp << "\n";
    }

    txtFile.close();
}

void saveToJson(const std::unordered_map<std::string, std::shared_ptr<User>> &usersByLogin,
                const std::unordered_map<std::string, std::shared_ptr<User>> &usersByName,
                const std::vector<Message> &messages)
{
    nlohmann::json j;

    // Сохраняем usersByLogin
    for (const auto &pair : usersByLogin)
    {
        j["usersByLogin"][pair.first] = {{"name", pair.second->name}, {"password", pair.second->password}};
    }

    // Сохраняем usersByName
    for (const auto &pair : usersByName)
    {
        j["usersByName"][pair.first] = {{"login", pair.second->login}, {"password", pair.second->password}};
    }

    // Сохраняем messages
    for (const auto &msg : messages)
    {
        j["messages"].push_back({{"from", msg.from}, {"to", msg.to}, {"text", msg.text}, {"timestamp", msg.timestamp}});
    }

    std::ofstream jsonFile("data.json");
    if (!jsonFile.is_open())
    {
        std::cerr << "Не удалось открыть файл data.json для записи." << std::endl;
        return;
    }

    jsonFile << j.dump(4); // Форматируем JSON с отступами для читаемости
    jsonFile.close();
}

void loadAndPrint()
{
    std::ifstream txtFile("data.txt");
    if (txtFile.is_open())
    {
        std::string line;
        while (std::getline(txtFile, line))
        {
            std::cout << line << std::endl;
        }
        txtFile.close();
        return;
    }

    std::ifstream jsonFile("data.json");
    if (jsonFile.is_open())
    {
        nlohmann::json j;
        jsonFile >> j;
        jsonFile.close();

        std::cout << "Users by Login:\n";
        for (const auto &user : j["usersByLogin"].items())
        {
            std::cout << "Login: " << user.key() << ", Name: " << user.value()["name"] << ", Password: " << user.value()["password"] << "\n";
        }

        std::cout << "\nUsers by Name:\n";
        for (const auto &user : j["usersByName"].items())
        {
            std::cout << "Name: " << user.key() << ", Login: " << user.value()["login"] << ", Password: " << user.value()["password"] << "\n";
        }

        std::cout << "\nMessages:\n";
        for (const auto &msg : j["messages"])
        {
            std::cout << "From: " << msg["from"] << ", To: " << msg["to"] << ", Text: " << msg["text"] << ", Timestamp: " << msg["timestamp"] << "\n";
        }
    }
    else
    {
        std::cerr << "Не удалось открыть ни один файл для чтения." << std::endl;
    }
}

int main()
{
    // Инициализация данных
    std::unordered_map<std::string, std::shared_ptr<User>> usersByLogin = {
        {"ivan123", std::make_shared<User>(User{"ivan123", "Иван", "********"})},
        {"maria22", std::make_shared<User>(User{"maria22", "Мария", "********"})}};

    std::unordered_map<std::string, std::shared_ptr<User>> usersByName = {
        {"Иван", usersByLogin["ivan123"]},
        {"Мария", usersByLogin["maria22"]}};

    std::vector<Message> messages = {
        {"ivan123", "всем", "Привет, друзья!", "2023-05-20 14:32:45"},
        {"maria22", "ivan123", "Привет, Иван!", "2023-05-20 14:33:12"}};

    // Сохранение данных в файлы
    saveToTxt(usersByLogin, usersByName, messages);
    saveToJson(usersByLogin, usersByName, messages);

    // Чтение и вывод данных
    loadAndPrint();

    return 0;
}
#include "Chat.h"
#include <iostream>
#include <algorithm>
#include <nlohmann/json.hpp>
#include "./utils/Input.h"
#include "./utils/Display.h"
#include "./utils/Time.h"
#include "./utils/Exceptions.h"

namespace ChatApp
{
    void Chat::Start()
    {
        _isActive = true;
    }

    std::shared_ptr<User> Chat::findUserByLogin(const std::string &login) const
    {
        auto it = _usersByLogin.find(login);
        return (it != _usersByLogin.end()) ? it->second : nullptr;
    }

    std::shared_ptr<User> Chat::findUserByName(const std::string &name) const
    {
        auto it = _usersByName.find(name);
        return (it != _usersByName.end()) ? it->second : nullptr;
    }

    void Chat::displayLoginMenu()
    {
        _currentUser.reset();
        char option = '\0';
        do
        {
            std::cout << "\n(1) Вход в чат\n(2) Регистрация нового пользователя\n(0) Выход из чата\n";
            std::cout << "Выберите действие: ";
            option = InputUtils::ReadOption();

            switch (option)
            {
            case '1':
                login();
                break;
            case '2':
                try
                {
                    signUp();
                }
                catch (const std::exception &ex)
                {
                    std::cout << ex.what() << std::endl;
                }
                break;
            case '0':
                _isActive = false;
                break;
            default:
                std::cout << "Выберите 1 или 2. Для выхода нажмите 0.\n";
                break;
            }
        } while (!_currentUser && _isActive);
    }

    void Chat::login()
    {
        std::string loginInput, passwordInput;
        char retryOption = '\0';

        do
        {
            std::cout << "Логин: ";
            std::cin >> loginInput;
            std::cout << "Пароль: ";
            std::cin >> passwordInput;

            auto user = findUserByLogin(loginInput);
            if (!user || (passwordInput != user->GetUserPassword()))
            {
                std::cout << "Ошибка входа.\nНажмите любую клавишу для повторной попытки или 0 для выхода: ";
                std::cin >> retryOption;
                if (retryOption == '0')
                    break;
            }
            else
            {
                _currentUser = user;
            }
        } while (!_currentUser);
    }

    void Chat::signUp()
    {
        std::string login, password, name;
        std::cout << "Придумайте логин: ";
        std::cin >> login;
        std::cout << "Придумайте пароль: ";
        std::cin >> password;
        std::cout << "Ваше имя: ";
        std::cin >> name;

        if (_usersByLogin.find(login) != _usersByLogin.end() || login == "всем")
        {
            throw UserLoginException();
        }
        if (_usersByName.find(name) != _usersByName.end() || name == "всем")
        {
            throw UserNameException();
        }
        auto newUser = std::make_shared<User>(login, password, name);
        _usersByLogin[login] = newUser;
        _usersByName[name] = newUser;
        _currentUser = newUser;
    }

    void Chat::displayChat() const
    {
        DisplayUtils::PrintSeparator("Чат");
        for (const auto &message : _messages)
        {
            if (_currentUser->GetUserLogin() == message.GetFrom() ||
                _currentUser->GetUserLogin() == message.GetTo() ||
                message.GetTo() == "всем")
            {
                std::string fromName = (_currentUser->GetUserLogin() == message.GetFrom())
                                           ? "я"
                                           : findUserByLogin(message.GetFrom())->GetUserName();

                std::string toName;
                if (message.GetTo() == "всем")
                {
                    toName = "всем";
                }
                else
                {
                    toName = (_currentUser->GetUserLogin() == message.GetTo())
                                 ? "я"
                                 : findUserByLogin(message.GetTo())->GetUserName();
                }

                std::string timestamp = TimeUtils::FormatTimestamp(
                    message.GetTimestamp());
                DisplayUtils::PrintMessage(fromName, toName, message.GetText(),
                                           timestamp);
            }
        }
    }

    void Chat::displayUserMenu()
    {
        char option = '\0';
        std::cout << "\nПривет, " << _currentUser->GetUserName() << "!\n";
        while (_currentUser)
        {
            std::cout << "\nМеню: Показать чат (1) | Новое сообщение (2) | Пользователи (3) | Выход (0)\n";
            std::cout << "Выберите действие: ";
            option = InputUtils::ReadOption();

            switch (option)
            {
            case '1':
                displayChat();
                break;
            case '2':
                addMessage();
                break;
            case '3':
                displayAllUserNames();
                break;
            case '0':
                _currentUser.reset();
                break;
            default:
                std::cout << "Пожалуйста, выберите из списка.\n";
                break;
            }
        }
    }

    void Chat::addMessage()
    {
        std::string recipient, text;
        std::cout << "Введите имя получателя или 'всем' для рассылки: ";
        std::cin >> recipient;
        std::cout << "Текст сообщения: ";
        std::cin.ignore();
        std::getline(std::cin, text);

        if (recipient != "всем" && !findUserByName(recipient))
        {
            std::cout << "Ошибка: не найден пользователь " << recipient << ".\n";
            return;
        }
        if (recipient == "всем")
        {
            _messages.emplace_back(_currentUser->GetUserLogin(), "всем", text);
        }
        else
        {
            _messages.emplace_back(_currentUser->GetUserLogin(),
                                   findUserByName(recipient)->GetUserLogin(), text);
        }
    }

    void Chat::displayAllUserNames() const
    {
        DisplayUtils::PrintSeparator("Пользователи");
        for (const auto &pair : _usersByLogin)
        {
            const auto &user = pair.second;
            std::cout << user->GetUserName();
            if (_currentUser->GetUserLogin() == user->GetUserLogin())
                std::cout << " (Я)";
            std::cout << "\n";
        }
    }

    void Chat::DataSaver() const
    {
        nlohmann::json j;

        for (const auto &pair : _usersByLogin)
        {
            j["users"].push_back({{"login", pair.first},
                                  {"name", pair.second->GetUserName()},
                                  {"password", pair.second->GetUserPassword()}});
        }

        for (const auto &msg : _messages)
        {
            j["messages"].push_back({{"from", msg.GetFrom()},
                                     {"to", msg.GetTo()},
                                     {"text", msg.GetText()},
                                     {"timestamp", TimeUtils::FormatTimestamp(msg.GetTimestamp())}});
        }

        std::ofstream outFile("chat_state.json");
        if (!outFile.is_open())
        {
            std::cerr << "Ошибка: не удалось открыть файл для сохранения состояния." << std::endl;
            return;
        }
        outFile << j.dump(4);
        outFile.close();
    }

    void Chat::LoaderMethod()
    {
        std::ifstream inFile("chat_state.json");
        if (!inFile.is_open())
        {
            return;
        }
        nlohmann::json j;
        inFile >> j;
        inFile.close();

        for (const auto &user : j["users"])
        {
            std::string login = user["login"];
            std::string name = user["name"];
            std::string password = user["password"];
            auto newUser = std::make_shared<User>(login, password, name);
            _usersByLogin[login] = newUser;
            _usersByName[name] = newUser;
        }

        for (const auto &msg : j["messages"])
        {
            std::string from = msg["from"];
            std::string to = msg["to"];
            std::string text = msg["text"];
            Message newMsg(from, to, text);
            _messages.push_back(newMsg);
        }
    }
}

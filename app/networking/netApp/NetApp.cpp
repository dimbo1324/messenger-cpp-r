#include "NetApp.h"
#include "../../app/src/User/User.h"
#include "../../app/src/Message/Message.h"
#include <iostream>
#include <cstring>
#include <chrono>
#include <thread>
#include <memory>
#include <stdexcept>


#if defined(_WIN32)
#include <winsock2.h>
#endif

namespace NetApp
{
#if defined(_WIN32)
    struct WinsockInitializer
    {
        WinsockInitializer()
        {
            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
            {
                throw std::runtime_error("WSAStartup failed");
            }
        }
        ~WinsockInitializer()
        {
            WSACleanup();
        }
    } winsockInit;
#endif

    Server::Server(unsigned short port)
        : port_(port), serverSocket_(INVALID_SOCKET_VALUE), running_(false)
    {
    }

    Server::~Server()
    {
        stop();
    }

    bool Server::start()
    {
        serverSocket_ = ::socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket_ == INVALID_SOCKET_VALUE)
        {
            throw std::runtime_error("Ошибка создания сокета");
        }

        sockaddr_in serverAddr;
        std::memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port_);
        serverAddr.sin_addr.s_addr = INADDR_ANY;

        if (::bind(serverSocket_, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR_VALUE)
        {
#if defined(_WIN32)
            ::closesocket(serverSocket_);
#else
            ::close(serverSocket_);
#endif
            throw std::runtime_error("Ошибка привязки сокета");
        }

        if (::listen(serverSocket_, SOMAXCONN) == SOCKET_ERROR_VALUE)
        {
#if defined(_WIN32)
            ::closesocket(serverSocket_);
#else
            ::close(serverSocket_);
#endif
            throw std::runtime_error("Ошибка прослушивания сокета");
        }

        running_ = true;
        std::cout << "Сервер запущен на порту " << port_ << "\n";
        clientThreads_.emplace_back(&Server::acceptClients, this);
        return true;
    }

    void Server::stop()
    {
        if (!running_)
            return;
        running_ = false;
#if defined(_WIN32)
        ::closesocket(serverSocket_);
#else
        ::close(serverSocket_);
#endif
        for (auto &t : clientThreads_)
        {
            if (t.joinable())
                t.join();
        }
        clientThreads_.clear();
        std::cout << "Сервер остановлен\n";
    }

    void Server::acceptClients()
    {
        while (running_)
        {
            sockaddr_in clientAddr;
            socklen_t clientAddrSize = sizeof(clientAddr);
            SocketType clientSocket = ::accept(serverSocket_, reinterpret_cast<sockaddr *>(&clientAddr), &clientAddrSize);
            if (clientSocket == INVALID_SOCKET_VALUE)
            {
                if (!running_)
                    break;
                throw std::runtime_error("Ошибка приема клиента");
            }
            std::cout << "Новый клиент подключился\n";
            clientThreads_.emplace_back(&Server::handleClient, this, clientSocket);
        }
    }

    void Server::handleClient(SocketType clientSocket)
    {
        char buffer[1024];
        while (running_)
        {
            std::memset(buffer, 0, sizeof(buffer));
            int bytesReceived = ::recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            if (bytesReceived > 0)
            {
                std::string request(buffer);
                std::string response = processRequest(clientSocket, request);
                if (::send(clientSocket, response.c_str(), response.size(), 0) == SOCKET_ERROR_VALUE)
                {
                    throw std::runtime_error("Ошибка отправки данных клиенту");
                }
            }
            else if (bytesReceived == 0)
            {
                std::cout << "Клиент отключился\n";
                handleDisconnect(clientSocket);
                break;
            }
            else
            {
                throw std::runtime_error("Ошибка получения данных");
            }
        }
#if defined(_WIN32)
        ::closesocket(clientSocket);
#else
        ::close(clientSocket);
#endif
    }

    std::string Server::processRequest(SocketType clientSocket, const std::string &request)
    {
        std::lock_guard<std::mutex> lock(dataMutex_);

        std::istringstream iss(request);
        std::string command;
        iss >> command;

        if (command == "LOGIN")
        {
            std::string login, password;
            iss >> login >> password;
            if (usersByLogin_.count(login) && usersByLogin_[login]->GetUserPassword() == password)
            {
                clientToUser_[clientSocket] = login;
                return "LOGIN_OK\n" + login;
            }
            return "ERROR\nInvalid login or password";
        }
        else if (command == "SIGNUP")
        {
            std::string login, password, name;
            iss >> login >> password >> name;
            if (usersByLogin_.count(login) || usersByName_.count(name))
            {
                return "ERROR\nUser already exists";
            }
            auto user = std::make_shared<ChatApp::User>(login, password, name);
            usersByLogin_[login] = user;
            usersByName_[name] = user;
            return "SIGNUP_OK";
        }
        else if (command == "GET_CHAT")
        {
            std::string response = "CHAT_MESSAGES\n";
            for (const auto &msg : messages_)
            {
                auto timestamp = std::chrono::system_clock::to_time_t(msg.GetTimestamp());
                response += std::to_string(timestamp) + " " + msg.GetFrom() + " " +
                            msg.GetTo() + " " + msg.GetText() + "\n";
            }
            return response;
        }
        else if (command == "SEND")
        {
            std::string recipient, text;
            iss >> recipient;
            std::getline(iss, text);
            if (clientToUser_.count(clientSocket) == 0)
                return "ERROR\nNot logged in";
            if (recipient != "всем" && usersByName_.count(recipient) == 0)
                return "ERROR\nRecipient not found";
            std::string sender = clientToUser_[clientSocket];
            messages_.emplace_back(sender, recipient, text);
            return "OK\nMessage sent";
        }
        else if (command == "GET_USERS")
        {
            std::string response = "USER_LIST\n";
            for (const auto &pair : usersByName_)
            {
                response += pair.first + "\n";
            }
            return response;
        }
        return "ERROR\nUnknown command";
    }

    void Server::handleDisconnect(SocketType clientSocket)
    {
        std::lock_guard<std::mutex> lock(dataMutex_);
        clientToUser_.erase(clientSocket);
    }
}

int main()
{
    try
    {
        NetApp::Server server(8080);
        server.start();
        std::cout << "Нажмите Enter для остановки сервера...\n";
        std::cin.get();
        server.stop();
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Исключение: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}

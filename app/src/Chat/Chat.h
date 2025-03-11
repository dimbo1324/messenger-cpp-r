#pragma once
#include <string>
#include <memory>
#include "../Message/Message.h"
#include "../User/User.h"
#include "../../networking/client/Client.h"

namespace ChatApp
{
    class Chat
    {
    private:
        bool _isActive = false;
        std::shared_ptr<User> _currentUser = nullptr;
        NetApp::Client client_;

        void login();
        void signUp();
        void displayChat() const;
        void displayAllUserNames() const;
        void addMessage();

    public:
        Chat(const std::string &serverAddress, unsigned short serverPort);
        ~Chat() = default;

        void Start();
        bool ChatIsActive() const { return _isActive; }
        std::shared_ptr<User> GetCurrentUser() const { return _currentUser; }
        void displayLoginMenu();
        void displayUserMenu();

        /*
        TODO:
      * По мере приближения к окончательной версии следует тщательно обдумать, как использовать эти методы:
      ? Оставить
      ? Удалить
      ? Переместить
      ? Переписать
      --------------------------------------------------------------------
      --------------------------------------------------------------------
        */
        void LoaderMethod();
        void DataSaver() const;
        /*
        --------------------------------------------------------------------
        --------------------------------------------------------------------
        */
    };
}
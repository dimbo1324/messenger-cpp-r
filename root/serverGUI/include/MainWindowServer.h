#pragma once

#include <QByteArray>
#include <QMainWindow>
#include <QString>
#include <QStringList>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindowServer;
}
QT_END_NAMESPACE

class QTcpSocket;

class MainWindowServer : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindowServer(QWidget *parent = nullptr);
    ~MainWindowServer();

private slots:
    void updateUsersFromServer();
    void updateMessagesFromServer();
    void handleToggleBan();
    void handleKickUser();
    void filterUsers();
    void filterMessages();
    void applyDarkTheme();
    void applyLightTheme();

private:
    Ui::MainWindowServer *ui;
    QTimer* updateTimer;
    QString serverStatus = "Disconnected";
    QString serverHost = "127.0.0.1";
    quint16 serverPort = 8080;
    QString adminLogin;
    QString adminPassword;

    void updateServerStatus(const QString& status);
    bool sendCommandToServer(const QString &command, QStringList *response = nullptr);
    bool sendFrame(QTcpSocket &socket, const QString &payload);
    bool readFrame(QTcpSocket &socket, QString *payload, int timeoutMs = 3000);
    bool loginForCommand(QTcpSocket &socket);
};

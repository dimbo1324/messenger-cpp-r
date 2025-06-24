#pragma once
#include <QMainWindow>
#include <QString>
#include <QStringList>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindowServer;
}
QT_END_NAMESPACE

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
    void updateServerStatus(const QString& status);
    // Возвращает ответ сервера в виде QStringList, или пустой список при ошибке
    bool sendCommandToServer(const QString &command, QStringList *response = nullptr);
};

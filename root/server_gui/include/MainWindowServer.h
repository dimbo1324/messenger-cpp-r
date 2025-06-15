#pragma once
#include <QMainWindow>

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

private:
    Ui::MainWindowServer *ui;
};

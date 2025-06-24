#include "MainWindowServer.h"
#include "ui_MainWindowServer.h"  // Сгенерируется, если всё ок

MainWindowServer::MainWindowServer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindowServer)
{
    ui->setupUi(this);
}

MainWindowServer::~MainWindowServer()
{
    delete ui;
}

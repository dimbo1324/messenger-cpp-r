#include <QApplication>
#include "MainWindowServer.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    MainWindowServer w;
    w.show();
    return app.exec();
}

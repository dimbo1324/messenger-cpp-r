#include <iostream>
#include "CommandLineInterface.h"
#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
using namespace std;

int main(int argc, char *argv[])
{
   QApplication a(argc, argv);
//	cout << "Usage:" << endl;
//	cout << "!login[username][password] - log in" << endl;
//	cout << "!logout - log out" << endl;
//	cout << "!register[username][password] - register new user" << endl;
//	cout << "!exit for exit" << endl;
//	cout << "!pm[(username)] message - write private message" << endl;
//	cout << "!getPM - receive private messages" << endl;
//  CommandLineInterface cli{};
//  cout << "chat ver. 0.1beta" << endl;
//  while (!cli->shouldExit())
//  {
//    cli->parseCommand();
//  }

  QTranslator myappTranslator;
  myappTranslator.load("translations/my_ru.qm");
  a.installTranslator(&myappTranslator);

  QTranslator defaultTranslator;
  defaultTranslator.load("translations/qt_ru.qm");
  a.installTranslator(&defaultTranslator);

  MainWindow::createClient();

  return a.exec();
}



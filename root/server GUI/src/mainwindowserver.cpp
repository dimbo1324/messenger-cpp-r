#include "MainWindowServer.h"
#include "ui_MainWindowServer.h"
#include <QTcpSocket>
#include <QDebug>
#include <QDateTime>


MainWindowServer::MainWindowServer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindowServer)
{
    ui->setupUi(this);

    ui->tableUsers->horizontalHeader()->setStretchLastSection(true);
    ui->tableMessages->horizontalHeader()->setStretchLastSection(true);

    connect(ui->buttonRefresh, &QPushButton::clicked, this, [this]() {
        updateUsersFromServer();
        ui->statusbar->showMessage("Обновлено!", 3000);
    });

    connect(ui->buttonRefreshMessages, &QPushButton::clicked, this, [this]() {
        updateMessagesFromServer();
        ui->statusbar->showMessage("Сообщения обновлены!", 3000);
    });

    connect(ui->buttonToggleBan, &QPushButton::clicked, this, [this]() {
        handleToggleBan();
    });

    connect(ui->buttonKickUser, &QPushButton::clicked, this, [this]() {
        handleKickUser();
    });

    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, [this]() {
        updateUsersFromServer();
    });
    updateTimer->start(5000);

    connect(ui->lineFilterLogin, &QLineEdit::textChanged,
            this, &MainWindowServer::filterUsers);

    connect(ui->comboStatusFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindowServer::filterUsers);

    connect(ui->lineFilterSender, &QLineEdit::textChanged,
            this, &MainWindowServer::filterMessages);

    connect(ui->lineFilterReceiver, &QLineEdit::textChanged,
            this, &MainWindowServer::filterMessages);

    connect(ui->lineFilterText, &QLineEdit::textChanged,
            this, &MainWindowServer::filterMessages);

    connect(ui->comboFilterDate, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindowServer::filterMessages);

    connect(ui->actionDarkTheme, &QAction::triggered,
            this, &MainWindowServer::applyDarkTheme);

    connect(ui->actionLightTheme, &QAction::triggered,
            this, &MainWindowServer::applyLightTheme);

}

MainWindowServer::~MainWindowServer()
{
    delete ui;
}

void MainWindowServer::updateUsersFromServer() {
#ifdef USE_STUB
    // Заглушка — тестовые данные
    ui->tableUsers->setRowCount(3);

    ui->tableUsers->setItem(0, 0, new QTableWidgetItem("alice"));
    ui->tableUsers->setItem(0, 1, new QTableWidgetItem("online"));
    ui->tableUsers->setItem(0, 2, new QTableWidgetItem("192.168.1.10"));
    ui->tableUsers->setItem(0, 3, new QTableWidgetItem("10:05"));

    ui->tableUsers->setItem(1, 0, new QTableWidgetItem("bob"));
    ui->tableUsers->setItem(1, 1, new QTableWidgetItem("offline"));
    ui->tableUsers->setItem(1, 2, new QTableWidgetItem("192.168.1.12"));
    ui->tableUsers->setItem(1, 3, new QTableWidgetItem("09:30"));

    ui->tableUsers->setItem(2, 0, new QTableWidgetItem("carol"));
    ui->tableUsers->setItem(2, 1, new QTableWidgetItem("online"));
    ui->tableUsers->setItem(2, 2, new QTableWidgetItem("192.168.1.20"));
    ui->tableUsers->setItem(2, 3, new QTableWidgetItem("11:15"));
#else
    QStringList responseLines;
    if (!sendCommandToServer("LIST", &responseLines)) {
        ui->statusbar->showMessage("Ошибка подключения к серверу");
        return;
    }
    if (responseLines.isEmpty() || !responseLines[0].startsWith("USERS ")) {
        ui->statusbar->showMessage("Ошибка получения списка пользователей");
        return;
    }

    QStringList tokens = responseLines[0].mid(6).split(' ', Qt::SkipEmptyParts);
    ui->tableUsers->setRowCount(tokens.size());

    for (int i = 0; i < tokens.size(); ++i) {
        ui->tableUsers->setItem(i, 0, new QTableWidgetItem(tokens[i]));
        ui->tableUsers->setItem(i, 1, new QTableWidgetItem("unknown")); // заглушка для статуса
        ui->tableUsers->setItem(i, 2, new QTableWidgetItem("-"));
        ui->tableUsers->setItem(i, 3, new QTableWidgetItem("-"));
    }
#endif
}

void MainWindowServer::filterUsers() {
    QString loginFilter = ui->lineFilterLogin->text().trimmed().toLower();
    QString statusFilter = ui->comboStatusFilter->currentText().toLower(); // "all", "online", "offline", "banned"

    for (int row = 0; row < ui->tableUsers->rowCount(); ++row) {
        QString login = ui->tableUsers->item(row, 0)->text().toLower();
        QString status = ui->tableUsers->item(row, 1)->text().toLower();

        bool matchesLogin = login.contains(loginFilter);
        bool matchesStatus = (statusFilter == "all") || (status == statusFilter);

        ui->tableUsers->setRowHidden(row, !(matchesLogin && matchesStatus));
    }
}



void MainWindowServer::updateMessagesFromServer() {
#ifdef USE_STUB
    // Заглушка — тестовые сообщения
    QList<QVector<QString>> messages = {
                                        {"alice", "all", "Hello everyone!", QDateTime::currentDateTime().addSecs(-3600).toString(Qt::ISODate)},
                                        {"bob", "alice", "Hi Alice!", QDateTime::currentDateTime().addSecs(-1800).toString(Qt::ISODate)},
                                        {"carol", "bob", "Secret message", QDateTime::currentDateTime().addSecs(-90000).toString(Qt::ISODate)},
                                        };

    ui->tableMessages->setRowCount(messages.size());
    ui->tableMessages->setColumnCount(4);
    ui->tableMessages->setHorizontalHeaderLabels(QStringList{"Sender", "Recipient", "Text", "Timestamp"});

    for (int i = 0; i < messages.size(); ++i) {
        ui->tableMessages->setItem(i, 0, new QTableWidgetItem(messages[i][0]));
        ui->tableMessages->setItem(i, 1, new QTableWidgetItem(messages[i][1]));
        ui->tableMessages->setItem(i, 2, new QTableWidgetItem(messages[i][2]));
        ui->tableMessages->setItem(i, 3, new QTableWidgetItem(messages[i][3]));
    }
#else
    QStringList responseLines;
    if (!sendCommandToServer("ALL_MESSAGES", &responseLines)) {
        ui->statusbar->showMessage("Ошибка подключения к серверу");
        return;
    }

    ui->tableMessages->clear();
    ui->tableMessages->setColumnCount(4);
    ui->tableMessages->setHorizontalHeaderLabels(QStringList{"Sender", "Recipient", "Text", "Timestamp"});
    ui->tableMessages->setRowCount(0);

    for (const QString& line : responseLines) {
        if (line.trimmed().isEmpty()) continue;

        QStringList parts = line.split('|');
        if (parts.size() < 4) continue;

        int row = ui->tableMessages->rowCount();
        ui->tableMessages->insertRow(row);
        ui->tableMessages->setItem(row, 0, new QTableWidgetItem(parts[0]));
        ui->tableMessages->setItem(row, 1, new QTableWidgetItem(parts[1]));
        ui->tableMessages->setItem(row, 2, new QTableWidgetItem(parts[3]));
        ui->tableMessages->setItem(row, 3, new QTableWidgetItem(parts[2]));
    }
#endif

    // Заполнение comboFilterDate уникальными датами (без времени)
    QSet<QString> uniqueDates;
    uniqueDates.insert("All");

    for (int row = 0; row < ui->tableMessages->rowCount(); ++row) {
        auto dateItem = ui->tableMessages->item(row, 3);
        if (dateItem) {
            QString isoDateTime = dateItem->text();
            QDateTime dt = QDateTime::fromString(isoDateTime, Qt::ISODate);
            if (dt.isValid()) {
                QString dateOnly = dt.date().toString("yyyy-MM-dd");
                uniqueDates.insert(dateOnly);
            }
        }
    }

    QStringList datesList = uniqueDates.values();
    datesList.removeOne("All");
    datesList.sort();
    datesList.prepend("All");

    ui->comboFilterDate->clear();
    ui->comboFilterDate->addItems(datesList);

    // Настройка размеров колонок (вынесена за пределы #ifdef)
    QHeaderView* header = ui->tableMessages->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents); // Sender
    header->setSectionResizeMode(1, QHeaderView::ResizeToContents); // Recipient
    header->setSectionResizeMode(2, QHeaderView::Stretch);          // Text
    //    header->setSectionResizeMode(3, QHeaderView::ResizeToContents); // Timestamp
    header->setSectionResizeMode(3, QHeaderView::Fixed);
    ui->tableMessages->setColumnWidth(3, 140);

}


void MainWindowServer::handleToggleBan() {
    int row = ui->tableUsers->currentRow();
    if (row < 0) return;

    QString username = ui->tableUsers->item(row, 0)->text();
    QString currentStatus = ui->tableUsers->item(row, 1)->text().toLower();

#ifdef USE_STUB
    QString newStatus;
    if (currentStatus == "banned") {
        newStatus = "offline"; // или "online", по ситуации
        ui->statusbar->showMessage("User " + username + " unbanned (stub)", 3000);
    } else {
        newStatus = "banned";
        ui->statusbar->showMessage("User " + username + " banned (stub)", 3000);
    }
    ui->tableUsers->setItem(row, 1, new QTableWidgetItem(newStatus));
#else
    QString cmd = (currentStatus == "banned" ? "UNBAN " : "BAN ") + username;
    QStringList response;
    if (sendCommandToServer(cmd, &response)) {
        if (!response.isEmpty() && response[0] == "OK") {
            QString newStatus = (currentStatus == "banned") ? "offline" : "banned";
            ui->tableUsers->setItem(row, 1, new QTableWidgetItem(newStatus));
            ui->statusbar->showMessage("User " + username + " " + (currentStatus == "banned" ? "unbanned" : "banned"), 3000);
        } else {
            ui->statusbar->showMessage("Server error: " + (response.isEmpty() ? "no response" : response[0]), 3000);
        }
    } else {
        ui->statusbar->showMessage("Failed to send command", 3000);
    }
#endif
}

void MainWindowServer::handleKickUser() {
    int row = ui->tableUsers->currentRow();
    if (row < 0) return;

    QString username = ui->tableUsers->item(row, 0)->text();

#ifdef USE_STUB
    // Заглушка — просто показываем сообщение и ставим статус offline
    ui->tableUsers->setItem(row, 1, new QTableWidgetItem("offline"));
    ui->statusbar->showMessage("User " + username + " kicked (stub)", 3000);
#else
    QString cmd = "KICK " + username;
    QStringList response;
    if (sendCommandToServer(cmd, &response)) {
        if (!response.isEmpty() && response[0] == "OK") {
            ui->tableUsers->setItem(row, 1, new QTableWidgetItem("offline"));
            ui->statusbar->showMessage("User " + username + " kicked", 3000);
        } else {
            ui->statusbar->showMessage("Server error: " + (response.isEmpty() ? "no response" : response[0]), 3000);
        }
    } else {
        ui->statusbar->showMessage("Failed to send command", 3000);
    }
#endif
}

void MainWindowServer::filterMessages() {
    QString senderFilter = ui->lineFilterSender->text().trimmed().toLower();
    QString receiverFilter = ui->lineFilterReceiver->text().trimmed().toLower();
    QString textFilter = ui->lineFilterText->text().trimmed().toLower();
    QString dateFilter = ui->comboFilterDate->currentText();

    for (int row = 0; row < ui->tableMessages->rowCount(); ++row) {
        auto senderItem = ui->tableMessages->item(row, 0);
        auto receiverItem = ui->tableMessages->item(row, 1);
        auto textItem = ui->tableMessages->item(row, 2);
        auto dateItem = ui->tableMessages->item(row, 3);

        QString sender = senderItem ? senderItem->text().toLower() : QString();
        QString receiver = receiverItem ? receiverItem->text().toLower() : QString();
        QString messageText = textItem ? textItem->text().toLower() : QString();
        QString messageDateStr = dateItem ? dateItem->text() : QString();

        bool matchesSender = sender.contains(senderFilter);
        bool matchesReceiver = receiver.contains(receiverFilter);
        bool matchesText = messageText.contains(textFilter);

        bool matchesDate = false;
        if (dateFilter == "All") {
            matchesDate = true;
        } else {
            QDate filterDate = QDate::fromString(dateFilter, "yyyy-MM-dd");
            QDateTime messageDate = QDateTime::fromString(messageDateStr, Qt::ISODate);
            matchesDate = filterDate.isValid() && messageDate.isValid() && (filterDate == messageDate.date());
        }

        bool showRow = matchesSender && matchesReceiver && matchesText && matchesDate;

        ui->tableMessages->setRowHidden(row, !showRow);
    }
}

void MainWindowServer::applyDarkTheme() {
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(30, 30, 30));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(20, 20, 20));
    darkPalette.setColor(QPalette::AlternateBase, QColor(35, 35, 35));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::black);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(40, 40, 40));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Highlight, QColor(100, 100, 255));
    darkPalette.setColor(QPalette::HighlightedText, Qt::white);
    qApp->setPalette(darkPalette);

    qApp->setStyleSheet(R"(
        QMainWindow {
            background-color: #1e1e1e;
            color: #ffffff;
        }

        QLabel, QCheckBox, QRadioButton, QGroupBox, QMenuBar, QMenu, QMenu::item {
            color: #ffffff;
        }

        QPushButton {
            background-color: #333;
            color: white;
            border: 1px solid #555;
            padding: 5px;
        }

        QPushButton:hover {
            background-color: #444;
        }

        QLineEdit, QComboBox, QPlainTextEdit, QTextEdit {
            background-color: #222;
            color: white;
            border: 1px solid #555;
        }

        QComboBox QAbstractItemView {
            background-color: #222;
            color: white;
        }

        QHeaderView::section {
            background-color: #2c2c2c;
            color: white;
            padding: 4px;
            border: 1px solid #444;
        }

        QTableWidget {
            background-color: #1e1e1e;
            color: white;
            gridline-color: #444;
        }

        QTableWidget QTableCornerButton::section {
            background-color: #2c2c2c;
        }

        QTableCornerButton::section {
            background-color: #2c2c2c;
            border: 1px solid #444;
        }

        QScrollBar:vertical, QScrollBar:horizontal {
            background-color: #2a2a2a;
        }

        QMenu {
            background-color: #2d2d2d;
            color: white;
        }

        QMenu::item:selected {
            background-color: #5050ff;
        }

        QMenuBar {
            background-color: #2d2d2d;
            color: white;
        }

        QMenuBar::item:selected {
            background-color: #5050ff;
        }

        QMenuBar::item:pressed {
            background-color: #3030a0;
        }

        QStatusBar {
            background-color: #1e1e1e;
            color: white;
        }
    )");
}

void MainWindowServer::applyLightTheme() {
    qApp->setPalette(style()->standardPalette());
    qApp->setStyleSheet(""); // сброс QSS
}

void MainWindowServer::updateServerStatus(const QString& status) {
    serverStatus = status;
    ui->statusbar->showMessage("Server status: " + serverStatus);
}



bool MainWindowServer::sendCommandToServer(const QString& cmd, QStringList* response)
{
    QTcpSocket socket;
    socket.connectToHost("127.0.0.1", 12345);

    if (!socket.waitForConnected(3000)) {
        qDebug() << "Ошибка подключения к серверу";
        updateServerStatus("Disconnected");
        return false;
    }

    updateServerStatus("Connected");

    QByteArray data = cmd.toUtf8() + '\n';
    socket.write(data);

    if (!socket.waitForBytesWritten(1000)) {
        qDebug() << "Ошибка отправки данных";
        updateServerStatus("Disconnected");
        return false;
    }

    if (response) {
        response->clear();
        while (socket.waitForReadyRead(1000)) {
            while (socket.bytesAvailable()) {
                QByteArray line = socket.readLine();
                if (!line.isEmpty())
                    response->append(QString::fromUtf8(line).trimmed());
            }
        }
    }

    socket.disconnectFromHost();
    return true;
}

#include "MainWindowServer.h"
#include "ui_MainWindowServer.h"

#include <QCoreApplication>
#include <QDate>
#include <QDateTime>
#include <QDebug>
#include <QHeaderView>
#include <QPalette>
#include <QSet>
#include <QTcpSocket>

namespace
{
    constexpr quint32 kMaxFrameSize = 64 * 1024;

    quint16 envPort(const char *name, quint16 fallback)
    {
        bool ok = false;
        const int value = qEnvironmentVariableIntValue(name, &ok);
        if (!ok || value <= 0 || value > 65535)
        {
            return fallback;
        }
        return static_cast<quint16>(value);
    }

    bool commandFinished(const QString &command, const QString &line)
    {
        if (command.startsWith("LIST"))
        {
            return line == "USERS_END";
        }
        if (command.startsWith("ALL_MESSAGES"))
        {
            return line == "ALL_MESSAGES_END";
        }
        return line == "OK" || line.startsWith("ERROR_") || line.startsWith("UNKNOWN_CMD");
    }
}

MainWindowServer::MainWindowServer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindowServer)
{
    ui->setupUi(this);

    serverHost = qEnvironmentVariable("CHAT_GUI_HOST", "127.0.0.1");
    serverPort = envPort("CHAT_GUI_PORT", 8080);
    adminLogin = qEnvironmentVariable("CHAT_GUI_ADMIN_LOGIN");
    adminPassword = qEnvironmentVariable("CHAT_GUI_ADMIN_PASSWORD");

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
    ui->tableUsers->setRowCount(3);
    ui->tableUsers->setItem(0, 0, new QTableWidgetItem("alice"));
    ui->tableUsers->setItem(0, 1, new QTableWidgetItem("online"));
    ui->tableUsers->setItem(0, 2, new QTableWidgetItem("user"));
    ui->tableUsers->setItem(0, 3, new QTableWidgetItem("10:05"));

    ui->tableUsers->setItem(1, 0, new QTableWidgetItem("bob"));
    ui->tableUsers->setItem(1, 1, new QTableWidgetItem("offline"));
    ui->tableUsers->setItem(1, 2, new QTableWidgetItem("moderator"));
    ui->tableUsers->setItem(1, 3, new QTableWidgetItem("09:30"));

    ui->tableUsers->setItem(2, 0, new QTableWidgetItem("carol"));
    ui->tableUsers->setItem(2, 1, new QTableWidgetItem("banned"));
    ui->tableUsers->setItem(2, 2, new QTableWidgetItem("user"));
    ui->tableUsers->setItem(2, 3, new QTableWidgetItem("11:15"));
#else
    QStringList responseLines;
    if (!sendCommandToServer("LIST", &responseLines)) {
        ui->statusbar->showMessage("Ошибка подключения к серверу");
        return;
    }

    ui->tableUsers->setRowCount(0);
    for (const QString &line : responseLines) {
        if (!line.startsWith("USER ")) {
            continue;
        }
        QStringList parts = line.split(' ', Qt::KeepEmptyParts);
        if (parts.size() < 4) {
            continue;
        }
        const int row = ui->tableUsers->rowCount();
        ui->tableUsers->insertRow(row);
        ui->tableUsers->setItem(row, 0, new QTableWidgetItem(parts.value(1)));
        ui->tableUsers->setItem(row, 1, new QTableWidgetItem(parts.value(2)));
        ui->tableUsers->setItem(row, 2, new QTableWidgetItem(parts.value(3)));
        ui->tableUsers->setItem(row, 3, new QTableWidgetItem(parts.value(4)));
    }
#endif
}

void MainWindowServer::filterUsers() {
    QString loginFilter = ui->lineFilterLogin->text().trimmed().toLower();
    QString statusFilter = ui->comboStatusFilter->currentText().toLower();

    for (int row = 0; row < ui->tableUsers->rowCount(); ++row) {
        const auto loginItem = ui->tableUsers->item(row, 0);
        const auto statusItem = ui->tableUsers->item(row, 1);
        const QString login = loginItem ? loginItem->text().toLower() : QString();
        const QString status = statusItem ? statusItem->text().toLower() : QString();

        const bool matchesLogin = login.contains(loginFilter);
        const bool matchesStatus = (statusFilter == "all") || (status == statusFilter);
        ui->tableUsers->setRowHidden(row, !(matchesLogin && matchesStatus));
    }
}

void MainWindowServer::updateMessagesFromServer() {
#ifdef USE_STUB
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
    if (!sendCommandToServer("ALL_MESSAGES 200", &responseLines)) {
        ui->statusbar->showMessage("Ошибка подключения к серверу");
        return;
    }

    ui->tableMessages->clear();
    ui->tableMessages->setColumnCount(4);
    ui->tableMessages->setHorizontalHeaderLabels(QStringList{"Sender", "Recipient", "Text", "Timestamp"});
    ui->tableMessages->setRowCount(0);

    for (const QString& line : responseLines) {
        if (!line.startsWith("MSG ")) {
            continue;
        }
        QStringList parts = line.split(' ', Qt::KeepEmptyParts);
        if (parts.size() < 5) {
            continue;
        }

        const int row = ui->tableMessages->rowCount();
        ui->tableMessages->insertRow(row);
        ui->tableMessages->setItem(row, 0, new QTableWidgetItem(parts.value(1)));
        ui->tableMessages->setItem(row, 1, new QTableWidgetItem(parts.value(2)));
        ui->tableMessages->setItem(row, 3, new QTableWidgetItem(parts.value(3)));
        ui->tableMessages->setItem(row, 2, new QTableWidgetItem(parts.mid(4).join(QStringLiteral(" "))));
    }
#endif

    QSet<QString> uniqueDates;
    uniqueDates.insert("All");

    for (int row = 0; row < ui->tableMessages->rowCount(); ++row) {
        auto dateItem = ui->tableMessages->item(row, 3);
        if (dateItem) {
            const QDateTime dt = QDateTime::fromString(dateItem->text(), Qt::ISODate);
            if (dt.isValid()) {
                uniqueDates.insert(dt.date().toString("yyyy-MM-dd"));
            }
        }
    }

    QStringList datesList = uniqueDates.values();
    datesList.removeOne("All");
    datesList.sort();
    datesList.prepend("All");

    ui->comboFilterDate->clear();
    ui->comboFilterDate->addItems(datesList);

    QHeaderView* header = ui->tableMessages->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(2, QHeaderView::Stretch);
    header->setSectionResizeMode(3, QHeaderView::Fixed);
    ui->tableMessages->setColumnWidth(3, 150);
}

void MainWindowServer::handleToggleBan() {
    int row = ui->tableUsers->currentRow();
    if (row < 0) return;

    QString username = ui->tableUsers->item(row, 0)->text();
    QString currentStatus = ui->tableUsers->item(row, 1)->text().toLower();

#ifdef USE_STUB
    QString newStatus = currentStatus == "banned" ? "offline" : "banned";
    ui->tableUsers->setItem(row, 1, new QTableWidgetItem(newStatus));
    ui->statusbar->showMessage("User " + username + " status changed (stub)", 3000);
#else
    const QString cmd = (currentStatus == "banned" ? "UNBAN " : "BAN ") + username;
    QStringList response;
    if (sendCommandToServer(cmd, &response) && !response.isEmpty() && response[0] == "OK") {
        ui->tableUsers->setItem(row, 1, new QTableWidgetItem(currentStatus == "banned" ? "offline" : "banned"));
        ui->statusbar->showMessage("User " + username + " updated", 3000);
    } else {
        ui->statusbar->showMessage("Server error: " + (response.isEmpty() ? "no response" : response[0]), 3000);
    }
#endif
}

void MainWindowServer::handleKickUser() {
    int row = ui->tableUsers->currentRow();
    if (row < 0) return;

    QString username = ui->tableUsers->item(row, 0)->text();

#ifdef USE_STUB
    ui->tableUsers->setItem(row, 1, new QTableWidgetItem("offline"));
    ui->statusbar->showMessage("User " + username + " kicked (stub)", 3000);
#else
    QStringList response;
    if (sendCommandToServer("KICK " + username, &response) && !response.isEmpty() && response[0] == "OK") {
        ui->tableUsers->setItem(row, 1, new QTableWidgetItem("offline"));
        ui->statusbar->showMessage("User " + username + " kicked", 3000);
    } else {
        ui->statusbar->showMessage("Server error: " + (response.isEmpty() ? "no response" : response[0]), 3000);
    }
#endif
}

void MainWindowServer::filterMessages() {
    const QString senderFilter = ui->lineFilterSender->text().trimmed().toLower();
    const QString receiverFilter = ui->lineFilterReceiver->text().trimmed().toLower();
    const QString textFilter = ui->lineFilterText->text().trimmed().toLower();
    const QString dateFilter = ui->comboFilterDate->currentText();

    for (int row = 0; row < ui->tableMessages->rowCount(); ++row) {
        const auto senderItem = ui->tableMessages->item(row, 0);
        const auto receiverItem = ui->tableMessages->item(row, 1);
        const auto textItem = ui->tableMessages->item(row, 2);
        const auto dateItem = ui->tableMessages->item(row, 3);

        const QString sender = senderItem ? senderItem->text().toLower() : QString();
        const QString receiver = receiverItem ? receiverItem->text().toLower() : QString();
        const QString messageText = textItem ? textItem->text().toLower() : QString();
        const QString messageDateStr = dateItem ? dateItem->text() : QString();

        const bool matchesSender = sender.contains(senderFilter);
        const bool matchesReceiver = receiver.contains(receiverFilter);
        const bool matchesText = messageText.contains(textFilter);

        bool matchesDate = dateFilter == "All";
        if (!matchesDate) {
            const QDate filterDate = QDate::fromString(dateFilter, "yyyy-MM-dd");
            const QDateTime messageDate = QDateTime::fromString(messageDateStr, Qt::ISODate);
            matchesDate = filterDate.isValid() && messageDate.isValid() && (filterDate == messageDate.date());
        }

        ui->tableMessages->setRowHidden(row, !(matchesSender && matchesReceiver && matchesText && matchesDate));
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
        QMainWindow { background-color: #1e1e1e; color: #ffffff; }
        QLabel, QCheckBox, QRadioButton, QGroupBox, QMenuBar, QMenu, QMenu::item { color: #ffffff; }
        QPushButton { background-color: #333; color: white; border: 1px solid #555; padding: 5px; }
        QPushButton:hover { background-color: #444; }
        QLineEdit, QComboBox, QPlainTextEdit, QTextEdit { background-color: #222; color: white; border: 1px solid #555; }
        QComboBox QAbstractItemView { background-color: #222; color: white; }
        QHeaderView::section { background-color: #2c2c2c; color: white; padding: 4px; border: 1px solid #444; }
        QTableWidget { background-color: #1e1e1e; color: white; gridline-color: #444; }
        QTableCornerButton::section { background-color: #2c2c2c; border: 1px solid #444; }
        QScrollBar:vertical, QScrollBar:horizontal { background-color: #2a2a2a; }
        QMenu { background-color: #2d2d2d; color: white; }
        QMenu::item:selected { background-color: #5050ff; }
        QMenuBar { background-color: #2d2d2d; color: white; }
        QMenuBar::item:selected { background-color: #5050ff; }
        QMenuBar::item:pressed { background-color: #3030a0; }
        QStatusBar { background-color: #1e1e1e; color: white; }
    )");
}

void MainWindowServer::applyLightTheme() {
    qApp->setPalette(style()->standardPalette());
    qApp->setStyleSheet("");
}

void MainWindowServer::updateServerStatus(const QString& status) {
    serverStatus = status;
    ui->statusbar->showMessage("Server status: " + serverStatus);
}

bool MainWindowServer::sendFrame(QTcpSocket &socket, const QString &payload)
{
    const QByteArray body = payload.toUtf8();
    if (body.size() > static_cast<int>(kMaxFrameSize)) {
        return false;
    }

    QByteArray frame;
    const quint32 size = static_cast<quint32>(body.size());
    frame.append(static_cast<char>((size >> 24) & 0xFF));
    frame.append(static_cast<char>((size >> 16) & 0xFF));
    frame.append(static_cast<char>((size >> 8) & 0xFF));
    frame.append(static_cast<char>(size & 0xFF));
    frame.append(body);

    socket.write(frame);
    return socket.waitForBytesWritten(3000);
}

bool MainWindowServer::readFrame(QTcpSocket &socket, QString *payload, int timeoutMs)
{
    QByteArray header;
    while (header.size() < 4) {
        if (!socket.bytesAvailable() && !socket.waitForReadyRead(timeoutMs)) {
            return false;
        }
        header.append(socket.read(4 - header.size()));
    }

    const auto b0 = static_cast<quint8>(header[0]);
    const auto b1 = static_cast<quint8>(header[1]);
    const auto b2 = static_cast<quint8>(header[2]);
    const auto b3 = static_cast<quint8>(header[3]);
    const quint32 size = (static_cast<quint32>(b0) << 24) |
                         (static_cast<quint32>(b1) << 16) |
                         (static_cast<quint32>(b2) << 8) |
                         static_cast<quint32>(b3);
    if (size > kMaxFrameSize) {
        return false;
    }

    QByteArray body;
    while (body.size() < static_cast<int>(size)) {
        if (!socket.bytesAvailable() && !socket.waitForReadyRead(timeoutMs)) {
            return false;
        }
        body.append(socket.read(static_cast<int>(size) - body.size()));
    }

    if (payload) {
        *payload = QString::fromUtf8(body);
    }
    return true;
}

bool MainWindowServer::loginForCommand(QTcpSocket &socket)
{
    if (adminLogin.isEmpty() || adminPassword.isEmpty()) {
        updateServerStatus("Missing CHAT_GUI_ADMIN_LOGIN/CHAT_GUI_ADMIN_PASSWORD");
        return false;
    }

    if (!sendFrame(socket, "LOGIN " + adminLogin + " " + adminPassword)) {
        return false;
    }

    QString response;
    if (!readFrame(socket, &response)) {
        return false;
    }
    if (!response.startsWith("LOGIN_OK")) {
        updateServerStatus("Admin login failed: " + response);
        return false;
    }
    return true;
}

bool MainWindowServer::sendCommandToServer(const QString& cmd, QStringList* response)
{
    QTcpSocket socket;
    socket.connectToHost(serverHost, serverPort);

    if (!socket.waitForConnected(3000)) {
        qDebug() << "Ошибка подключения к серверу";
        updateServerStatus("Disconnected");
        return false;
    }

    updateServerStatus("Connected");
    if (!loginForCommand(socket)) {
        socket.disconnectFromHost();
        return false;
    }

    if (!sendFrame(socket, cmd)) {
        updateServerStatus("Send failed");
        socket.disconnectFromHost();
        return false;
    }

    if (response) {
        response->clear();
        while (true) {
            QString line;
            if (!readFrame(socket, &line, 3000)) {
                break;
            }
            response->append(line);
            if (commandFinished(cmd, line)) {
                break;
            }
        }
    }

    sendFrame(socket, "LOGOUT");
    socket.disconnectFromHost();
    return true;
}

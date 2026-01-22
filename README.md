# C++ Messenger

[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17+-00599C?style=flat&logo=c%2B%2B&logoColor=white)](https://en.cppreference.com/w/cpp/17)
[![Qt Version](https://img.shields.io/badge/Qt-6+-41CD52?style=flat&logo=qt&logoColor=white)](https://www.qt.io/)
[![PostgreSQL](https://img.shields.io/badge/PostgreSQL-16.3-316192?style=flat&logo=postgresql&logoColor=white)](https://www.postgresql.org/)
[![License](https://img.shields.io/badge/License-Free_to_Use-green.svg)](LICENSE)
[![Maintenance](https://img.shields.io/badge/Maintained%3F-yes-green.svg)](https://github.com/dimbo1324)

> **A cross-platform messenger with client-server architecture, built using C++(17) and Qt6. Features console client/server, admin GUI, and PostgreSQL storage.**

---

### 🌐 Documentation / Документация / Documentación

* 🇷🇺 [**Russian (Русский)**](docs/descriptions/README_rus.md)
* 🇪🇸 [**Spanish (Español)**](docs/descriptions/README_sp.md)

---

## 📖 Overview

**C++ Messenger** is a production-ready template for building a secure, scalable messaging application. It follows a clean architecture, separating networking (TCP sockets), multithreading, database interactions, and UI components.

Whether you're learning C++ or developing a real-time chat system, this project provides essential infrastructure—cross-platform sockets, thread pooling, PostgreSQL integration, and a Qt6-based admin interface—so you can focus on adding features like encryption or media support.

### ✨ Key Features

* **Cross-Platform Support:** Works on Linux and Windows via abstractions for sockets (POSIX/WinSock) and threads.
* **Multithreaded Server:** Handles multiple connections using a ThreadPool for efficient task distribution.
* **Qt6 Admin GUI:** Monitors users, filters messages, manages bans/kicks, with dark/light themes.
* **PostgreSQL Storage:** Securely stores users, messages, logs, and online statuses using libpqxx.
* **Console Client/Server:** Registration, login, messaging, user lists, and history viewing.
* **Reusable Libraries:** `tcp` for sockets and `threading` for pooling and logging.
* **Extensible Design:** Easy to add encryption, media files, push notifications, or unit tests.

---

## 🛠️ Tech Stack

* **Language:** [C++ (17)](https://en.cppreference.com/w/cpp/17)
* **UI Framework:** [Qt6](https://www.qt.io/)
* **Database:** [PostgreSQL](https://www.postgresql.org/)
* **Database Driver:** [libpqxx](https://github.com/jtv/libpqxx)
* **Build System:** [CMake](https://cmake.org/)
* **Networking:** POSIX sockets / WinSock
* **Multithreading:** std::thread / std::mutex
* **Other Tools:** pkg-config (for Linux/macOS), Git

---

## 🚀 Getting Started

Follow these steps to get a local copy up and running. This guide is designed for beginners—even if you're new to IT, we'll explain each step clearly.

### Prerequisites

Before starting, install these tools. Download links are provided.

* **Git**: To clone the project. Download from [git-scm.com](https://git-scm.com/downloads). Install and add to your PATH.
* **CMake**: Version 3.21 or higher. Download from [cmake.org](https://cmake.org/download/). Install and add to PATH.
* **C++ Compiler**:
  - **Windows**: Visual Studio 2019/2022 (Community edition is free). Download from [visualstudio.microsoft.com](https://visualstudio.microsoft.com/downloads). Select "Desktop development with C++" workload.
  - **Linux (Ubuntu/Debian)**: Install GCC/Clang via terminal: `sudo apt update && sudo apt install g++ cmake`.
  - **macOS**: Install Xcode from App Store, then Command Line Tools: `xcode-select --install`.
* **Qt6**: Version 6 or higher (for GUI). Download the installer from [qt.io/download](https://www.qt.io/download-qt-online-installer). Install the "Desktop" components for your platform (e.g., msvc2019_64 on Windows).
* **PostgreSQL**: Database server. Download from [postgresql.org/download](https://www.postgresql.org/download/). Install and remember the superuser password (default: "postgres").
* **libpqxx**: C++ library for PostgreSQL.
  - **Linux**: `sudo apt install libpqxx-dev`.
  - **macOS**: `brew install libpqxx` (install Homebrew first: `/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"`).
  - **Windows**: Download source from [github.com/jtv/libpqxx](https://github.com/jtv/libpqxx), build with CMake, or use vcpkg: Install vcpkg, then `vcpkg install libpqxx:x64-windows`.

Test installations:
- Open a terminal/command prompt and run: `git --version`, `cmake --version`, `g++ --version` (or `cl` on Windows), `psql --version`.

### 1. Clone the Repository

Open a terminal/command prompt and run:

```bash
git clone https://github.com/dimbo1324/c-plus-plus-messenger.git
cd c-plus-plus-messenger
```

This downloads the project to your computer.

### 2. Environment Configuration

The project uses environment variables for customization. Set them in your terminal before building/running.

| Variable           | Description                          | Default Value                                                                  |
| ------------------ | ------------------------------------ | ------------------------------------------------------------------------------ |
| `CHAT_DB_CONN`     | Database connection string           | `host=localhost port=5432 dbname=chat_db user=postgres password=your_password` |
| `QT_PATH`          | Path to Qt6 installation (for CMake) | (Set if not in PATH, e.g., `C:/Qt/6.5.3/msvc2019_64`)                          |
| `CMAKE_BUILD_TYPE` | Build type (Release/Debug)           | `Release`                                                                      |

On Windows: `set CHAT_DB_CONN=host=localhost port=5432 dbname=chat_db user=postgres password=your_password`

On Linux/macOS: `export CHAT_DB_CONN="host=localhost port=5432 dbname=chat_db user=postgres password=your_password"`

Replace `your_password` with your PostgreSQL password.

### 3. Start the Database

Start PostgreSQL service:
- **Windows**: Search "Services" in Start menu, find "postgresql-x64-16", right-click > Start.
- **Linux**: `sudo systemctl start postgresql`
- **macOS**: `brew services start postgresql`

Verify: Run `psql -U postgres` and enter your password. Type `\q` to exit.

### 4. Database Migration

Create the database and tables.

1. Open psql: `psql -U postgres`
2. Create database: `CREATE DATABASE chat_db;`
3. Connect: `\c chat_db`
4. Run the SQL script from `root/app/database/init.sql`. Copy-paste the contents into psql, or run: `psql -U postgres -d chat_db -f root/app/database/init.sql`
5. (Optional) Add sample data: `psql -U postgres -d chat_db -f root/app/database/sample_data.sql`

Create a dedicated user for security:
- In psql: `CREATE USER chat_user WITH PASSWORD 'secure_password';`
- Grant access: `GRANT CONNECT ON DATABASE chat_db TO chat_user; \c chat_db GRANT USAGE ON SCHEMA public TO chat_user; GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA public TO chat_user;`

Update `CHAT_DB_CONN` to use `chat_user` and `secure_password`.

### 5. Build the Project

Create a build folder to keep things clean:

```bash
mkdir build
cd build
```

Configure CMake:
- **Windows** (in "x64 Native Tools Command Prompt for VS"):
  ```bat
  cmake -G "Visual Studio 16 2019" -A x64 -DQT_PATH="C:/Qt/6.5.3/msvc2019_64" ..
  ```
- **Linux/macOS**:
  ```bash
  cmake -DCMAKE_BUILD_TYPE=Release ..
  ```

Build:
```bash
cmake --build . --config Release
```

This creates executables: `chat_client`, `chat_server`, `server_gui`.

---

## 📂 Project Structure

The project follows a modular layout:

```text
c-plus-plus-messenger/
├── .gitignore
├── LICENSE
├── README.md
├── assets/                   # Resources (e.g., images for GUI)
│   ├── 1.png
│   └── ...
├── CMakeLists.txt            # Root CMake: Includes all subprojects
└── root/
    ├── app/
    │   ├── client/           # Console client
    │   │   ├── CMakeLists.txt
    │   │   ├── include/      # Client headers
    │   │   │   ├── Client.h
    │   │   │   └── UI.h
    │   │   └── src/          # Client sources
    │   │       ├── main.cpp
    │   │       ├── Client.cpp
    │   │       └── UI.cpp
    │   ├── server/           # Console server
    │   │   ├── CMakeLists.txt
    │   │   ├── include/      # Server headers
    │   │   │   ├── ClientHandler.h
    │   │   │   ├── Database.h
    │   │   │   ├── Logger.h
    │   │   │   └── Server.h
    │   │   └── src/          # Server sources
    │   │       ├── main.cpp
    │   │       ├── Server.cpp
    │   │       ├── ClientHandler.cpp
    │   │       ├── Database.cpp
    │   │       └── Logger.cpp
    │   ├── libs/             # Shared libraries
    │   │   ├── tcp/          # TCP sockets abstraction
    │   │   │   ├── CMakeLists.txt
    │   │   │   ├── include/
    │   │   │   │   └── tcp/
    │   │   │   │       ├── ISocket.h
    │   │   │   │       ├── SocketFactory.h
    │   │   │   │       ├── TcpSocketLinux.h
    │   │   │   │       └── TcpSocketWin.h
    │   │   │   └── src/
    │   │   │       ├── SocketFactory.cpp
    │   │   │       ├── TcpSocketLinux.cpp
    │   │   │       └── TcpSocketWin.cpp
    │   │   └── threading/    # ThreadPool and Logger
    │   │       ├── CMakeLists.txt
    │   │       ├── include/
    │   │       │   ├── threading_config.h
    │   │       │   └── threading/
    │   │       │       ├── Logger.h
    │   │       │       └── ThreadPool.h
    │   │       └── src/
    │   │           ├── Logger.cpp
    │   │           └── ThreadPool.cpp
    │   ├── database/         # SQL scripts
    │   │   ├── init.sql
    │   │   ├── sample_data.sql
    │   │   └── README.md
    │   └── logs/             # Server logs
    │       └── user_activity.log
    └── serverGUI/            # Qt6 admin GUI
        ├── CMakeLists.txt
        ├── include/
        │   └── MainWindowServer.h
        ├── src/
        │   ├── main.cpp
        │   └── mainwindowserver.cpp
        └── ui/
            └── MainWindowServer.ui
```

---

## 🔌 Usage

### Server

```bash
cd build/root/app/server/Release  # or Debug
./chat_server 8080
```

Output: "Server listening on port 8080". Logs in `../logs/user_activity.log`.

### Client

```bash
cd build/root/app/client/Release
./chat_client 127.0.0.1:8080
```

Follow menu: Register/Login, then send messages, view lists/history.

### Admin GUI

```bash
cd build/root/serverGUI/Release
./server_gui
```

Refresh users/messages, filter, ban/kick. Uses stubs by default (edit CMake to disable).

---

## 🤝 Contributing

Contributions are what make the open-source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

---

## 📜 License

This project is free to use.

---

## 📬 Contact

If you have questions, suggestions, or just want to say hi, feel free to reach out!

* **Author:** dimbo1324
* **Telegram:** [@dimbo1324](https://t.me/dimbo1324)
* **Email:** dimaprihodko180@gmail.com
* **GitHub:** [github.com/dimbo1324](https://github.com/dimbo1324)

---

*Developed with ❤️ by dimbo1324*
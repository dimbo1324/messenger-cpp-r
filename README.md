# C++ Messenger

C++ Messenger — это кроссплатформенный мессенджер с клиент-серверной архитектурой, реализованный на C++(17). Проект включает:

- **Консольный клиент**: регистрация, вход, отправка/получение сообщений, просмотр онлайн-пользователей и истории.
- **Консольный сервер**: многопоточный обработчик подключений, хранение данных в PostgreSQL, логирование.
- **Qt6 GUI для администратора сервера**: мониторинг пользователей, фильтрация сообщений, управление (бан/кик).
- **Библиотеки TCP-сокетов и многопоточности** для кроссплатформенной работы (Linux/Windows).
- **PostgreSQL + libpqxx** для надёжного хранения пользователей, сообщений, логов, статусов.

_Дальнейшее развитие проекта: шифрование, медиа, масштабирование. Также можно добавить push-уведомления, конфигурацию, тестирование и т. д._

---

## Содержание README

1. [Особенности](#особенности)
2. [Требования](#требования)
3. [Подготовка окружения](#подготовка-окружения)

   - [Установка PostgreSQL и libpqxx](#установка-postgresql-и-libpqxx)
   - [Установка Qt6](#установка-qt6)
   - [Установка компилятора и CMake](#установка-компилятора-и-cmake)

4. [Структура проекта](#структура-проекта)
5. [Сборка проекта](#сборка-проекта)

   - [Общая схема out-of-source сборки](#общая-схема-out-of-source-сборки)
   - [Параметры CMake](#параметры-cmake)
   - [Сборка на Windows](#сборка-на-windows)
   - [Сборка на Linux/macOS](#сборка-на-linuxmacos)

6. [Конфигурация базы данных](#конфигурация-базы-данных)

   - [Инициализация схемы и таблиц](#инициализация-схемы-и-таблиц)
   - [Создание пользователя БД и права доступа](#создание-пользователя-бд-и-права-доступа)
   - [Безопасность хранения паролей](#безопасность-хранения-паролей)

7. [Запуск и использование](#запуск-и-использование)

   - [Запуск сервера](#запуск-сервера)
   - [Запуск клиента](#запуск-клиента)
   - [Запуск GUI сервера](#запуск-gui-сервера)

8. [Логирование и рабочие директории](#логирование-и-рабочие-директории)
9. [Настройка и расширение](#настройка-и-расширение)

   - [Параметры сервера и пул потоков](#параметры-сервера-и-пул-потоков)
   - [USE_STUB в GUI](#use_stub-в-gui)
   - [Шифрование, хэширование паролей и TLS](#шифрование-хэширование-паролей-и-tls)
   - [Международная поддержка и мультиязычность](#международная-поддержка-и-мультиязычность)
   - [Тестирование и CI/CD](#тестирование-и-ci-cd)

10. [Статьи и обзорные руководства](#статьи-и-обзорные-руководства)
11. [Репозитории GitHub (реальный код)](#репозитории-github-реальный-код)
12. [Видео и плейлисты YouTube](#видео-и-плейлисты-youtube)
13. [Документация и официальные справочники](#документация-и-официальные-справочники)
14. [Как использовать эти ресурсы](#как-использовать-эти-ресурсы)
15. [Контакты и поддержка](#контакты-и-поддержка)
16. [Лицензия](#лицензия)

---

## Особенности

- **Кроссплатформенность**: Linux и Windows поддерживаются через абстракции сокетов (POSIX vs WinSock) и потоков.
- **Многопоточность**: сервер обрабатывает подключения через ThreadPool, задачи распределяются по потокам.
- **Qt6 GUI для администратора**: удобное табличное отображение пользователей и сообщений, фильтрация, тема (светлая/тёмная), операции бан/кик.
- **PostgreSQL + libpqxx**: надёжное хранение пользователей, сообщений, онлайн-статусов и логов.
- **Консистентная архитектура**: разделение на чистый C++ клиент/сервер, GUI на Qt, повторно используемые библиотеки tcp и threading.
- **Расширяемость**: легко добавить шифрование, хранение медиа, push-уведомления, конфигурацию, тестирование и т. д.
- **Простота запуска**: подробные инструкции для новичков.

---

## Требования

1. **CMake** ≥ 3.21
2. **Компилятор C++17**

   - Linux: GCC ≥ 9 или Clang ≥ 10
   - Windows: MSVC из Visual Studio 2019/2022 (x64)

3. **Qt6** (для сборки GUI)
4. **PostgreSQL** (сервер базы данных)
5. **libpqxx** (C++ API для PostgreSQL)
6. **pkg-config** (на Linux/macOS) для поиска libpqxx. На Windows возможна ручная настройка include/lib.
7. Сетевые библиотеки ОС: `<sys/socket.h>`, WinSock.
8. **Git** (для клонирования репозитория).

---

## Подготовка окружения

Ниже приведены детальные шаги для разных ОС. Следуйте шаг за шагом, даже если вы новичок.

### Установка PostgreSQL и libpqxx

#### Linux (Debian/Ubuntu)

1. Обновите индексы пакетов:

   ```bash
   sudo apt update
   ```

2. Установите PostgreSQL и dev-пакеты:

   ```bash
   sudo apt install postgresql postgresql-contrib libpqxx-dev pkg-config
   ```

3. Убедитесь, что служба PostgreSQL запущена:

   ```bash
   sudo systemctl status postgresql
   ```

4. Проверка libpqxx:

   ```bash
   pkg-config --modversion libpqxx
   ```

   Если выводит версию (например, `7.6.0`), значит libpqxx доступен.

#### macOS (Homebrew)

1. Установите Homebrew, если ещё нет: [https://brew.sh](https://brew.sh)
2. Установите PostgreSQL и libpqxx:

   ```bash
   brew update
   brew install postgresql libpqxx pkg-config
   ```

3. Запустите PostgreSQL (при необходимости):

   ```bash
   brew services start postgresql
   ```

4. Проверьте:

   ```bash
   pkg-config --modversion libpqxx
   ```

#### Windows

1. Установите PostgreSQL через официальный установщик:

   - [https://postgrespro.ru/windows](https://postgrespro.ru/windows)
     Следуйте инструкциям установщика, запомните учётные данные администратора (postgres).

2. Установите libpqxx:

   - Либо скачайте и соберите из исходников (см. документацию libpqxx).
   - Либо, если используете MSYS2, в MSYS2 установите пакет `mingw-w64-x86_64-libpqxx`.

3. Убедитесь, что include-папка libpqxx и библиотека (.lib/.dll) доступны:

   - Если pkg-config доступен (через MSYS2), CMake найдёт libpqxx автоматически.
   - В противном случае придётся вручную задать пути в CMakeLists (см. ниже).

4. Запуск службы PostgreSQL:

   - Служба запускается автоматически или вручную через «Службы Windows».

### Установка Qt6

#### Windows

1. Скачайте и установите Qt6 через Qt Installer (официальный сайт).
2. Запомните путь установки, например: `C:/Qt/6.5.3/msvc2019_64`.
3. При запуске CMake указывайте:

   ```bat
   cmake -DQT_PATH="C:/Qt/6.5.3/msvc2019_64" ..
   ```

   или экспортируйте в окружении:

   ```bat
   set CMAKE_PREFIX_PATH=C:/Qt/6.5.3/msvc2019_64;%CMAKE_PREFIX_PATH%
   ```

#### Linux

1. Установите через пакетный менеджер или из официального релиза:

   ```bash
   sudo apt install qt6-base-dev qt6-tools-dev
   ```

2. Если Qt установлен в нестандартном месте, задайте `CMAKE_PREFIX_PATH`:

   ```bash
   export CMAKE_PREFIX_PATH=/path/to/Qt/6.5.3/gcc_64:$CMAKE_PREFIX_PATH
   ```

#### macOS

1. Установите через Homebrew (`brew install qt`) или скачайте с официального сайта.
2. Задайте `CMAKE_PREFIX_PATH`, если необходимо:

   ```bash
   export CMAKE_PREFIX_PATH=/usr/local/opt/qt6:$CMAKE_PREFIX_PATH
   ```

### Установка компилятора и CMake

- **Linux/macOS**: убедитесь, что `g++` или `clang++` и `cmake` установлены:

  ```bash
  g++ --version
  cmake --version
  ```

- **Windows**:

  - Откройте терминал «x64 Native Tools Command Prompt for VS» (если MSVC).
  - Убедитесь, что `cl` доступен:

    ```
    cl
    ```

  - `cmake` должен быть в PATH (обычно устанавливается вместе с CMake Installer или в составе Visual Studio).

---

## Структура проекта

Ниже дерево каталогов относительно корня репозитория `c-plus-plus-messenger`. Пояснения для новичков: каждая папка содержит свой CMakeLists.txt (или SQL-скрипты), а структура логически разделена.

```
c-plus-plus-messenger/
├── .gitignore
├── LICENSE
├── README.md
├── assets/                   # Ресурсы (например, изображения). При необходимости могут использоваться GUI.
│   ├── 1.png
│   ├── 2.png
│   └── ...
├── CMakeLists.txt            # Корневой: подключает все подпроекты
└── root/
    ├── app/
    │   ├── client/           # Консольный клиент
    │   │   ├── CMakeLists.txt
    │   │   ├── include/      # Заголовки клиента
    │   │   │   ├── Client.h
    │   │   │   └── UI.h
    │   │   └── src/          # Исходники клиента
    │   │       ├── main.cpp
    │   │       ├── Client.cpp
    │   │       └── UI.cpp
    │   ├── server/           # Консольный сервер
    │   │   ├── CMakeLists.txt
    │   │   ├── include/      # Заголовки сервера
    │   │   │   ├── ClientHandler.h
    │   │   │   ├── Database.h
    │   │   │   ├── Logger.h
    │   │   │   └── Server.h
    │   │   └── src/          # Исходники сервера
    │   │       ├── main.cpp
    │   │       ├── Server.cpp
    │   │       ├── ClientHandler.cpp
    │   │       ├── Database.cpp
    │   │       └── Logger.cpp
    │   ├── libs/             # Вспомогательные библиотеки
    │   │   ├── tcp/          # TCP-сокеты, абстракция ISocket
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
    │   │   └── threading/    # ThreadPool и Logger
    │   │       ├── CMakeLists.txt
    │   │       ├── include/
    │   │       │   ├── threading_config.h
    │   │       │   └── threading/
    │   │       │       ├── Logger.h
    │   │       │       └── ThreadPool.h
    │   │       └── src/
    │   │           ├── Logger.cpp
    │   │           └── ThreadPool.cpp
    │   ├── database/         # SQL-скрипты для инициализации БД
    │   │   ├── init.sql      # Создание схемы и таблиц
    │   │   ├── sample_data.sql
    │   │   └── README.md     # Краткое описание БД (опционально)
    │   └── logs/             # Логи сервера
    │       └── user_activity.log  # создаётся при запуске сервера (строка: "../logs/user_activity.log")
    └── serverGUI/            # Qt6 GUI для администрирования сервера
        ├── CMakeLists.txt
        ├── include/
        │   └── MainWindowServer.h
        ├── src/
        │   ├── main.cpp
        │   └── mainwindowserver.cpp
        └── ui/
            └── MainWindowServer.ui
```

> **Совет новичкам**: внимательно следуйте структуре, не меняйте расположение без корректировки CMakeLists.txt. Если добавляете новые модули, создавайте свои подпроекты и подключайте в корневом CMakeLists.

---

## Сборка проекта

Ниже шаги для максимально подробного руководства. Если вы ранее не работали с CMake или Qt, следуйте внимательно, чтобы избежать ошибок.

### Общая схема out-of-source сборки

1. Перейдите в корень клонированного репозитория:

   ```bash
   cd c-plus-plus-messenger
   ```

2. Создайте отдельный каталог для сборки:

   ```bash
   mkdir build
   cd build
   ```

   Это помогает держать исходники чистыми и позволяет легко удалять/пересоздавать сборку.

3. Запустите команду `cmake`, указывая параметры:

   - `-DQT_PATH="..."` (для Windows или нестандартных установок Qt).
   - Другие опции, если необходимо (например, для libpqxx на Windows).

   ```bash
   cmake -DQT_PATH="C:/Qt/6.5.3/msvc2019_64" ..
   ```

   или на Linux/macOS, если Qt и libpqxx установлены в стандартных местах:

   ```bash
   cmake ..
   ```

4. После успешной конфигурации запустите сборку:

   ```bash
   cmake --build . --config Release
   ```

   или просто `cmake --build .` (в зависимости от генератора).
   Для отладочной сборки укажите `--config Debug` (особенно на Windows/Visual Studio).

5. При успешной сборке вы увидите соответствующие таргеты:

   - `chat_client` (консольный клиент)
   - `chat_server` (консольный сервер)
   - `server_gui` (Qt GUI для сервера)

Пути к сгенерированным бинарям обычно находятся в поддиректориях `build/root/app/...` или рядом с CMake корнем, в зависимости от конфигурации. Проверьте вывод CMake, где именно создаются исполняемые файлы.

### Параметры CMake

- **QT_PATH**: путь к установке Qt6. Задавайте через `-DQT_PATH="путь"` или экспортируйте `CMAKE_PREFIX_PATH`.
- **CMAKE_BUILD_TYPE** (Linux/macOS): например, `-DCMAKE_BUILD_TYPE=Release` или Debug.
- **На Windows**: чаще указывают генератор:

  ```bat
  cmake -G "Visual Studio 16 2019" -A x64 -DQT_PATH="C:/Qt/6.5.3/msvc2019_64" ..
  ```

- **libpqxx**: на Linux/macOS CMake найдёт через pkg-config. На Windows, если pkg-config отсутствует, можно добавить в CMakeLists проверку `if(WIN32)` и вручную задать:

  ```cmake
  find_path(PQXX_INCLUDE_DIRS "C:/path/to/libpqxx/include")
  find_library(PQXX_LIBRARIES NAMES pqxx PATHS "C:/path/to/libpqxx/lib")
  if(PQXX_INCLUDE_DIRS AND PQXX_LIBRARIES)
      message(STATUS "Found libpqxx: ${PQXX_LIBRARIES}")
      target_include_directories(server_core PUBLIC ${PQXX_INCLUDE_DIRS})
      target_link_libraries(server_core PRIVATE ${PQXX_LIBRARIES})
  else()
      message(FATAL_ERROR "libpqxx not found, set PQXX paths")
  endif()
  ```

- **USE_STUB**: по умолчанию GUI собирается с `USE_STUB`, т. е. использует заглушки. Если вы хотите собирать GUI без заглушек, в корневом CMakeLists уберите или условно задавайте `-DUSE_STUB=OFF`.

### Сборка на Windows

1. Откройте терминал Visual Studio (e.g. «x64 Native Tools Command Prompt for VS»).
2. Перейдите в папку проекта:

   ```bat
   cd C:\Users\d-prihodko\Desktop\c-plus-plus-messenger
   mkdir build
   cd build
   ```

3. Запустите CMake с генератором VS и указанием Qt:

   ```bat
   cmake -G "Visual Studio 16 2019" -A x64 -DQT_PATH="C:/Qt/6.5.3/msvc2019_64" ..
   ```

4. Постройте проект:

   ```bat
   cmake --build . --config Release
   ```

5. Запустите бинарь:

   - `.\root\app\server\Release\chat_server.exe 8080`
   - `.\root\app\client\Release\chat_client.exe 127.0.0.1:8080`
   - `.\root\serverGUI\Release\server_gui.exe`

> Если возникают ошибки «Qt6::Widgets не найден», проверьте, что `CMAKE_PREFIX_PATH` корректно указывает на Qt6.
> Если libpqxx не находится, убедитесь, что правильно заданы include/lib пути.

### Сборка на Linux/macOS

1. В терминале:

   ```bash
   cd ~/projects/c-plus-plus-messenger
   mkdir build
   cd build
   cmake ..
   cmake --build .
   ```

2. При необходимости:

   ```bash
   cmake -DCMAKE_BUILD_TYPE=Release ..
   ```

3. После сборки исполняемые:

   - `./root/app/server/chat_server 8080`
   - `./root/app/client/chat_client 127.0.0.1:8080`
   - `./root/serverGUI/server_gui`

> При ошибках поиска Qt: укажите `-DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/:$CMAKE_PREFIX_PATH`.

---

## Конфигурация базы данных

Часто новичкам сложно настроить БД. Здесь подробный пошаговый пример для PostgreSQL.

### Инициализация схемы и таблиц

1. Запустите psql под привилегированным пользователем (обычно `postgres`):

   ```bash
   sudo -u postgres psql
   ```

   или Windows: откройте «psql» с учёткой postgres.

2. Создайте базу данных:

   ```sql
   CREATE DATABASE chat_db;
   ```

3. Подключитесь к созданной БД:

   ```sql
   \c chat_db
   ```

4. Создайте схему `chat`:

   ```sql
   CREATE SCHEMA chat;
   ```

5. Выполните SQL-скрипт `root/app/database/init.sql`. Если в скрипте нет префикса `chat.`, откройте файл и:

   - Добавьте в начало:

     ```sql
     SET search_path = chat;
     ```

     и уберите префиксы `chat.` из имён таблиц внутри скрипта, или

   - Явно создайте таблицы с префиксом `chat.`:

     ```sql
     CREATE TABLE chat.users (...);
     ```

6. Пример содержимого `init.sql` (скорректируйте, если необходимо):

   ```sql
   -- Создание таблиц внутри схемы chat
   CREATE TABLE chat.users (
       id SERIAL PRIMARY KEY,
       login VARCHAR(50) NOT NULL UNIQUE,
       password VARCHAR(50) NOT NULL
   );
   CREATE TABLE chat.messages (
       id SERIAL PRIMARY KEY,
       sender_id INTEGER REFERENCES chat.users ON DELETE CASCADE,
       receiver_id INTEGER REFERENCES chat.users ON DELETE CASCADE,
       text TEXT NOT NULL,
       timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
   );
   CREATE INDEX idx_messages_sender ON chat.messages (sender_id);
   CREATE INDEX idx_messages_receiver ON chat.messages (receiver_id);
   CREATE TABLE chat.online_status (
       user_id INTEGER NOT NULL PRIMARY KEY REFERENCES chat.users ON DELETE CASCADE,
       status VARCHAR(10) DEFAULT 'offline' NOT NULL
   );
   CREATE INDEX idx_online_status ON chat.online_status (status);
   CREATE TABLE chat.logs (
       id SERIAL PRIMARY KEY,
       event_type VARCHAR(50) NOT NULL,
       user_id INTEGER REFERENCES chat.users,
       timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
       details TEXT
   );
   -- Триггер для обновления timestamp (опционально):
   CREATE FUNCTION chat.update_timestamp() RETURNS trigger LANGUAGE plpgsql AS $$
   BEGIN
       NEW.timestamp = NOW();
       RETURN NEW;
   END;
   $$;
   CREATE TRIGGER messages_timestamp BEFORE
       INSERT ON chat.messages FOR EACH ROW EXECUTE PROCEDURE chat.update_timestamp();
   ```

7. При необходимости загрузки тестовых данных:

   ```bash
   psql -U postgres -d chat_db -f root/app/database/sample_data.sql
   ```

   Если `sample_data.sql` пуст или отсутствует данные, проверьте или создайте вручную тестовых пользователей.

### Создание пользователя БД и права доступа

По соображениям безопасности не рекомендуется использовать системного пользователя `postgres` из приложения. Лучше создать отдельного:

1. В psql:

   ```sql
   CREATE USER chat_user WITH PASSWORD 'secure_password';
   GRANT CONNECT ON DATABASE chat_db TO chat_user;
   ```

2. Внутри БД:

   ```sql
   \c chat_db
   GRANT USAGE ON SCHEMA chat TO chat_user;
   GRANT SELECT, INSERT, UPDATE, DELETE ON ALL TABLES IN SCHEMA chat TO chat_user;
   -- Чтобы новые таблицы автоматически получали права:
   ALTER DEFAULT PRIVILEGES IN SCHEMA chat
     GRANT SELECT, INSERT, UPDATE, DELETE ON TABLES TO chat_user;
   ```

3. В настройках запуска сервера используйте:

   - Переменную окружения:

     ```bash
     export CHAT_DB_CONN="host=localhost port=5432 dbname=chat_db user=chat_user password=secure_password"
     ```

   - Или CLI-параметры при запуске:

     ```bash
     ./chat_server --db "host=... user=... password=..."
     ```

   - В коде: читайте переменную окружения или передавайте строку подключения в конструктор `Database`.

> **Совет новичкам**: не храните пароли в репозитории. Используйте переменные окружения или отдельный конфигурационный файл, игнорируемый Git (например, `.env`, но не в репозитории).

### Безопасность хранения паролей

По умолчанию в схеме таблицы `users` хранится поле `password VARCHAR(50)`, что означает хранение паролей в открытом виде — **небезопасно**. Рекомендуется:

1. Изменить схему БД:

   ```sql
   ALTER TABLE chat.users
     RENAME COLUMN password TO password_hash;
   -- При необходимости добавить поле salt или хранить соль внутри хэша.
   ```

2. Использовать алгоритм хэширования:

   - bcrypt, Argon2, PBKDF2.
   - Библиотеки: e.g. libsodium, cryptopp, libbcrypt.

3. При регистрации:

   - Клиент отправляет пароль серверу (в чистом виде); лучше шифровать трафик (TLS).
   - Сервер генерирует соль и хэширует пароль, сохраняет только хэш.

4. При входе:

   - Клиент отправляет пароль; сервер загружает хранимый хэш, сравнивает введённый пароль путём вычисления хэша с тем же алгоритмом и солью.

5. Дополнительные рекомендации:

   - Ограничьте длину и сложность пароля на клиенте и сервере.
   - Храните пароли и конфигурацию БД вне кода.

---

## Запуск и использование

### 1. Запуск сервера

1. Убедитесь, что БД создана, схема и таблицы инициализированы (см. раздел выше).
2. Убедитесь, что переменная окружения или конфиг содержит строку подключения:

   ```bash
   export CHAT_DB_CONN="host=localhost port=5432 dbname=chat_db user=chat_user password=secure_password"
   ```

   Если вы не используете переменные окружения, отредактируйте код `Server`/`Database`, чтобы считывать строку подключения из аргументов или файла.

3. Перейдите в папку с бинарями:

   ```bash
   cd build/root/app/server
   ```

4. Запустите сервер:

   ```bash
   ./chat_server 8080
   ```

   Или, если в коде сервер ожидает только порт, передаётся `argv[1]`.

   - Если порт занят или нет прав, появится ошибка — измените порт (например, `9090`).

5. Сервер выведет сообщение о запуске и начнёт слушать указанный порт. Логи будут записаны в файл `../logs/user_activity.log` (относительно рабочей директории).
6. Проверяйте логи:

   ```bash
   tail -f ../logs/user_activity.log
   ```

### 2. Запуск клиента

1. Убедитесь, что сервер запущен и слушает нужный порт.
2. Перейдите в папку:

   ```bash
   cd build/root/app/client
   ```

3. Запустите клиент с указанием `host:port`:

   ```bash
   ./chat_client 127.0.0.1:8080
   ```

4. Клиент отобразит меню:

   ```
   Выберите действие:
   [L]ogin - Вход
   [R]egister - Регистрация
   [Q]uit - Выход
   Ваш выбор:
   ```

5. Для регистрации: введите `r`, затем логин (не пустой) и пароль (не пустой). Сервер сохранит пользователя (если логин уникален).
6. Для входа: введите `l`, затем логин и пароль. Если всё верно, получите `Вход OK`.
7. После входа появится меню пользователя:

   ```
   Пользователь: <login>
   [L]ist users - Список онлайн пользователей
   [I]nbox - Входящие сообщения
   [S]end - Отправить сообщение
   [H]istory - История сообщений
   [O]ut - Выйти из аккаунта
   ```

8. **LIST**: клиент отправляет `"LIST\n"`, сервер возвращает `USERS user1 user2 ...`. Клиент выводит список пользователей, помеченных как онлайн (по таблице `online_status`).
9. **INBOX**: отправляется `"INBOX\n"`, сервер отвечает `INBOX_COUNT N`, затем `INBOX_MSG from message`, затем `INBOX_END`. Клиент выводит сообщения.
10. **SEND**: клиент запрашивает target user и текст, отправляет `"MESSAGE to text\n"`. Сервер сохраняет сообщение в БД и отвечает `MESSAGE_OK`.
11. **HISTORY**: запрашивает историю переписки с указанным пользователем, сервер отправляет несколько строк `HIST [sender to receiver]: text`, затем `HISTORY_END`.
12. При ошибках (например, пользователь не найден), сервер присылает `ERROR_USER_NOT_FOUND`, клиент может вывести сообщение об ошибке.
13. Чтобы выйти из аккаунта, выберите `o`. Чтобы завершить клиент, в начальном меню выберите `q`.

> **Совет новичкам**: внимательно вводите `host:port`. Если сервер на другой машине, укажите реальный IP или доменное имя: `chat_client 192.168.0.10:8080`. Если брандмауэр блокирует порт, настройте разрешения.

### 3. Запуск Qt GUI сервера

GUI по умолчанию собран с `USE_STUB`, т. е. использует заглушки, чтобы разработчик мог проверить интерфейс без реального сервера. Для интеграции:

1. **Отключите режим заглушек**: в `root/serverGUI/CMakeLists.txt` уберите `target_compile_definitions(... USE_STUB)` или соберите с `-DUSE_STUB=OFF`.
2. **Реализуйте сетевой протокол**: в коде GUI есть функция `sendCommandToServer`, которая подключается к `127.0.0.1:12345` и отправляет строку `cmd + '\n'`.

   - Сделайте сервер слушающим этот порт или измените адрес/порт в GUI-коде на тот, что использует консольный сервер (по умолчанию консольный сервер слушает порт, переданный в `chat_server <port>`).
   - Внедрите в сервер обработку команд GUI: `ALL_MESSAGES`, `BAN user`, `UNBAN user`, `KICK user`, и т. д., чтобы GUI мог взаимодействовать с сервером.

3. Запуск GUI:

   ```bash
   cd build/root/serverGUI
   ./server_gui
   ```

4. GUI попытается подключиться к серверу; при успешном соединении отобразит пользователей или сообщения.
5. **Настройка адреса/порта**: по умолчанию жёстко указан `"127.0.0.1", 12345`. Измените в коде или добавьте UI-элемент для ввода адреса/порта.
6. **Фильтрация и темы**: GUI позволяет фильтровать по логину, статусу, фильтровать сообщения по отправителю, получателю, дате, тексту. Тема может переключаться на тёмную/светлую.
7. **Логи GUI**: можно добавить логирование действий админа (например, в файл или консоль).

> **Совет новичкам**: сначала проверьте GUI в режиме stub (USE_STUB), чтобы убедиться, что интерфейс работает и вы понимаете структуру (таблицы, фильтры). Затем постепенно подключайте реальный сервер.

---

## Логирование и рабочие директории

- **Консольный сервер**: в коде `Logger` открывает файл `../logs/user_activity.log`, где `..` относительно рабочей директории, из которой вы запускаете сервер. Рекомендуется:

  - Запускать сервер из папки `build/root/app/server`, тогда `../logs` укажет на `build/root/app/logs`.
  - Если вы хотите записывать логи в исходную папку `root/app/logs`, можете запускать сервер из каталога `root/app/server`, либо изменить путь в коде на абсолютный или параметризуемый.

- **Консольный клиент**: выводит в stdout/stderr, не пишет в файлы.
- **GUI**: не записывает логи по умолчанию; вы можете добавить логирование по вашему усмотрению.

> **Совет**: для дебага удобно видеть логи сразу в консоли и/или в файле. Добавьте поддержку разных уровней логирования: DEBUG, INFO, WARN, ERROR.

---

## Настройка и расширение

### Параметры сервера и пул потоков

- В коде `Server::start()` создаётся `ThreadPool pool(4);`. Лучше:

  - Определять количество потоков по `std::thread::hardware_concurrency()`.
  - Делать число потоков настраиваемым через аргументы командной строки или конфигурационный файл.

- Порт сервера передавать как `chat_server <port>`; можно добавить больше параметров: адрес, пул потоков, таймауты, путь к логам, строку подключения к БД.

### USE_STUB в GUI

- `USE_STUB` позволяет разрабатывать интерфейс независимо от серверной логики. Для реальной работы отключите его:

  - Удалите определение или передавайте `-DUSE_STUB=0` в CMake.

- Добавьте поддержку конфигурации адреса/порта сервера в GUI.
- Реализуйте команды на сервере, которые GUI отправляет: `LIST`, `ALL_MESSAGES`, `BAN`, `UNBAN`, `KICK`.
- Парсинг ответов должен соответствовать тому, что возвращает сервер.

### Шифрование, хэширование паролей и TLS

- **Хэширование паролей**: вместо хранения plain-text храните хэши (bcrypt/Argon2). Используйте библиотеки, например, libsodium или libbcrypt.
- **TLS/SSL**: для защиты трафика между клиентом и сервером:

  - В консольном клиенте и сервере вместо «сырых» TCP-сокетов используйте TLS-обёртку (OpenSSL или другой TLS-библиотекой).
  - В GUI используйте QSslSocket вместо QTcpSocket.
  - Настройте сертификаты: CA, сертификат сервера; при желании — клиентская аутентификация.

- **Безопасность в конфигурации**: не храните пароли или ключи в репозитории; используйте переменные окружения или секретный менеджер.

### Международная поддержка и мультиязычность

- Для вывода сообщений в консоли и GUI используйте механизмы локализации:

  - В Qt: QTranslator, .ts/.qm файлы для разных языков.
  - В консольных приложениях: можно завести ключи сообщений и хранить словари перевода, или использовать gettext.

- По умолчанию можно поддерживать русский и английский. Добавьте переключатель языка в настройках.

---

## Статьи и обзорные руководства

Ниже собраны полезные материалы для глубокого понимания технологий, используемых мною в проекте.

| Тема                                          | Краткое содержание                                                                                                                                                                           |
| --------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Учебник “Chat Tutorial” от Qt6**            | Пошаговый разбор создания простого чата на Qt Quick + SQL, показывает базовые паттерны клиент/серверных сообщений и интеграцию с БД ([doc.qt.io][1])                                         |
| **Интеграция PostgreSQL ↔ C/C++**             | Классическая статья о libpq/libpqxx: установка, подключение, примеры запросов и транзакций — хороший старт, если раньше не работали с Postgres из C++ ([tutorialspoint.com][2])              |
| **О многопоточности в Qt**                    | Детальный разбор моделей потоков Qt (QThread, QRunnable, QThreadPool) на русском языке с примерами кода — полезно для понимания, как распределять задачи в GUI-приложении ([zhitenev.ru][3]) |
| **QThread/QThreadPool/QtConcurrent — сводка** | Короткая шпаргалка на русском о подходах к параллельным вычислениям в Qt, с хитростями и граблями при миграции с одного API на другое ([russianblogs.com][4])                                |

---

## Репозитории GitHub (реальный код)

Ниже перечислены полезные репозитории с примерами клиент-серверных чатов, многопоточностью и хранилищем данных:

| Репозиторий                                     | Описание / чем полезен                                                                                                                                    |
| ----------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `Runanka/Chat-Application`                      | Полный клиент-серверный чат на C++ & Qt (QTcpSocket/QTcpServer) с лексическим разбором сообщений — хорошая сверка архитектурных решений ([github.com][5]) |
| `mnbedel/chat-application`                      | Продвинутый Qt-чат с поддержкой файлов и бродкаста; разделённый сервер/клиент поможет увидеть масштабирование протокола ([github.com][6])                 |
| `Hosbital1990/Multithreaded-TCP-Server`         | Чистый “голый” C++20-сервер + thread-pool + epoll; концентрируется на производительности и чистоте архитектуры ([github.com][7])                          |
| `AnkitDimri/multithread-tcp-server`             | Свежий пример (обновляется) мультипоточного TCP-сервера; читаемый код + Makefile, удобно для экспериментов ([github.com][8])                              |
| `progschj/ThreadPool`                           | Минималистичная реализация thread-pool’а на std::thread; можно встроить вместо собственного пула ([github.com][9])                                        |
| `chriskohlhoff/asio` (пример `chat_server.cpp`) | Эталонный пример асинхронного чата на Boost.Asio с корутинами C++20; хорош для сравнения с Qt-решением ([github.com][10])                                 |
| `botaojia/chat`                                 | Консольный чат на Boost.Asio + thread-pool + strand — иллюстрирует продвинутую синхронизацию обработчиков ([github.com][11])                              |
| `BaseMax/BoostAsioChat`                         | Ещё одна лёгкая реализация чата на Boost.Asio; лаконичный код полезен для быстрого понимания паттерна publish/subscribe ([github.com][12])                |
| `taocpp/taopq`                                  | Альтернативный лёгковесный клиент к PostgreSQL на C++20; интересен тем, что не зависит от libpqxx и поддерживает connection-pooling ([github.com][13])    |
| `Odizinne/QServer-Monitor`                      | Qt-GUI для мониторинга удалённого сервера; можно подсмотреть паттерны построения табличного интерфейса и обновления через таймеры ([github.com][14])      |

---

## Видео и плейлисты YouTube

| Ссылка                                             | Описание                                                                                                                                     |
| -------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------- |
| **Qt Network Programming (30 мин.)**               | Быстрый практический обзор работы с QTcpSocket/QTcpServer, сигналами и слотами — выдержка из курса по Qt 6 ([youtube.com][15])               |
| **Плейлист “Qt C++ Network Application” (ч. 1-8)** | Серия из 8 роликов: создание клиент-серверного приложения с нуля, разбор ошибок соединений, протокол обмена, базовый чат ([youtube.com][16]) |

---

## Документация и официальные справочники

- **Qt Examples & Tutorials** — центральный каталог всех примеров Qt; смотрите раздел _Network → Fortune Server/Client_ и _Threading → Threaded Fortune Server_ ([doc.qt.io][17]).
- **libpqxx “Getting Started”** — краткий гайд: подключение, транзакция, перебор результатов (удобен для первых запросов) ([libpqxx.readthedocs.io][18]).
- **QThreadPool (Qt Core)** — официальное API-описание; покажет, как запускать QRunnable и регулировать максимальное число потоков ([doc.qt.io][19]).

---

## Контакты и поддержка

- **Email разработчиков**: [example1@example.com](mailto:example1@example.com), [example2@example.com](mailto:example2@example.com)
- **Telegram**: [@example_dev](https://t.me/example_dev)
- **Issue tracker**: создавайте issue в репозитории с подробным описанием проблемы или предложения.
- **PR**: присылайте чистый, документированный код, следуя стилю C++17, с тестами и описанием изменений.

При создании issue указывайте:

- Описание проблемы/фичи.
- Шаги для воспроизведения (если баг).
- Версия ОС, компилятора, Qt, PostgreSQL.
- Логи и вывод ошибок.

---

## Лицензия

Проект распространяется под лицензией MIT.

---

## Примечания для новичков

- **Чистая сборка (clean build)**: если вы меняли файлы CMakeLists или обновляли зависимости, удалите папку `build` и создайте заново, чтобы избежать артефактов.
- **Отладка ошибок CMake**:

  - Если CMake не находит Qt: проверьте `CMAKE_PREFIX_PATH`.
  - Если не находит libpqxx: проверьте `pkg-config --modversion libpqxx`, или задайте вручную пути.
  - Читайте вывод CMake внимательно: ищите строки `-- Could NOT find Qt6` или `libpqxx not found`.

- **Рабочая директория при запуске**:

  - Сервер: запускайте из папки с бинарем или настройте путь к логам через переменную.
  - GUI: запускайте из билд-папки, чтобы ресурсы (если есть) находились корректно.

- **Переменные окружения**:

  - Для строки подключения к БД: `CHAT_DB_CONN`.
  - Для пути к Qt: `CMAKE_PREFIX_PATH`, а при запуске GUI/сервера можно использовать другие переменные (например, адрес сервера, порт).

- **Пример .env** (не добавляйте .env в репозиторий!):

  ```
  CHAT_DB_CONN="host=localhost port=5432 dbname=chat_db user=chat_user password=secure_password"
  SERVER_PORT=8080
  ```

  В коде считывайте `std::getenv("CHAT_DB_CONN")`.

- **Отладка**:

  - Используйте `std::cerr`/`qDebug()` для вывода ошибок.
  - Проверьте соединение с БД отдельно через psql перед запуском сервера.
  - Проверьте сетевые подключения: `telnet localhost 8080` или `nc -vz localhost 8080`.

- **Версионирование**:

  - Помечайте релизы в GitHub, фиксируйте версии зависимостей (например, Qt6.5, libpqxx 7.x).

- **Документирование кода**:

  - Комментируйте публичные методы, описывайте протокол команд (например, `"REGISTER login password\n"` → ответ `"REGISTER_OK"` или `"REGISTER_ERROR"`).
  - Генерируйте документацию через Doxygen (добавьте Doxyfile и цель `make docs`).

- **Общение**:

  - При возникновении проблем сначала ищите похожие решения в официальных документациях Qt, PostgreSQL, CMake, libpqxx.
  - Задавайте вопросы на профильных форумах (например, StackOverflow) с описанием окружения и ошибок.

---

## Ссылки

[1]: https://doc.qt.io/qt-6/qtquickcontrols-chattutorial-example.html "Qt Quick Controls - Chat Tutorial | Qt 6"
[2]: https://www.tutorialspoint.com/postgresql/postgresql_c_cpp.htm "PostgreSQL C/C++ Integration - TutorialsPoint"
[3]: https://zhitenev.ru/about-threading-in-qt-and-how-to-create-a-thread/ "О многопоточности в Qt и как создать поток"
[4]: https://russianblogs.com/article/7549950449/ "Сводка по использованию потока Qt: QtConcurrent, QThreadPool, QRunnable ..."
[5]: https://github.com/Runanka/Chat-Application "GitHub - Runanka/Chat-Application: пример чат-приложения на C++ & Qt"
[6]: https://github.com/mnbedel/chat-application "GitHub - mnbedel/chat-application: продвинутый Qt-чат с файлами"
[7]: https://github.com/Hosbital1990/Multithreaded-TCP-Server "GitHub - Multithreaded-TCP-Server: C++20 + thread-pool + epoll"
[8]: https://github.com/AnkitDimri/multithread-tcp-server "GitHub - multithread-tcp-server: пример мультипоточного TCP-сервера"
[9]: https://github.com/progschj/ThreadPool "GitHub - ThreadPool: простая реализация пула потоков на C++11"
[10]: https://github.com/chriskohlhoff/asio/blob/master/asio/src/examples/cpp20/coroutines/chat_server.cpp "GitHub - Asio Chat Server example"
[11]: https://github.com/botaojia/chat "GitHub - Boost.Asio chat server example"
[12]: https://github.com/BaseMax/BoostAsioChat "GitHub - BoostAsioChat: простой чат на Boost.Asio"
[13]: https://github.com/taocpp/taopq "GitHub - taopq: клиент PostgreSQL на C++20 (альтернатива libpqxx)"
[14]: https://github.com/Odizinne/QServer-Monitor "GitHub - QServer-Monitor: Qt GUI для мониторинга сервера"
[15]: https://www.youtube.com/watch?v=JacoUUPklHY "Qt Network Programming (30 мин.)"
[16]: https://www.youtube.com/playlist?list=PLh0cogPqXcJMwB5xyLE2cpp39DcSQ1lY6 "Qt C++ Network Application Tutorial (1-8)"
[17]: https://doc.qt.io/qt-6/qtexamplesandtutorials.html "Qt Examples & Tutorials | Qt 6"
[18]: https://libpqxx.readthedocs.io/stable/getting-started.html "libpqxx: Getting Started"
[19]: https://doc.qt.io/qt-6/qthreadpool.html "QThreadPool Class | Qt Core | Qt 6"

---

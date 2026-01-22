# C++ Messenger

[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17+-00599C?style=flat&logo=c%2B%2B&logoColor=white)](https://en.cppreference.com/w/cpp/17)
[![Qt Version](https://img.shields.io/badge/Qt-6+-41CD52?style=flat&logo=qt&logoColor=white)](https://www.qt.io/)
[![PostgreSQL](https://img.shields.io/badge/PostgreSQL-16.3-316192?style=flat&logo=postgresql&logoColor=white)](https://www.postgresql.org/)
[![License](https://img.shields.io/badge/License-Free_to_Use-green.svg)](LICENSE)
[![Maintenance](https://img.shields.io/badge/Maintained%3F-yes-green.svg)](https://github.com/dimbo1324)

> **Un mensajero multiplataforma con arquitectura cliente-servidor, construido usando C++(17) y Qt6. Incluye cliente/servidor de consola, GUI para administrador y almacenamiento en PostgreSQL.**

---

### 🌐 Documentación / Documentation / Документация

* 🇺🇸 [**English (Inglés)**](../../README.md)
* 🇷🇺 [**Russian (Ruso)**](README_rus.md)

---

## 📖 Visión general

**C++ Messenger** es una plantilla lista para producción para crear una aplicación de mensajería segura y escalable. Sigue una arquitectura limpia, separando interacciones de red (sockets TCP), multihilo, trabajo con base de datos y componentes de interfaz.

Ya sea que estés aprendiendo C++ o desarrollando un sistema de chat en tiempo real, este proyecto proporciona la infraestructura esencial — sockets multiplataforma, pools de hilos, integración con PostgreSQL e interfaz administrativa en Qt6 — para que puedas enfocarte en agregar funciones como encriptación o soporte para medios.

### ✨ Características clave

* **Soporte multiplataforma:** Funciona en Linux y Windows gracias a abstracciones para sockets (POSIX/WinSock) e hilos.
* **Servidor multihilo:** Maneja múltiples conexiones usando ThreadPool para distribución eficiente de tareas.
* **Qt6 GUI para administrador:** Monitoreo de usuarios, filtrado de mensajes, gestión de bans/kicks, con temas (oscuro/claro).
* **Almacenamiento en PostgreSQL:** Almacenamiento seguro de usuarios, mensajes, logs y estados en línea usando libpqxx.
* **Cliente/servidor de consola:** Registro, inicio de sesión, intercambio de mensajes, listas de usuarios y visualización de historial.
* **Bibliotecas reutilizables:** `tcp` para sockets y `threading` para pools y logging.
* **Diseño extensible:** Fácil de agregar encriptación, archivos multimedia, notificaciones push o pruebas unitarias.

---

## 🛠️ Pila tecnológica

* **Lenguaje:** [C++ (17)](https://en.cppreference.com/w/cpp/17)
* **Framework UI:** [Qt6](https://www.qt.io/)
* **Base de datos:** [PostgreSQL](https://www.postgresql.org/)
* **Driver de base de datos:** [libpqxx](https://github.com/jtv/libpqxx)
* **Sistema de compilación:** [CMake](https://cmake.org/)
* **Red:** Sockets POSIX / WinSock
* **Multihilo:** std::thread / std::mutex
* **Otras herramientas:** pkg-config (para Linux/macOS), Git

---

## 🚀 Cómo empezar

Sigue estos pasos para ejecutar una copia local. Esta guía está diseñada para principiantes — incluso si eres nuevo en TI, explicaremos cada paso con claridad.

### Requisitos previos

Antes de comenzar, instala estas herramientas. Se proporcionan enlaces de descarga.

* **Git**: Para clonar el proyecto. Descarga de [git-scm.com](https://git-scm.com/downloads). Instala y agrega a tu PATH.
* **CMake**: Versión 3.21 o superior. Descarga de [cmake.org](https://cmake.org/download/). Instala y agrega a PATH.
* **Compilador C++**:
  - **Windows**: Visual Studio 2019/2022 (edición Community es gratuita). Descarga de [visualstudio.microsoft.com](https://visualstudio.microsoft.com/downloads). Selecciona la carga de trabajo "Desarrollo de escritorio con C++".
  - **Linux (Ubuntu/Debian)**: Instala GCC/Clang vía terminal: `sudo apt update && sudo apt install g++ cmake`.
  - **macOS**: Instala Xcode desde App Store, luego Command Line Tools: `xcode-select --install`.
* **Qt6**: Versión 6 o superior (para GUI). Descarga el instalador de [qt.io/download](https://www.qt.io/download-qt-online-installer). Instala los componentes "Desktop" para tu plataforma (ej., msvc2019_64 en Windows).
* **PostgreSQL**: Servidor de base de datos. Descarga de [postgresql.org/download](https://www.postgresql.org/download/). Instala y recuerda la contraseña del superusuario (por defecto: "postgres").
* **libpqxx**: Biblioteca C++ para PostgreSQL.
  - **Linux**: `sudo apt install libpqxx-dev`.
  - **macOS**: `brew install libpqxx` (instala Homebrew primero: `/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"`).
  - **Windows**: Descarga fuentes de [github.com/jtv/libpqxx](https://github.com/jtv/libpqxx), compila con CMake, o usa vcpkg: Instala vcpkg, luego `vcpkg install libpqxx:x64-windows`.

Prueba las instalaciones:
- Abre un terminal/símbolo del sistema y ejecuta: `git --version`, `cmake --version`, `g++ --version` (o `cl` en Windows), `psql --version`.

### 1. Clonar el repositorio

Abre un terminal/símbolo del sistema y ejecuta:

```bash
git clone https://github.com/dimbo1324/c-plus-plus-messenger.git
cd c-plus-plus-messenger
```

Esto descarga el proyecto en tu computadora.

### 2. Configuración del entorno

El proyecto usa variables de entorno para personalización. Configúralas en tu terminal antes de compilar/ejecutar.

| Variable           | Descripción                               | Valor por defecto                                                              |
| ------------------ | ----------------------------------------- | ------------------------------------------------------------------------------ |
| `CHAT_DB_CONN`     | Cadena de conexión a la BD                | `host=localhost port=5432 dbname=chat_db user=postgres password=your_password` |
| `QT_PATH`          | Ruta a la instalación de Qt6 (para CMake) | (Configura si no está en PATH, ej., `C:/Qt/6.5.3/msvc2019_64`)                 |
| `CMAKE_BUILD_TYPE` | Tipo de compilación (Release/Debug)       | `Release`                                                                      |

En Windows: `set CHAT_DB_CONN=host=localhost port=5432 dbname=chat_db user=postgres password=your_password`

En Linux/macOS: `export CHAT_DB_CONN="host=localhost port=5432 dbname=chat_db user=postgres password=your_password"`

Reemplaza `your_password` con tu contraseña de PostgreSQL.

### 3. Iniciar la base de datos

Inicia el servicio de PostgreSQL:
- **Windows**: Busca "Servicios" en el menú Inicio, encuentra "postgresql-x64-16", clic derecho > Iniciar.
- **Linux**: `sudo systemctl start postgresql`
- **macOS**: `brew services start postgresql`

Verifica: Ejecuta `psql -U postgres` e ingresa tu contraseña. Escribe `\q` para salir.

### 4. Migración de la base de datos

Crea la base de datos y las tablas.

1. Abre psql: `psql -U postgres`
2. Crea la BD: `CREATE DATABASE chat_db;`
3. Conéctate: `\c chat_db`
4. Ejecuta el script SQL de `root/app/database/init.sql`. Copia-pega el contenido en psql, o ejecuta: `psql -U postgres -d chat_db -f root/app/database/init.sql`
5. (Opcional) Agrega datos de muestra: `psql -U postgres -d chat_db -f root/app/database/sample_data.sql`

Crea un usuario dedicado por seguridad:
- En psql: `CREATE USER chat_user WITH PASSWORD 'secure_password';`
- Otorga acceso: `GRANT CONNECT ON DATABASE chat_db TO chat_user; \c chat_db GRANT USAGE ON SCHEMA public TO chat_user; GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA public TO chat_user;`

Actualiza `CHAT_DB_CONN` para usar `chat_user` y `secure_password`.

### 5. Compilar el proyecto

Crea una carpeta de compilación para mantener las cosas limpias:

```bash
mkdir build
cd build
```

Configura CMake:
- **Windows** (en "x64 Native Tools Command Prompt for VS"):
  ```bat
  cmake -G "Visual Studio 16 2019" -A x64 -DQT_PATH="C:/Qt/6.5.3/msvc2019_64" ..
  ```
- **Linux/macOS**:
  ```bash
  cmake -DCMAKE_BUILD_TYPE=Release ..
  ```

Compila:
```bash
cmake --build . --config Release
```

Esto crea ejecutables: `chat_client`, `chat_server`, `server_gui`.

---

## 📂 Estructura del proyecto

El proyecto sigue un diseño modular:

```text
c-plus-plus-messenger/
├── .gitignore
├── LICENSE
├── README.md
├── assets/                   # Recursos (ej., imágenes para GUI)
│   ├── 1.png
│   └── ...
├── CMakeLists.txt            # CMake raíz: Incluye todos los subproyectos
└── root/
    ├── app/
    │   ├── client/           # Cliente de consola
    │   │   ├── CMakeLists.txt
    │   │   ├── include/      # Cabeceras del cliente
    │   │   │   ├── Client.h
    │   │   │   └── UI.h
    │   │   └── src/          # Fuentes del cliente
    │   │       ├── main.cpp
    │   │       ├── Client.cpp
    │   │       └── UI.cpp
    │   ├── server/           # Servidor de consola
    │   │   ├── CMakeLists.txt
    │   │   ├── include/      # Cabeceras del servidor
    │   │   │   ├── ClientHandler.h
    │   │   │   ├── Database.h
    │   │   │   ├── Logger.h
    │   │   │   └── Server.h
    │   │   └── src/          # Fuentes del servidor
    │   │       ├── main.cpp
    │   │       ├── Server.cpp
    │   │       ├── ClientHandler.cpp
    │   │       ├── Database.cpp
    │   │       └── Logger.cpp
    │   ├── libs/             # Bibliotecas compartidas
    │   │   ├── tcp/          # Abstracción de sockets TCP
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
    │   │   └── threading/    # ThreadPool y Logger
    │   │       ├── CMakeLists.txt
    │   │       ├── include/
    │   │       │   ├── threading_config.h
    │   │       │   └── threading/
    │   │       │       ├── Logger.h
    │   │       │       └── ThreadPool.h
    │   │       └── src/
    │   │           ├── Logger.cpp
    │   │           └── ThreadPool.cpp
    │   ├── database/         # Scripts SQL
    │   │   ├── init.sql
    │   │   ├── sample_data.sql
    │   │   └── README.md
    │   └── logs/             # Logs del servidor
    │       └── user_activity.log
    └── serverGUI/            # Qt6 GUI para administrador
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

## 🔌 Uso

### Servidor

```bash
cd build/root/app/server/Release  # o Debug
./chat_server 8080
```

Salida: "Server listening on port 8080". Logs en `../logs/user_activity.log`.

### Cliente

```bash
cd build/root/app/client/Release
./chat_client 127.0.0.1:8080
```

Sigue el menú: Registro/Inicio de sesión, luego envía mensajes, ve listas/historial.

### GUI de administrador

```bash
cd build/root/serverGUI/Release
./server_gui
```

Actualiza usuarios/mensajes, filtra, banea/kickea. Usa stubs por defecto (edita CMake para desactivar).

---

## 🤝 Contribuir

Las contribuciones son lo que hace que la comunidad open-source sea un lugar increíble para aprender, inspirar y crear. Cualquier contribución que hagas será **muy apreciada**.

1. Forkea el proyecto
2. Crea tu rama de característica (`git checkout -b feature/AmazingFeature`)
3. Confirma tus cambios (`git commit -m 'Add some AmazingFeature'`)
4. Empuja a la rama (`git push origin feature/AmazingFeature`)
5. Abre un Pull Request

---

## 📜 Licencia

Este proyecto es libre para usar.

---

## 📬 Contacto

Si tienes preguntas, sugerencias o solo quieres saludar, ¡no dudes en contactarme!

* **Autor:** dimbo1324
* **Telegram:** [@dimbo1324](https://t.me/dimbo1324)
* **Email:** dimaprihodko180@gmail.com
* **GitHub:** [github.com/dimbo1324](https://github.com/dimbo1324)

---

*Desarrollado con ❤️ por dimbo1324*
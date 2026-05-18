# C++ Messenger

C++ Messenger is an experimental client/server messenger prototype written in C++17 with a console client, a TCP server, PostgreSQL storage, and an optional Qt6 admin GUI.

It is no longer documented as production-ready. The project now has safer defaults and a more honest baseline, but it has not had a full security audit and still uses plaintext TCP unless you run it only on trusted networks or add TLS in a later change.

## Current Features

- User registration and login.
- Password storage as PostgreSQL `pgcrypto` bcrypt hashes in `chat.users.password_hash`.
- Length-prefixed TCP frames with partial read/write handling.
- PostgreSQL schema under `chat` with versioned SQL migrations.
- Thread-safe PostgreSQL connection pool.
- Online/offline/banned statuses with `last_seen`.
- Message persistence, inbox, dialogue history, and online delivery to active sessions.
- Multiple active sessions for one user are allowed; online delivery is sent to all active sessions.
- Basic roles: `user`, `moderator`, `admin`.
- Admin commands: list users/messages, ban, unban, kick.
- Optional Qt6 admin GUI connected to the real framed protocol.
- Basic protocol/auth unit tests and GitHub Actions CI.

## Important Security Notes

- Passwords are not stored as plaintext. The server hashes passwords using PostgreSQL `pgcrypto` (`crypt()` with `gen_salt('bf', 12)`).
- `CHAT_DB_CONN` is required unless `config/server.conf` provides `db_conn`. The server refuses to start with a hardcoded password or unsafe fallback.
- `.env` and local config files are ignored by git. Use `.env.example` and `config/server.conf.example` as templates.
- TLS is not implemented in this pass. `ENABLE_TLS` and TLS env variables are placeholders for a future transport change. Do not use this over untrusted networks.
- Existing plaintext-password users from the old prototype cannot be migrated safely. Recreate them or force password reset in development.

## Architecture

- `root/app/libs/tcp`: cross-platform socket wrapper plus framed protocol helpers.
- `root/app/server`: config, logging, auth policy, database pool, session registry, server/session command handling.
- `root/app/client`: console client using framed TCP.
- `root/serverGUI`: Qt6 admin GUI. Real-server mode is default; stub mode is optional.
- `root/app/database`: versioned PostgreSQL migrations and sample data.
- `docs/protocol.md`: current protocol format.

## Requirements

- C++17 compiler.
- CMake 3.21+.
- PostgreSQL 16 recommended.
- libpqxx.
- Qt6 Widgets and Network for the GUI.
- Docker Compose, optional, for local PostgreSQL.

## Quick Start With Docker PostgreSQL

Start PostgreSQL:

```bash
docker compose up -d postgres
```

The compose file uses development-only credentials:

```text
database: messenger_dev
app user: messenger_app
app password: messenger_dev_password
```

Set the connection string:

```bash
export CHAT_DB_CONN="host=localhost port=5432 dbname=messenger_dev user=messenger_app password=messenger_dev_password"
```

On Windows PowerShell:

```powershell
$env:CHAT_DB_CONN="host=localhost port=5432 dbname=messenger_dev user=messenger_app password=messenger_dev_password"
```

If the database volume already existed before migrations were mounted, apply migrations manually with an owner/admin connection:

```bash
psql "host=localhost port=5432 dbname=messenger_dev user=postgres password=postgres_dev_password" -f root/app/database/init.sql
```

Optional sample data:

```bash
psql "host=localhost port=5432 dbname=messenger_dev user=postgres password=postgres_dev_password" -f root/app/database/sample_data.sql
```

## Local Build

Configure server, client, and tests without GUI:

```bash
cmake -S . -B build -DBUILD_GUI=OFF -DBUILD_TESTS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Build with GUI:

```bash
cmake -S . -B build -DBUILD_GUI=ON -DQT_PATH=/path/to/Qt
cmake --build build --parallel
```

Useful CMake options:

```text
BUILD_CLIENT=ON
BUILD_SERVER=ON
BUILD_GUI=ON
BUILD_TESTS=ON
USE_STUB=OFF
ENABLE_TLS=OFF
ENABLE_WARNINGS=ON
ENABLE_SANITIZERS=OFF
```

`USE_STUB=ON` is only for GUI demo data. Normal builds should leave it off.

## Configuration

Environment variables take precedence over `config/server.conf`.

```text
CHAT_DB_CONN             required unless db_conn exists in config/server.conf
CHAT_SERVER_PORT         default 8080
CHAT_DB_POOL_SIZE        default 4
CHAT_MAX_CLIENTS         default 64
CHAT_READ_TIMEOUT_SEC    default 300
CHAT_MAX_FRAME_SIZE      default 65536
CHAT_LOG_PATH            default logs/user_activity.log
```

GUI variables:

```text
CHAT_GUI_HOST
CHAT_GUI_PORT
CHAT_GUI_ADMIN_LOGIN
CHAT_GUI_ADMIN_PASSWORD
```

The GUI logs in before each admin command. The account must exist and have `admin` or `moderator` role.

## Database Migrations

Migration files live in `root/app/database/migrations`.

Apply all migrations with a database owner/admin connection because migrations create a schema, extension, tables, and grants:

```bash
psql "<admin-or-owner-connection-string>" -f root/app/database/init.sql
```

The schema contains:

- `chat.users`
- `chat.messages`
- `chat.audit_log`

Statuses are constrained to:

```text
online
offline
banned
```

Roles are constrained to:

```text
user
moderator
admin
```

## Running

Server:

```bash
./build/root/app/server/chat_server
```

Optional CLI port override:

```bash
./build/root/app/server/chat_server 8080
```

Client:

```bash
./build/root/app/client/chat_client 127.0.0.1:8080
```

GUI:

```bash
CHAT_GUI_HOST=127.0.0.1 CHAT_GUI_PORT=8080 CHAT_GUI_ADMIN_LOGIN=admin CHAT_GUI_ADMIN_PASSWORD=... ./build/root/serverGUI/server_gui
```

## Creating an Admin User for Development

Register a user through the client, then promote it locally:

```sql
UPDATE chat.users SET role = 'admin' WHERE login = 'admin';
```

Do not use shared development passwords outside a local machine.

## Protocol

The TCP protocol uses a 4-byte big-endian length prefix followed by a UTF-8 payload. See `docs/protocol.md`.

## Tests

Current tests cover:

- frame encoder/decoder partial data;
- multiple frames in one buffer;
- oversized frames;
- basic password/login policy.

Run:

```bash
ctest --test-dir build --output-on-failure
```

Integration tests with a live PostgreSQL database are not yet implemented.

## Troubleshooting

- `CHAT_DB_CONN is not set`: set the environment variable or create `config/server.conf` from `config/server.conf.example`.
- `pgcrypto` errors: apply migrations as a database owner that can run `CREATE EXTENSION IF NOT EXISTS pgcrypto`.
- GUI says admin login failed: verify `CHAT_GUI_ADMIN_LOGIN`, `CHAT_GUI_ADMIN_PASSWORD`, and the user's role.
- Client cannot connect: check server port, firewall, and `CHAT_SERVER_PORT`.

## Known Limitations

- TLS transport is not implemented yet.
- Payloads are text commands, not JSON or protobuf.
- There is no end-to-end encryption.
- Admin GUI authentication is env-driven, not an interactive login form.
- No automated PostgreSQL integration tests yet.
- Thread-per-client with a connection limit is safer than the old fixed worker pool, but it is not a high-scale async server.

## Roadmap

- Add real TLS transport with certificate configuration.
- Replace text payloads with JSON or a typed binary protocol.
- Add PostgreSQL-backed integration tests.
- Add password reset and account recovery flows.
- Add interactive admin login in the GUI.
- Add rate limiting and brute-force protection.

## License

No repository license file is currently specified. Add a real `LICENSE` file before distributing this project as open source.

-- ⚡ Таблица Users
-- ❓ Назначение: Хранение информации о пользователях чата.
-- 📌 user_id (PK, автоинкремент)
-- 📌 login (уникальное, обязательное)
-- 📌 password_hash (хеш пароля, обязательное)
-- 📌 name (обязательное)
-- 📌 created_at (дата создания записи)
-- ⚠️ Поле для хранения salt, если требуется алгоритм хеширования.
create table users
(
    user_id       serial primary key,
    login         varchar(50) unique not null,
    password_hash varchar(255)       not null,
    name          varchar(100)       not null,
    created_at    timestamp default current_timestamp,
    salt          varchar(255)
);
-- ⚡ Таблица Chats
-- ❓ Назначение: Хранение информации о чатах (личных и групповых).
-- 📌 chat_id (PK, автоинкремент)
-- 📌 chat_type (тип чата: личный/групповой)
-- 📌 created_at (дата создания)
-- ⚠️ chat_name для групповых чатов, описание.
create table chats
(
    chat_id    serial primary key,
    chat_type  varchar(10) not null check (chat_type in ('личный', 'групповой')),
    chat_name  varchar(100),
    created_at timestamp default current_timestamp
);
-- ⚡ Таблица ChatParticipants
-- ❓ Назначение: Связующая таблица для участников чатов.
-- 📌 chat_id (FK, обязательное)
-- 📌 user_id (FK, обязательное)
-- 📌 joined_at (дата присоединения)
-- 🔑 Ключи: составной первичный ключ (chat_id, user_id).
create table chat_participants
(
    chat_id   int not null,
    user_id   int not null,
    joined_at timestamp default current_timestamp,
    primary key (chat_id, user_id),
    foreign key (chat_id) references chats (chat_id),
    foreign key (user_id) references users (user_id)
);
-- ⚡ Таблица Messages
-- ❓ Назначение: Хранение отправленных сообщений.
-- 📌 message_id (PK, автоинкремент)
-- 📌 chat_id (FK, обязательное)
-- 📌 sender_id (FK, обязательное)
-- 📌 message_text (текст сообщения)
-- 📌 sent_at (дата и время отправки)
-- ⚠️ Флаг, указывающий, что сообщение было отредактировано.
create table messages
(
    message_id   serial primary key,
    chat_id      int  not null,
    sender_id    int  not null,
    message_text text not null,
    sent_at      timestamp default current_timestamp,
    edited       boolean   default false,
    foreign key (chat_id) references chats (chat_id),
    foreign key (sender_id) references users (user_id)
);
-- ⚡ Таблица MessageRecipients
-- ❓ Назначение: Отслеживание получателей сообщений и их статуса.
-- 📌 message_id (FK, обязательное)
-- 📌 recipient_id (FK, обязательное)
-- 📌 status ('отправлено', 'доставлено', 'прочитано')
-- 🔑 Ключи: составной первичный ключ (message_id, recipient_id).
create table message_recipients
(
    message_id   int         not null,
    recipient_id int         not null,
    status       varchar(20) not null check ( status in ('отправлено', 'доставлено', 'прочитано')),
    primary key (message_id, recipient_id),
    foreign key (message_id) references messages (message_id),
    foreign key (recipient_id) references users (user_id)
);
-- ⚡ Таблица UserRoles
-- ❓ Назначение: Определение ролей пользователей (например, администратор, модератор).
-- 📌 role_id (PK, автоинкремент)
-- 📌 role_name (уникальное, обязательное)
-- ⚠️ Описание роли, приоритет.
create table user_roles
(
    role_id   serial primary key,
    role_name varchar(50) unique not null
);
-- ⚡ Таблица ChatRoles
-- ❓ Назначение: Связывание пользователей, чатов и ролей в этих чатах.
-- 📌 chat_id (FK, обязательное)
-- 📌 user_id (FK, обязательное)
-- 📌 role_id (FK, обязательное)
-- 🔑 Ключи: составной первичный ключ (chat_id, user_id, role_id).
create table chat_roles
(
    chat_id int not null,
    user_id int not null,
    role_id int not null,
    primary key (chat_id,
                 user_id,
                 role_id),
    foreign key (chat_id) references chats (chat_id),
    foreign key (user_id) references users (user_id),
    foreign key (role_id) references user_roles (role_id)
);
-- ⚡ Таблица Attachments
-- ❓ Назначение: Хранение информации о вложениях в сообщениях.
-- 📌 attachment_id (PK, автоинкремент)
-- 📌 message_id (FK, обязательное)
-- 📌 file_path (путь к файлу, обязательное)
-- 📌 file_type (тип файла, обязательное)
-- ⚠️ Размер файла, дата загрузки.
create table attachments
(
    attachment_id serial primary key,
    message_id    int          not null,
    file_path     varchar(255) not null,
    file_type     varchar(50)  not null,
    file_size     int,
    uploaded_at   timestamp default current_timestamp,
    foreign key (message_id) references messages (message_id)
);
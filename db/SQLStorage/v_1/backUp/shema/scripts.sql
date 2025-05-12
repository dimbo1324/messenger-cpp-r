create table users
(
user_id       serial primary key,
login         varchar(50) unique not null,
password_hash varchar(255)       not null,
name          varchar(100)       not null,
created_at    timestamp default current_timestamp,
salt          varchar(255)
);
create table chats
(
chat_id    serial primary key,
chat_type  varchar(10) not null check (chat_type in ('личный', 'групповой')),
chat_name  varchar(100),
created_at timestamp default current_timestamp
);
create table chat_participants
(
chat_id   int not null,
user_id   int not null,
joined_at timestamp default current_timestamp,
primary key (chat_id, user_id),
foreign key (chat_id) references chats (chat_id),
foreign key (user_id) references users (user_id)
);
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
create table message_recipients
(
message_id   int         not null,
recipient_id int         not null,
status       varchar(20) not null check ( status in ('отправлено', 'доставлено', 'прочитано')),
primary key (message_id, recipient_id),
foreign key (message_id) references messages (message_id),
foreign key (recipient_id) references users (user_id)
);
create table user_roles
(
role_id   serial primary key,
role_name varchar(50) unique not null
);
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
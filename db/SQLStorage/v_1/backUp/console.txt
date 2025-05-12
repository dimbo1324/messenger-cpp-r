[2025-04-04 20:21:27] Connected to chat_db
chat_db.help_schema> set search_path = "help_schema"
[2025-04-04 20:21:27] completed in 2 ms
chat_db.help_schema> create table users
(
user_id       serial primary key,
login         varchar(50) unique not null,
password_hash varchar(255)       not null,
name          varchar(100)       not null,
created_at    timestamp default current_timestamp,
salt          varchar(255)
)
[2025-04-04 21:00:27] completed in 120 ms
chat_db.help_schema> create table chats
(
chat_id    serial primary key,
chat_type  varchar(10) not null check (chat_type in ('личный', 'групповой')),
chat_name  varchar(100),
created_at timestamp default current_timestamp
)
[2025-04-04 21:00:27] completed in 11 ms
chat_db.help_schema> create table chat_participants
(
chat_id   int not null,
user_id   int not null,
joined_at timestamp default current_timestamp,
primary key (chat_id, user_id),
foreign key (chat_id) references chats (chat_id),
foreign key (user_id) references users (user_id)
)
[2025-04-04 21:00:27] completed in 25 ms
chat_db.help_schema> create table messages
(
message_id   serial primary key,
chat_id      int  not null,
sender_id    int  not null,
message_text text not null,
sent_at      timestamp default current_timestamp,
edited       boolean   default false,
foreign key (chat_id) references chats (chat_id),
foreign key (sender_id) references users (user_id)
)
[2025-04-04 21:00:27] completed in 16 ms
chat_db.help_schema> create table message_recipients
(
message_id   int         not null,
recipient_id int         not null,
status       varchar(20) not null check ( status in ('отправлено', 'доставлено', 'прочитано')),
primary key (message_id, recipient_id),
foreign key (message_id) references messages (message_id),
foreign key (recipient_id) references users (user_id)
)
[2025-04-04 21:00:27] completed in 7 ms
chat_db.help_schema> create table user_roles
(
role_id   serial primary key,
role_name varchar(50) unique not null
)
[2025-04-04 21:00:27] completed in 8 ms
chat_db.help_schema> create table chat_roles
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
)
[2025-04-04 21:00:27] completed in 8 ms
chat_db.help_schema> create table attachments
(
attachment_id serial primary key,
message_id    int          not null,
file_path     varchar(255) not null,
file_type     varchar(50)  not null,
file_size     int,
uploaded_at   timestamp default current_timestamp,
foreign key (message_id) references messages (message_id)
)
[2025-04-04 21:00:27] completed in 6 ms
chat_db.help_schema> create index idx_user_login on users (login)
[2025-04-04 21:17:28] completed in 11 ms
chat_db.help_schema> create index idx_messages_sent_at on messages (sent_at)
[2025-04-04 21:17:28] completed in 3 ms
chat_db.help_schema>
create or replace function update_updated_at_column()
returns trigger as
$$
begin
new.update_at = current_timestamp;
return new;
end
$$ language plpgsql
[2025-04-04 21:46:11] completed in 91 ms
chat_db.help_schema> create table message_logs
(
log_id     serial primary key,
message_id int not null,
old_text   text,
new_text   text,
old_status varchar(20),
new_status varchar(20),
changed_at timestamp default current_timestamp
)
[2025-04-04 21:46:11] completed in 8 ms
chat_db.help_schema> create or replace function log_messages_changes()
returns trigger as
$$
begin
insert into message_logs (message_id, old_text, new_text, old_status, new_status)
values (old.message_id, old.message_text, new.message_text, old.status, new.status);
return new;
end;
$$ language plpgsql
[2025-04-04 21:46:11] completed in 2 ms
chat_db.help_schema>
create trigger update_users_updated_at
before update
on users
for each row
execute function update_updated_at_column()
[2025-04-04 21:46:11] completed in 3 ms
chat_db.help_schema> create trigger log_messages_changes
after update
on messages
for each row
when ( old is distinct from new)
execute function log_messages_changes()
[2025-04-04 21:46:11] completed in 2 ms
chat_db.help_schema> create extension if not exists pgcrypto
[2025-04-06 10:00:51] completed in 143 ms
[2025-04-06 11:15:03] Connected to chat_db
chat_db.help_schema> set search_path = "help_schema"
[2025-04-06 11:15:03] completed in 2 ms
chat_db.help_schema> create or replace function register_user(
p_login varchar,
p_password varchar,
p_name varchar
)
returns void as
$$
declare
v_salt          varchar(255);
v_password_hash varchar(255);
begin
if exists(select 1 from users where login = p_login) then
raise exception 'Логин уже существует: %', p_login;
end if;
v_salt := gen_salt('bf');
v_password_hash := crypt(p_password, v_salt);
insert into users (login, password_hash, name, salt)
values (p_login, v_password_hash, p_name, v_salt);
end
$$ language plpgsql
[2025-04-06 11:15:03] completed in 71 ms
chat_db.help_schema> create or replace procedure send_message(
p_chat_id int,
p_sender_id int,
p_message_text text,
p_recipient_ids int[]
)
language plpgsql
as
$$
declare
v_message_id int;
recipient    int;
begin
insert into messages(chat_id, sender_id, message_text)
values (p_chat_id, p_sender_id, p_message_text)
returning message_id into v_message_id;
foreach recipient in array p_recipient_ids
loop
insert into message_recipients(message_id, recipient_id, status)
values (v_message_id, recipient, 'отправлено');
end loop;
end;
$$
[2025-04-06 11:15:03] completed in 5 ms
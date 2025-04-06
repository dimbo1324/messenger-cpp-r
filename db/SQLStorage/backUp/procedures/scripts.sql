create extension if not exists pgcrypto;
create or replace function register_user(
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
$$ language plpgsql;
create or replace procedure send_message(
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
$$;

create table users (
    id serial primary key,
    login varchar(50) not null unique,
    password varchar(50) not null
);
create table messages (
    id serial primary key,
    sender_id integer references users on delete cascade,
    receiver_id integer references users on delete cascade,
    text text not null,
    timestamp timestamp default CURRENT_TIMESTAMP
);
create index idx_messages_sender on messages (sender_id);
create index idx_messages_receiver on messages (receiver_id);
create table online_status (
    user_id integer not null primary key references users on delete cascade,
    status varchar(10) default 'offline'::character varying not null
);
create index idx_online_status on online_status (status);
create table logs (
    id serial primary key,
    event_type varchar(50) not null,
    user_id integer references users,
    timestamp timestamp default CURRENT_TIMESTAMP,
    details text
);
create function update_timestamp() returns trigger language plpgsql as $$ BEGIN NEW.timestamp = NOW();
RETURN NEW;
END;
$$;
create trigger messages_timestamp before
insert on messages for each row execute procedure update_timestamp();
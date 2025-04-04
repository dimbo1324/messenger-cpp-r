------------------------------------------------------------------
-- functions and help tables
------------------------------------------------------------------
create or replace function update_updated_at_column()
    returns trigger as
$$
begin
    new.update_at = current_timestamp;
    return new;
end
$$ language plpgsql;
create table message_logs
(
    log_id     serial primary key,
    message_id int not null,
    old_text   text,
    new_text   text,
    old_status varchar(20),
    new_status varchar(20),
    changed_at timestamp default current_timestamp
);
create or replace function log_messages_changes()
    returns trigger as
$$
begin
    insert into message_logs (message_id, old_text, new_text, old_status, new_status)
    values (old.message_id, old.message_text, new.message_text, old.status, new.status);
    return new;
end;
$$ language plpgsql;
------------------------------------------------------------------
-- triggers
------------------------------------------------------------------
create trigger update_users_updated_at
    before update
    on users
    for each row
execute function update_updated_at_column();
create trigger log_messages_changes
    after update
    on messages
    for each row
    when ( old is distinct from new)
execute function log_messages_changes();
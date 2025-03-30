<!-- 
TODO: Ссылка на ТЗ (Database for console chat's project): 
* https://docs.google.com/document/d/1gSBqZ10cYyHCSCBy8xABeF39an_JUxmv0StW5y-0UvE/edit?usp=sharing 
-->

<!-- 
Техническое задание на разработку базы данных проекта «Чат»
1. Общие положения
База данных должна обеспечивать:
Нормализацию до 4-й нормальной формы, что исключает транзитивные и многозначные зависимости.
Блочность и динамичность за счёт использования автоинкрементных ключей и триггеров для автоматической синхронизации данных.
Безопасность за счёт разделения публичных и конфиденциальных данных (разделение таблиц с профилем и учетными данными, применение современных методов хэширования с солью).
Масштабируемость посредством индексирования ключевых полей и возможного партиционирования крупных таблиц (например, сообщений) по временным периодам.
Интегрируемость и читаемость благодаря четкой документированной структуре таблиц и явному определению связей.
Поддерживаемость за счёт ведения аудита (таблица audit_log) и автоматизации (триггеры для создания/удаления записей в связанных таблицах).
2. Структура базы данных
2.1 Таблица: users_profile
Поля и их характеристики:
user_id
Тип: Автоинкрементное целое число (SERIAL/INTEGER).
Описание: Уникальный идентификатор пользователя, служащий первичным ключом.
Роль и обоснование: Обеспечивает уникальность каждой записи и используется для связей с другими таблицами.
first_name
Тип: VARCHAR(50).
Описание: Имя пользователя для отображения в интерфейсе.
Роль и обоснование: Используется для персонализации и идентификации пользователя.
last_name
Тип: VARCHAR(50).
Описание: Фамилия пользователя для отображения в интерфейсе.
Роль и обоснование: Совместно с именем помогает однозначно идентифицировать пользователя.
email
Тип: VARCHAR(100) (с уникальным ограничением).
Описание: Уникальный адрес электронной почты, используемый для входа и связи.
Роль и обоснование: Является важным идентификатором и коммуникационным каналом; уникальность предотвращает дублирование.
language
Тип: VARCHAR(10) или ENUM (при ограниченном наборе языков).
Описание: Предпочитаемый язык пользователя для локализации интерфейса.
Роль и обоснование: Обеспечивает поддержку многоязычного интерфейса и персонализацию.
timezone
Тип: VARCHAR(50) или ссылка на справочную таблицу часовых поясов.
Описание: Часовой пояс пользователя для корректного отображения времени в системе.
Роль и обоснование: Позволяет корректно синхронизировать время для пользователей из разных регионов.
created_at
Тип: TIMESTAMP (с дефолтным значением – текущее время).
Описание: Время создания записи пользователя.
Роль и обоснование: Используется для аудита, отслеживания регистрации и истории изменений.
updated_at
Тип: TIMESTAMP (с дефолтным значением – текущее время, обновляется автоматически).
Описание: Время последнего обновления записи.
Роль и обоснование: Помогает отслеживать изменения и обеспечивает актуальность данных.
is_deleted
Тип: BOOLEAN (по умолчанию FALSE).
Описание: Флаг логического удаления записи пользователя.
Роль и обоснование: Обеспечивает возможность мягкого удаления данных для сохранения истории и последующего восстановления.
2.2 Таблица: users_credentials
Поля и их характеристики:
user_id
Тип: INTEGER.
Описание: Идентификатор пользователя, являющийся первичным ключом и внешним ключом, ссылающимся на users_profile.user_id.
Роль и обоснование: Связывает учетные данные с публичной информацией пользователя; использование одного и того же идентификатора повышает безопасность и целостность данных (отношение 1:1).
password_hash
Тип: VARCHAR(255).
Описание: Хэшированное значение пароля пользователя.
Роль и обоснование: Гарантирует безопасность аутентификации, так как реальный пароль не хранится в открытом виде.
salt
Тип: VARCHAR(50).
Описание: Случайная строка, применяемая при хэшировании пароля для усиления защиты от атак.
Роль и обоснование: Повышает стойкость хэширования и предотвращает использование предварительно вычисленных таблиц (rainbow tables).
Примечание по автоматизации:
При добавлении новой записи в таблицу users_profile срабатывает триггер, который автоматически создает соответствующую запись в users_credentials с тем же user_id и значением password_hash = NULL. Аналогичным образом, при удалении записи из users_profile удаляется связанная запись в users_credentials.
2.3 Таблица: messages
Поля и их характеристики:
message_id
Тип: Автоинкрементное целое число (SERIAL/INTEGER).
Описание: Уникальный идентификатор сообщения, являющийся первичным ключом.
Роль и обоснование: Обеспечивает уникальную идентификацию каждого сообщения в системе.
sender_id
Тип: INTEGER.
Описание: Идентификатор отправителя сообщения, внешний ключ, ссылающийся на users_profile.user_id.
Роль и обоснование: Обеспечивает связь между сообщениями и пользователем-отправителем (отношение один ко многим).
receiver_id
Тип: INTEGER.
Описание: Идентификатор получателя сообщения, внешний ключ, ссылающийся на users_profile.user_id.
Роль и обоснование: Обеспечивает связь между сообщениями и пользователем-получателем (отношение один ко многим).
text
Тип: VARCHAR(1000) или TEXT.
Описание: Текстовое содержимое сообщения.
Роль и обоснование: Основной контент сообщения, необходимый для коммуникации между пользователями.
message_type
Тип: VARCHAR(20) или ENUM (например, 'text', 'image', 'file').
Описание: Тип сообщения, определяющий формат содержимого (например, текст, изображение, файл).
Роль и обоснование: Позволяет системе различать типы сообщений и правильно обрабатывать их отображение и логику.
sent_at
Тип: TIMESTAMP (с дефолтным значением – текущее время).
Описание: Дата и время отправки сообщения.
Роль и обоснование: Обеспечивает хронологическую последовательность переписки, что важно для истории общения.
status
Тип: INTEGER.
Описание: Код статуса сообщения (например, 0 – не доставлено, 1 – доставлено, 2 – прочитано).
Роль и обоснование: Позволяет отслеживать этапы доставки и прочтения сообщения, что важно для обеспечения обратной связи между пользователями. Рекомендуется добавить CHECK-констрейнт для допустимых значений.
2.4 Таблица: message_attachments
Поля и их характеристики:
attachment_id
Тип: Автоинкрементное целое число (SERIAL/INTEGER).
Описание: Уникальный идентификатор вложения, являющийся первичным ключом.
Роль и обоснование: Обеспечивает уникальную идентификацию каждого файла-вложения.
message_id
Тип: INTEGER.
Описание: Идентификатор сообщения, к которому прикреплено вложение, внешний ключ, ссылающийся на messages.message_id.
Роль и обоснование: Обеспечивает связь между сообщением и его вложениями (отношение один ко многим).
file_path
Тип: VARCHAR(255).
Описание: Путь к файлу вложения в файловой системе или облачном хранилище.
Роль и обоснование: Позволяет системе находить и извлекать вложения для отображения пользователю.
file_type
Тип: VARCHAR(50) или ENUM (например, 'image/jpeg', 'application/pdf').
Описание: Тип файла вложения, позволяющий корректно обрабатывать и отображать данные.
Роль и обоснование: Обеспечивает адаптивную обработку различных форматов файлов.
2.5 Таблица: audit_log
Поля и их характеристики:
log_id
Тип: Автоинкрементное целое число (SERIAL/INTEGER).
Описание: Уникальный идентификатор записи аудита, являющийся первичным ключом.
Роль и обоснование: Позволяет однозначно идентифицировать каждую запись аудита.
table_name
Тип: VARCHAR(50).
Описание: Название таблицы, в которой произошло изменение.
Роль и обоснование: Обеспечивает контекст для каждой аудиторской записи.
record_id
Тип: INTEGER.
Описание: Идентификатор изменённой записи в соответствующей таблице.
Роль и обоснование: Позволяет точно определить, какая запись была изменена.
action
Тип: VARCHAR(20) или ENUM (например, 'INSERT', 'UPDATE', 'DELETE').
Описание: Тип выполненного действия.
Роль и обоснование: Обеспечивает понимание характера изменений для последующего анализа.
changed_at
Тип: TIMESTAMP (с дефолтным значением – текущее время).
Описание: Дата и время выполнения изменения.
Роль и обоснование: Позволяет восстановить последовательность событий и проводить аудит.
changed_by
Тип: INTEGER.
Описание: Идентификатор пользователя, инициировавшего изменение, внешний ключ, ссылающийся на users_profile.user_id.
Роль и обоснование: Позволяет установить ответственность за внесенные изменения.
old_data
Тип: JSON.
Описание: Снимок данных до изменения.
Роль и обоснование: Обеспечивает возможность восстановления данных и анализа внесенных изменений.
new_data
Тип: JSON.
Описание: Снимок данных после изменения.
Роль и обоснование: Позволяет сравнить состояние до и после изменений, что полезно для аудита.
3. Взаимосвязи между таблицами и обоснование
Связь между таблицами users_profile и users_credentials:
Тип связи: Один к одному.
Обоснование: Каждый пользователь имеет один набор учетных данных. Разделение публичной информации и конфиденциальных данных повышает безопасность, позволяя ограничить доступ к таблице с паролями.
Связь между таблицами users_profile и messages (как отправитель и получатель):
Тип связи: Один ко многим.
Обоснование: Один пользователь может отправлять множество сообщений (поле sender_id) и получать их (поле receiver_id), что обеспечивает хранение полной истории переписки и удобный поиск по участникам.
Связь между таблицами messages и message_attachments:
Тип связи: Один ко многим.
Обоснование: Одно сообщение может содержать несколько вложений. Разделение текстовой информации и вложений облегчает управление данными и позволяет масштабировать таблицу сообщений без перегрузки дополнительными данными.
Связь аудита через таблицу audit_log:
Тип связи: Логически – многие ко многим (реализуется через идентификаторы записей в различных таблицах).
Обоснование: Каждое изменение в основных таблицах фиксируется в журнале аудита с указанием названия таблицы и идентификатора измененной записи. Это обеспечивает полную прослеживаемость и возможность восстановления данных, а также позволяет анализировать изменения и устанавливать ответственность.
Дополнительно:
При добавлении и удалении пользователя должны использоваться триггеры для автоматической синхронизации данных между таблицами users_profile и users_credentials (автоматическое создание записи с password_hash = NULL при добавлении, каскадное или логическое удаление при удалении).
Рекомендуется использовать каскадное обновление/удаление там, где это уместно, либо логическое удаление (через флаг is_deleted) для сохранения истории.
4. Итоговое обоснование архитектуры
Нормализация (до 4-й нормальной формы):
Каждая таблица содержит только атомарные значения, отсутствуют избыточные данные. Все неключевые атрибуты зависят исключительно от первичных ключей, что исключает транзитивные и многозначные зависимости.
Блочность и динамичность:
Использование автоинкрементных ключей и триггеров позволяет автоматизировать синхронизацию данных между таблицами, поддерживать целостность данных и динамично реагировать на изменения (например, автоматическое обновление поля updated_at).
Безопасность:
Разделение публичных данных (таблица users_profile) и конфиденциальных данных (таблица users_credentials) повышает уровень защиты. Применение современных методов хэширования с солью обеспечивает безопасность аутентификации, а ограничение доступа к таблице с учетными данными снижает риски.
Масштабируемость:
Возможность индексирования ключевых полей (например, email, sender_id, receiver_id, sent_at) и потенциальное партиционирование таблицы messages по времени гарантирует высокую производительность даже при большом объеме данных.
Интегрируемость и читаемость:
Четкая, документированная структура таблиц, использование стандартных типов данных и явное определение связей упрощают интеграцию с внешними системами и повышают читаемость базы данных.
Поддерживаемость:
Наличие журнала аудита (audit_log) позволяет отслеживать изменения, что упрощает диагностику проблем и восстановление данных. Автоматизация посредством триггеров снижает вероятность ошибок при ручном управлении данными.
Эта архитектура соответствует мировым стандартам 2025 года и готова к расширению в будущем (например, для поддержки групповых чатов, ролей пользователей, дополнительных типов сообщений и вложений), обеспечивая надежную, масштабируемую и безопасную систему для работы с данными проекта чата.
Разработка должна учитывать возможность внедрения дополнительных проверок (например, CHECK-констрейнтов для полей status и message_type), а также предусмотреть индексацию наиболее часто используемых полей для повышения производительности системы. 
-->

create extension if not exists pgcrypto;

create table users
(
    user_id       serial primary key,
    username      varchar(50) unique  not null,
    email         varchar(100) unique not null,
    password_hash varchar(255)        not null,
    created_at    timestamp default current_timestamp,
    updated_at    timestamp default current_timestamp,
    is_deleted    boolean   default false
);

create table user_profiles
(
    user_id    int primary key,
    first_name varchar(50),
    last_name  varchar(50),
    language   varchar(10) not null,
    timezone   varchar(50) not null,
    avatar_url varchar(255),
    bio        text,
    constraint fk_user_profile foreign key (user_id) references users (user_id) on delete cascade
);

create table conversations
(
    conversation_id serial primary key,
    title           varchar(100),
    is_group        boolean   default false,
    created_at      timestamp default current_timestamp,
    updated_at      timestamp default current_timestamp
);

create table conversation_participants
(
    conversation_id int not null,
    user_id         int not null,
    role            varchar(20) default 'member',
    joined_at       timestamp   default current_timestamp,
    primary key (conversation_id, user_id),
    constraint fk_conversation foreign key (conversation_id) references conversations (conversation_id) on delete cascade,
    constraint fk_participant foreign key (user_id) references users (user_id) on delete cascade,
    constraint check_role check (role in ('member', 'admin'))
);

create type message_status as enum ('sent', 'delivered', 'read');

create table messages
(
    message_id      serial primary key,
    conversation_id int            not null,
    sender_id       int            not null,
    text            text           not null,
    message_type    varchar(20)    not null,
    sent_at         timestamp               default current_timestamp,
    status          message_status not null default 'sent',
    constraint fk_message_conversation foreign key (conversation_id) references conversations (conversation_id) on delete cascade,
    constraint fk_message_sender foreign key (sender_id) references users (user_id) on delete cascade
);

create table message_attachments
(
    attachment_id serial primary key,
    message_id    int          not null,
    file_path     varchar(255) not null,
    file_type     varchar(50)  not null,
    constraint fk_attachment_message foreign key (message_id) references messages (message_id) on delete cascade
);

create table read_receipts
(
    message_id int not null,
    user_id    int not null,
    read_at    timestamp default current_timestamp,
    primary key (message_id, user_id),
    constraint fk_receipt_message foreign key (message_id) references messages (message_id) on delete cascade,
    constraint fk_receipt_user foreign key (user_id) references users (user_id) on delete cascade
);

create table message_reactions
(
    reaction_id serial primary key,
    message_id  int         not null,
    user_id     int         not null,
    reaction    varchar(50) not null,
    reacted_at  timestamp default current_timestamp,
    constraint fk_reaction_message foreign key (message_id) references messages (message_id) on delete cascade,
    constraint fk_reaction_user foreign key (user_id) references users (user_id) on delete cascade
);

create table audit_log
(
    log_id     serial primary key,
    table_name varchar(50) not null,
    record_id  text,
    action     varchar(20) not null,
    changed_at timestamp default current_timestamp,
    changed_by int,
    old_data   json,
    new_data   json,
    constraint fk_audit_changed_by foreign key (changed_by) references users (user_id) on delete set null
);

create or replace function update_updated_at()
    returns trigger as
$$
begin
    new.updated_at = current_timestamp;
    return new;
end;
$$ language plpgsql;

create trigger update_users_updated_at
    before update
    on users
    for each row
execute function update_updated_at();

create trigger update_conversations_updated_at
    before update
    on conversations
    for each row
execute function update_updated_at();

create trigger update_messages_updated_at
    before update
    on messages
    for each row
execute function update_updated_at();

create trigger update_message_attachments_updated_at
    before update
    on message_attachments
    for each row
execute function update_updated_at();

create or replace procedure create_user(
    p_username varchar(50),
    p_email varchar(100),
    p_password_hash varchar(255),
    p_first_name varchar(50),
    p_last_name varchar(50),
    p_language varchar(10),
    p_timezone varchar(50)
)
    language plpgsql
as
$$
declare
    v_user_id int;
begin
    insert into users (username, email, password_hash)
    values (p_username, p_email, p_password_hash)
    returning user_id into v_user_id;

    insert into user_profiles (user_id, first_name, last_name, language, timezone)
    values (v_user_id, p_first_name, p_last_name, p_language, p_timezone);
end;
$$;

create or replace function log_audit()
    returns trigger as
$$
declare
    pk_value       text;
    changed_by_val int  := null;
    col_pk         text := tg_argv[0];
    col_changed_by text := null;
begin
    if array_length(tg_argv, 1) >= 2 then
        col_changed_by := tg_argv[1];
    end if;

    if tg_op = 'DELETE' then
        execute format('select ($1).' || quote_ident(col_pk) || '::text') into pk_value using old;
        if col_changed_by is not null then
            execute format('select ($1).' || quote_ident(col_changed_by) || '::int') into changed_by_val using old;
        end if;
    else
        execute format('select ($1).' || quote_ident(col_pk) || '::text') into pk_value using new;
        if col_changed_by is not null then
            execute format('select ($1).' || quote_ident(col_changed_by) || '::int') into changed_by_val using new;
        end if;
    end if;

    if tg_op = 'INSERT' then
        insert into audit_log (table_name, record_id, action, changed_at, changed_by, new_data)
        values (tg_table_name, pk_value, tg_op, current_timestamp, changed_by_val, row_to_json(new));
    elsif tg_op = 'UPDATE' then
        insert into audit_log (table_name, record_id, action, changed_at, changed_by, old_data, new_data)
        values (tg_table_name, pk_value, tg_op, current_timestamp, changed_by_val, row_to_json(old), row_to_json(new));
    elsif tg_op = 'DELETE' then
        insert into audit_log (table_name, record_id, action, changed_at, changed_by, old_data)
        values (tg_table_name, pk_value, tg_op, current_timestamp, changed_by_val, row_to_json(old));
    end if;
    return null;
end;
$$ language plpgsql;

create or replace function log_audit_composite()
    returns trigger as
$$
declare
    pk_value       text;
    changed_by_val int;
begin
    if tg_op = 'DELETE' then
        pk_value := old.conversation_id::text || '-' || old.user_id::text;
        changed_by_val := old.user_id;
    else
        pk_value := new.conversation_id::text || '-' || new.user_id::text;
        changed_by_val := new.user_id;
    end if;

    if tg_op = 'INSERT' then
        insert into audit_log (table_name, record_id, action, changed_at, changed_by, new_data)
        values (tg_table_name, pk_value, tg_op, current_timestamp, changed_by_val, row_to_json(new));
    elsif tg_op = 'UPDATE' then
        insert into audit_log (table_name, record_id, action, changed_at, changed_by, old_data, new_data)
        values (tg_table_name, pk_value, tg_op, current_timestamp, changed_by_val, row_to_json(old), row_to_json(new));
    elsif tg_op = 'DELETE' then
        insert into audit_log (table_name, record_id, action, changed_at, changed_by, old_data)
        values (tg_table_name, pk_value, tg_op, current_timestamp, changed_by_val, row_to_json(old));
    end if;
    return null;
end;
$$ language plpgsql;

create trigger audit_users
    after insert or update or delete
    on users
    for each row
execute function log_audit('user_id', 'user_id');

create trigger audit_user_profiles
    after insert or update or delete
    on user_profiles
    for each row
execute function log_audit('user_id');

create trigger audit_conversations
    after insert or update or delete
    on conversations
    for each row
execute function log_audit('conversation_id');

create trigger audit_messages
    after insert or update or delete
    on messages
    for each row
execute function log_audit('message_id', 'sender_id');

create trigger audit_message_attachments
    after insert or update or delete
    on message_attachments
    for each row
execute function log_audit('attachment_id');

create trigger audit_message_reactions
    after insert or update or delete
    on message_reactions
    for each row
execute function log_audit('reaction_id');

create trigger audit_conversation_participants
    after insert or update or delete
    on conversation_participants
    for each row
execute function log_audit_composite();

create index idx_messages_conversation_id on messages (conversation_id);
create index idx_conversation_participants_user_id on conversation_participants (user_id);
create index idx_read_receipts_message_id on read_receipts (message_id);
create index idx_messages_sender_id on messages (sender_id);
create index idx_messages_sent_at on messages (sent_at);

create or replace function soft_delete_user()
    returns trigger as
$$
begin
    if new.is_deleted = true and old.is_deleted = false then
        update conversation_participants set joined_at = null where user_id = new.user_id;
    end if;
    return new;
end;
$$ language plpgsql;

create trigger trigger_soft_delete_user
    before update
    on users
    for each row
execute function soft_delete_user();

create role user_role;
grant select, insert, update on users, messages to user_role;

create role admin_role;
grant all privileges on all tables in schema public to admin_role;

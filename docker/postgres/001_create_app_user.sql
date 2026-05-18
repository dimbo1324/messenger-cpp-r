DO $$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM pg_roles WHERE rolname = 'messenger_app') THEN
        CREATE ROLE messenger_app LOGIN PASSWORD 'messenger_dev_password' NOSUPERUSER NOCREATEDB NOCREATEROLE NOINHERIT;
    END IF;
END
$$;

GRANT CONNECT ON DATABASE messenger_dev TO messenger_app;

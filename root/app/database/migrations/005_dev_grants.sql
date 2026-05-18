DO $$
BEGIN
    IF EXISTS (SELECT 1 FROM pg_roles WHERE rolname = 'messenger_app') THEN
        GRANT USAGE ON SCHEMA chat TO messenger_app;
        GRANT SELECT, INSERT, UPDATE, DELETE ON ALL TABLES IN SCHEMA chat TO messenger_app;
        GRANT USAGE, SELECT ON ALL SEQUENCES IN SCHEMA chat TO messenger_app;
        ALTER DEFAULT PRIVILEGES IN SCHEMA chat
            GRANT SELECT, INSERT, UPDATE, DELETE ON TABLES TO messenger_app;
        ALTER DEFAULT PRIVILEGES IN SCHEMA chat
            GRANT USAGE, SELECT ON SEQUENCES TO messenger_app;
    END IF;
END
$$;

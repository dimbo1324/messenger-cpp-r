DO $$ BEGIN IF NOT EXISTS (
    SELECT 1
    FROM pg_roles
    WHERE rolname = 'chat_app'
) THEN CREATE ROLE chat_app LOGIN PASSWORD 'secure_password';
END IF;
END $$;
CREATE SCHEMA IF NOT EXISTS chat AUTHORIZATION chat_app;
CREATE TABLE IF NOT EXISTS chat.users (
    user_id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    login TEXT NOT NULL UNIQUE,
    password TEXT NOT NULL,
    created_at TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT now(),
    last_seen TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT now()
);
CREATE TABLE IF NOT EXISTS chat.messages (
    message_id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    sender_id BIGINT NOT NULL REFERENCES chat.users(user_id) ON DELETE CASCADE,
    recipient_id BIGINT NOT NULL REFERENCES chat.users(user_id) ON DELETE CASCADE,
    content TEXT NOT NULL,
    sent_at TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT now(),
    is_read BOOLEAN NOT NULL DEFAULT FALSE
);
CREATE INDEX IF NOT EXISTS idx_messages_recipient_sentat ON chat.messages(recipient_id, sent_at DESC);
CREATE INDEX IF NOT EXISTS idx_messages_sender_sentat ON chat.messages(sender_id, sent_at DESC);
CREATE OR REPLACE FUNCTION chat.get_unread_messages(p_user_id BIGINT) RETURNS TABLE(
        message_id BIGINT,
        sender_login TEXT,
        content TEXT,
        sent_at TIMESTAMP WITH TIME ZONE
    ) LANGUAGE sql STABLE AS $$
SELECT m.message_id,
    u.login AS sender_login,
    m.content,
    m.sent_at
FROM chat.messages m
    JOIN chat.users u ON u.user_id = m.sender_id
WHERE m.recipient_id = p_user_id
    AND m.is_read = FALSE
ORDER BY m.sent_at ASC;
$$;
CREATE OR REPLACE FUNCTION chat.mark_message_read(p_message_id BIGINT) RETURNS VOID LANGUAGE plpgsql AS $$ BEGIN
UPDATE chat.messages
SET is_read = TRUE
WHERE message_id = p_message_id;
END;
$$;
CREATE OR REPLACE FUNCTION chat.update_last_seen() RETURNS TRIGGER LANGUAGE plpgsql AS $$ BEGIN
UPDATE chat.users
SET last_seen = now()
WHERE user_id = NEW.recipient_id;
RETURN NEW;
END;
$$;
CREATE TRIGGER trg_update_last_seen
AFTER
INSERT ON chat.messages FOR EACH ROW EXECUTE FUNCTION chat.update_last_seen();
GRANT USAGE ON SCHEMA chat TO chat_app;
GRANT SELECT,
    INSERT,
    UPDATE ON chat.users TO chat_app;
GRANT SELECT,
    INSERT,
    UPDATE ON chat.messages TO chat_app;
GRANT EXECUTE ON FUNCTION chat.get_unread_messages(BIGINT) TO chat_app;
GRANT EXECUTE ON FUNCTION chat.mark_message_read(BIGINT) TO chat_app;
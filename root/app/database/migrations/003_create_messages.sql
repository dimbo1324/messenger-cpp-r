CREATE TABLE IF NOT EXISTS chat.messages (
    id bigserial PRIMARY KEY,
    sender_id integer NOT NULL REFERENCES chat.users(id) ON DELETE CASCADE,
    receiver_id integer NOT NULL REFERENCES chat.users(id) ON DELETE CASCADE,
    text text NOT NULL,
    created_at timestamptz NOT NULL DEFAULT now(),
    CONSTRAINT messages_text_not_blank CHECK (length(trim(text)) > 0),
    CONSTRAINT messages_text_size CHECK (length(text) <= 4096)
);

CREATE INDEX IF NOT EXISTS idx_messages_sender_receiver_created
    ON chat.messages (sender_id, receiver_id, created_at);

CREATE INDEX IF NOT EXISTS idx_messages_receiver_created
    ON chat.messages (receiver_id, created_at);

CREATE INDEX IF NOT EXISTS idx_messages_created
    ON chat.messages (created_at);

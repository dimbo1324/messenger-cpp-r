CREATE TABLE IF NOT EXISTS chat.audit_log (
    id bigserial PRIMARY KEY,
    actor_user_id integer REFERENCES chat.users(id) ON DELETE SET NULL,
    action varchar(64) NOT NULL,
    target_login varchar(50),
    details text,
    created_at timestamptz NOT NULL DEFAULT now(),
    CONSTRAINT audit_action_not_blank CHECK (length(trim(action)) > 0)
);

CREATE INDEX IF NOT EXISTS idx_audit_actor_created
    ON chat.audit_log (actor_user_id, created_at);

CREATE INDEX IF NOT EXISTS idx_audit_action_created
    ON chat.audit_log (action, created_at);

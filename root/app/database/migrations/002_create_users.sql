CREATE TABLE IF NOT EXISTS chat.users (
    id serial PRIMARY KEY,
    login varchar(50) NOT NULL UNIQUE,
    password_hash text NOT NULL,
    role varchar(16) NOT NULL DEFAULT 'user',
    status varchar(16) NOT NULL DEFAULT 'offline',
    created_at timestamptz NOT NULL DEFAULT now(),
    updated_at timestamptz NOT NULL DEFAULT now(),
    last_seen timestamptz,
    CONSTRAINT users_login_not_blank CHECK (length(trim(login)) >= 3),
    CONSTRAINT users_role_check CHECK (role IN ('user', 'moderator', 'admin')),
    CONSTRAINT users_status_check CHECK (status IN ('online', 'offline', 'banned'))
);

CREATE INDEX IF NOT EXISTS idx_users_login ON chat.users (login);
CREATE INDEX IF NOT EXISTS idx_users_status ON chat.users (status);
CREATE INDEX IF NOT EXISTS idx_users_role ON chat.users (role);

DROP TRIGGER IF EXISTS trg_users_updated_at ON chat.users;
CREATE TRIGGER trg_users_updated_at
BEFORE UPDATE ON chat.users
FOR EACH ROW
EXECUTE FUNCTION chat.set_updated_at();

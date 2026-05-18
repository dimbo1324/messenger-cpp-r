# Database Setup

This directory contains PostgreSQL setup files for local development.

Apply all migrations with a database owner/admin connection because migrations create the schema, extension, tables, and grants:

```bash
psql "<admin-or-owner-connection-string>" -f root/app/database/init.sql
```

Load optional sample data:

```bash
psql "<admin-or-owner-connection-string>" -f root/app/database/sample_data.sql
```

Reset a local development database:

```bash
dropdb messenger_dev
createdb messenger_dev
psql "<admin-or-owner-connection-string>" -f root/app/database/init.sql
```

Notes:

- The application expects all tables under the `chat` schema.
- Passwords are stored in `chat.users.password_hash`.
- Hashing and verification use PostgreSQL `pgcrypto` with bcrypt via `crypt()`/`gen_salt('bf')`.
- Existing plaintext users from the old prototype cannot be migrated safely without knowing the original passwords. They should reset their password or be recreated in development.

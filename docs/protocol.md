# Messenger TCP Protocol

The current transport is plaintext TCP with length-prefixed UTF-8 frames.

## Frame Format

Each frame is:

```text
uint32_be payload_length
payload_length bytes UTF-8 payload
```

Default maximum payload size is 64 KiB and can be changed with `CHAT_MAX_FRAME_SIZE`.

## Payload Format

Payloads are still simple text commands/responses. This is intentionally modest so the project can harden the TCP framing without rewriting the full application protocol in the same pass.

Client commands:

```text
REGISTER <login> <password>
LOGIN <login> <password>
LOGOUT
LIST
INBOX
MESSAGE <recipient_login> <text>
HISTORY <target_login> <limit> <offset>
ALL_MESSAGES <limit>
BAN <login>
UNBAN <login>
KICK <login>
```

Server responses/events:

```text
REGISTER_OK
REGISTER_ERROR <code> <message>
LOGIN_OK <role>
LOGIN_ERROR <code> <message>
LOGOUT_OK
USERS_BEGIN
USER <login> <status> <role> <last_seen_utc>
USERS_END
INBOX_BEGIN
INBOX_MSG <sender> <created_at_utc> <text>
INBOX_END
HISTORY_BEGIN
HIST <sender> <receiver> <created_at_utc> <text>
HISTORY_END
MESSAGE_OK <message_id>
MESSAGE_ERROR <code> <message>
MESSAGE <sender> <text>
ALL_MESSAGES_BEGIN
MSG <sender> <receiver> <created_at_utc> <text>
ALL_MESSAGES_END
KICKED
ERROR_NOT_AUTH
ERROR_FORBIDDEN <message>
ERROR_FRAME_TOO_LARGE
UNKNOWN_CMD
```

Admin commands require a logged-in user with `admin` or `moderator` role.

## Known Limitations

- The payload format is not JSON yet, so text fields are parsed by command-specific conventions.
- TLS is not implemented in this pass. Do not use this protocol over untrusted networks.
- There is no replay protection or end-to-end encryption.

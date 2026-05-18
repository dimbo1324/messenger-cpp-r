-- Development-only sample data. Password hashes are intentionally not documented
-- as usable plaintext credentials; create real local users through REGISTER.

INSERT INTO chat.users (login, password_hash, role, status)
VALUES
    ('alice', crypt('AliceDev12345', gen_salt('bf', 12)), 'user', 'offline'),
    ('bob', crypt('BobDev12345', gen_salt('bf', 12)), 'moderator', 'offline'),
    ('carol', crypt('CarolDev12345', gen_salt('bf', 12)), 'user', 'banned')
ON CONFLICT (login) DO NOTHING;

INSERT INTO chat.messages (sender_id, receiver_id, text)
SELECT s.id, r.id, 'Sample message from Alice to Bob'
FROM chat.users s, chat.users r
WHERE s.login = 'alice' AND r.login = 'bob'
  AND NOT EXISTS (
      SELECT 1 FROM chat.messages m
      WHERE m.sender_id = s.id AND m.receiver_id = r.id AND m.text = 'Sample message from Alice to Bob'
  );

INSERT INTO chat.messages (sender_id, receiver_id, text)
SELECT s.id, r.id, 'Sample reply from Bob to Alice'
FROM chat.users s, chat.users r
WHERE s.login = 'bob' AND r.login = 'alice'
  AND NOT EXISTS (
      SELECT 1 FROM chat.messages m
      WHERE m.sender_id = s.id AND m.receiver_id = r.id AND m.text = 'Sample reply from Bob to Alice'
  );

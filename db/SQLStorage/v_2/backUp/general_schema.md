classDiagram
direction BT
class attachments {
   integer message_id
   varchar(255) file_path
   varchar(50) file_type
   integer file_size
   timestamp uploaded_at
   integer attachment_id
}
class chat_participants {
   timestamp joined_at
   integer chat_id
   integer user_id
}
class chat_roles {
   integer chat_id
   integer user_id
   integer role_id
}
class chats {
   varchar(10) chat_type
   varchar(100) chat_name
   text description
   timestamp created_at
   integer chat_id
}
class message_logs {
   integer message_id
   integer recipient_id
   text old_text
   text new_text
   varchar(20) old_status
   varchar(20) new_status
   timestamp changed_at
   integer log_id
}
class message_recipients {
   varchar(20) status
   integer message_id
   integer recipient_id
}
class messages {
   integer chat_id
   integer sender_id
   text message_text
   timestamp sent_at
   boolean edited
   integer message_id
}
class user_roles {
   varchar(50) role_name
   text description
   integer priority
   integer role_id
}
class users {
   varchar(50) login
   varchar(512) password_hash
   varchar(100) name
   timestamp created_at
   timestamp update_at
   varchar(255) salt
   integer user_id
}

attachments  -->  messages : message_id
chat_participants  -->  chats : chat_id
chat_participants  -->  users : user_id
chat_roles  -->  chats : chat_id
chat_roles  -->  user_roles : role_id
chat_roles  -->  users : user_id
message_logs  -->  users : recipient_id:user_id
message_recipients  -->  messages : message_id
message_recipients  -->  users : recipient_id:user_id
messages  -->  chats : chat_id
messages  -->  users : sender_id:user_id

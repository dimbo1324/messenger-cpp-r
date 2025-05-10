-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Хост: localhost
-- Время создания: Авг 10 2023 г., 07:27
-- Версия сервера: 8.0.33-0ubuntu0.23.04.2
-- Версия PHP: 8.1.12-1ubuntu4.2

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- База данных: `console_chat`
--

-- --------------------------------------------------------

--
-- Структура таблицы `hash_tab`
--

CREATE TABLE `hash_tab` (
  `uid` bigint UNSIGNED NOT NULL,
  `hash` varchar(40) COLLATE utf8mb4_general_ci NOT NULL,
  `salt` varchar(40) COLLATE utf8mb4_general_ci NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Дамп данных таблицы `hash_tab`
--

INSERT INTO `hash_tab` (`uid`, `hash`, `salt`) VALUES
(0, '2cfee2bd8b52a3800e594c6aae4deff49b6ac690', '019UL67jUe3u9js7LKDz00QKUXuwMSM24m5Y062I'),
(1, '8cda2ca21cd4c6919ffd143624bdde7cd2093f3f', 'ASDFG'),
(2, 'bc71172a816250f481f42569edc80f114d5035f5', 'ASDFG'),
(3, 'bc71172a816250f481f42569edc80f114d5035f5', 'ASDFG'),
(4, 'bc71172a816250f481f42569edc80f114d5035f5', 'ASDFG'),
(5, 'bc71172a816250f481f42569edc80f114d5035f5', 'ASDFG'),
(6, 'bc71172a816250f481f42569edc80f114d5035f5', 'ASDFG');

-- --------------------------------------------------------

--
-- Структура таблицы `private_messages`
--

CREATE TABLE `private_messages` (
  `id` bigint UNSIGNED NOT NULL,
  `author_id` bigint UNSIGNED DEFAULT NULL,
  `recipient_id` bigint UNSIGNED DEFAULT NULL,
  `text` mediumtext CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci,
  `published` bigint UNSIGNED NOT NULL DEFAULT '0',
  `status` int UNSIGNED NOT NULL DEFAULT '38'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- --------------------------------------------------------

--
-- Структура таблицы `pub_messages`
--

CREATE TABLE `pub_messages` (
  `id` bigint UNSIGNED NOT NULL,
  `author_id` bigint UNSIGNED DEFAULT NULL,
  `text` mediumtext COLLATE utf8mb4_general_ci NOT NULL,
  `published` bigint UNSIGNED NOT NULL DEFAULT '0',
  `status` int NOT NULL DEFAULT '34' COMMENT 'Флаги'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Дамп данных таблицы `pub_messages`
--

INSERT INTO `pub_messages` (`id`, `author_id`, `text`, `published`, `status`) VALUES
(1, 1, 'Приветсвую всех. Наш чат открылся. Поздравляю всех участников с данным событием!', 1691416012, 66),
(2, 2, 'Здарова! Мне кажется это событие надо отметить!', 1691416055, 66),
(3, 5, 'Предлагаю поехать на пикник', 1691416069, 66),
(4, 3, 'У меня есть бутылочка хорошего эля', 1691416087, 66),
(5, 4, 'А у меня есть отличная медовуха', 1691416104, 66),
(6, 6, 'шашлык кто берет?', 1691416222, 66);

-- --------------------------------------------------------

--
-- Структура таблицы `users`
--

CREATE TABLE `users` (
  `id` bigint UNSIGNED NOT NULL,
  `login` varchar(768) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `email` varchar(768) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `first_name` varchar(768) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `last_name` varchar(768) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci DEFAULT NULL,
  `registered` bigint UNSIGNED DEFAULT '0',
  `status` int UNSIGNED NOT NULL DEFAULT '2',
  `session_key` bigint UNSIGNED NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Дамп данных таблицы `users`
--

INSERT INTO `users` (`id`, `login`, `email`, `first_name`, `last_name`, `registered`, `status`, `session_key`) VALUES
(0, 'deleted', 'no', 'noname', '', 0, 16, 0),
(1, 'admin', 'admin@admin.ru', 'Администратор', 'Администраторов', 0, 2, 1245090619829347503),
(2, 'test', 'test@test.ru', 'Tester', 'Testerov', 0, 1, 12228371960314758897),
(3, 'olaf', 'Olaf@north.se', 'Olaf', 'Viking', 0, 1, 4165081690467800183),
(4, 'baleog', 'Baleog@north.se', 'Baleog', 'Viking', 0, 1, 0),
(5, 'typical', 'typical@typical.ru', 'ТипичноеИмя', 'ТипичнаяФамилия', 0, 1, 0),
(6, 'длинный', 'почта@русская.ру', 'Проверка_Поля_на_достаточно_длинную_и_нестандартную_фамилию', 'такое_же_длинное_имя_с_нестандартными_символами_#!@$#?\\_', 0, 1, 0);

--
-- Триггеры `users`
--
DELIMITER $$
CREATE TRIGGER `addhash` AFTER INSERT ON `users` FOR EACH ROW INSERT INTO `hash_tab` (`uid`, `hash`, `salt`) VALUES (NEW.id, '', '')
$$
DELIMITER ;
DELIMITER $$
CREATE TRIGGER `delete` BEFORE DELETE ON `users` FOR EACH ROW UPDATE `pub_messages` SET `status` = ((SELECT `pub_messages`.`status` WHERE `pub_messages`.`author_id` = OLD.id) | '16'), `author_id` = 0 WHERE `pub_messages`.`author_id` = OLD.id
$$
DELIMITER ;

--
-- Индексы сохранённых таблиц
--

--
-- Индексы таблицы `hash_tab`
--
ALTER TABLE `hash_tab`
  ADD PRIMARY KEY (`uid`),
  ADD UNIQUE KEY `uid` (`uid`);

--
-- Индексы таблицы `private_messages`
--
ALTER TABLE `private_messages`
  ADD PRIMARY KEY (`id`),
  ADD KEY `author_id` (`author_id`),
  ADD KEY `recipient_id` (`recipient_id`),
  ADD KEY `id` (`id`);

--
-- Индексы таблицы `pub_messages`
--
ALTER TABLE `pub_messages`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `id` (`id`),
  ADD KEY `author_id` (`author_id`);

--
-- Индексы таблицы `users`
--
ALTER TABLE `users`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `id` (`id`),
  ADD UNIQUE KEY `login` (`login`),
  ADD UNIQUE KEY `email` (`email`);

--
-- AUTO_INCREMENT для сохранённых таблиц
--

--
-- AUTO_INCREMENT для таблицы `private_messages`
--
ALTER TABLE `private_messages`
  MODIFY `id` bigint UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=10;

--
-- AUTO_INCREMENT для таблицы `pub_messages`
--
ALTER TABLE `pub_messages`
  MODIFY `id` bigint UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=1034;

--
-- AUTO_INCREMENT для таблицы `users`
--
ALTER TABLE `users`
  MODIFY `id` bigint UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=45;

--
-- Ограничения внешнего ключа сохраненных таблиц
--

--
-- Ограничения внешнего ключа таблицы `hash_tab`
--
ALTER TABLE `hash_tab`
  ADD CONSTRAINT `hash_tab_ibfk_1` FOREIGN KEY (`uid`) REFERENCES `users` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Ограничения внешнего ключа таблицы `private_messages`
--
ALTER TABLE `private_messages`
  ADD CONSTRAINT `private_messages_ibfk_1` FOREIGN KEY (`author_id`) REFERENCES `users` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `private_messages_ibfk_2` FOREIGN KEY (`recipient_id`) REFERENCES `users` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Ограничения внешнего ключа таблицы `pub_messages`
--
ALTER TABLE `pub_messages`
  ADD CONSTRAINT `pub_messages_ibfk_1` FOREIGN KEY (`author_id`) REFERENCES `users` (`id`) ON DELETE SET NULL ON UPDATE CASCADE;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;

# ------------------------------------------------------------
# 1. Основные параметры компилятора
# ------------------------------------------------------------

# 1.1 Тип компилятора
CXX = g++

# 1.2 Флаги компиляции
CXXFLAGS = -Wall -Wextra -O2 -std=c++23 -I./include -I./app/src

# 1.3 Имя итогового исполняемого файла
TARGET = Console_chat_by_Dima_Prihodko


# ------------------------------------------------------------
# 2. Пути и имена файлов
# Дерево проекта (упрощённо):
# ├── app/
# │   ├── main.cpp
# │   └── src/
# │       ├── Chat/
# │       │   └── utils/
# │       ├── Message/
# │       └── User/
# ├── Makefile
# ------------------------------------------------------------
# dimbo@dimbo:~/Programs/home_work_1/console-chat-by-Cpp$ ls -R
# .:
# app  Makefile  README.md

# ./app:
# main.cpp  src

# ./app/src:
# Chat  Message  User

# ./app/src/Chat:
# Chat.cpp  Chat.h  utils

# ./app/src/Chat/utils:
# Display.cpp  Display.h  Exceptions.h  Input.cpp  Input.h  Time.cpp  Time.h

# ./app/src/Message:
# Message.cpp  Message.h

# ./app/src/User:
# User.cpp  User.h
# ------------------------------------------------------------

# 2.1 Список .cpp-файлов
SOURCES = \
	app/main.cpp \
	app/src/Chat/Chat.cpp \
	app/src/Chat/utils/Display.cpp \
	app/src/Chat/utils/Input.cpp \
	app/src/Chat/utils/Time.cpp \
	app/src/Message/Message.cpp \
	app/src/User/User.cpp

# 2.2 Список объектов
OBJECTS = $(SOURCES:.cpp=.o)


# ------------------------------------------------------------
# 3. Цели и правила сборки
# ------------------------------------------------------------

# 3.1 Цель по умолчанию
all: $(TARGET)

# 3.2 Правило сборки финального бинарника
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 3.3 Правило сборки объектных файлов
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 3.4 Очистка
clean:
	rm -f $(OBJECTS) $(TARGET)

# 3.5 Запуск программы
run: $(TARGET)
	./$(TARGET)

# 3.6 Установка
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/$(TARGET)
	cp $(TARGET) /home/dimbo/Programs/home_work_1/bin/$(TARGET)

# 3.7 Удаление установленной программы
uninstall:
	rm -f /usr/local/bin/$(TARGET)
	rm -f /home/dimbo/Programs/home_work_1/bin/$(TARGET)


# ------------------------------------------------------------
# 4. Прочие цели
# ------------------------------------------------------------
help:
	@echo "Доступные цели:"
	@echo "  all       - Сборка всего проекта (по умолчанию)"
	@echo "  clean     - Удалить объектные файлы и бинарник"
	@echo "  run       - Запустить программу"
	@echo "  install   - Установить бинарник в /usr/local/bin (и др.)"
	@echo "  uninstall - Удалить установленный бинарник"
	@echo "  help      - Вывести это сообщение"

.PHONY: all clean run install uninstall help

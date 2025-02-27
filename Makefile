# ------------------------------------------------------------
# 1. Основные параметры компилятора
# ------------------------------------------------------------

# 1.1 Тип
CXX = g++

# 1.2 Флаги
CXXFLAGS = -Wall -Wextra -02 -I./src -std=c++23

# 1.3 Имя исполняемого файла
TARGET = Console_chat_by_Dima_Prihodko


# ------------------------------------------------------------
# 2. Пути и имена файлов
# Дерево проекта:
# ├── app/
# │   ├── src/
# │   │   ├── Chat/
# │   │   ├── Message/
# │   │   ├── User/
# │   │   └── utils/
# │   ├── main.cpp
# ├── Makefile
# ------------------------------------------------------------

# 2.1 Пути
SOURCES = \
main.cpp \
src/Chat/Chat.cpp \
src/Message/Message.cpp \
src/User/User.cpp \
src/Chat/utils/Display.cpp \
src/Chat/utils/Input.cpp \
src/Chat/utils/Time.cpp \

# 2.2 Имена
OBJECTS = $(SOURCES:.cpp=.o)


# ------------------------------------------------------------
# 3. Цели и правила сборки
# ------------------------------------------------------------

# 3.1 Цели
all: $(TARGET)

# 3.2 Правила
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 3.3 Правила
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 3.4 Правила
clean:
	rm -f $(OBJECTS) $(TARGET)
	
# 3.5 Правила	
run: $(TARGET)
	./$(TARGET)	

# 3.6 Правила
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/$(TARGET)
	cp $(TARGET) /home/dimbo/Programs/home_work_1/bin/$(TARGET)

# 3.7 Правила
uninstall:
	rm -f /usr/local/bin/$(TARGET)	
	rm -f /home/dimbo/Programs/home_work_1/bin/$(TARGET)


# ------------------------------------------------------------
# 4. Прочее
# ------------------------------------------------------------
help:
	@echo "Доступные цели:"
	@echo "  all       - Сборка всего проекта (по умолчанию)"
	@echo "  clean     - Удалить объектные файлы и бинарник"
	@echo "  run       - Запустить программу"
	@echo "  install   - Установить бинарник в /usr/local/bin"
	@echo "  uninstall - Удалить установленный бинарник"
	@echo "  help      - Вывести это сообщение"

.PHONY: all clean run install uninstall help
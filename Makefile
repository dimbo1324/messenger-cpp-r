CXX = g++
CXXFLAGS = -Wall -Wextra -O2 -std=c++23 -I./include -I./include/PlatformOSInfo -I./app/src
TARGET = Console_chat_by_Dima_Prihodko
SOURCES = \
	app/main.cpp \
	app/src/Chat/Chat.cpp \
	app/src/Chat/utils/Display.cpp \
	app/src/Chat/utils/Input.cpp \
	app/src/Chat/utils/Time.cpp \
	app/src/Message/Message.cpp \
	app/src/User/User.cpp \
	include/PlatformOSInfo/PlatformOSInfo.cpp
OBJECTS = $(SOURCES:.cpp=.o)
all: $(TARGET)
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
clean:
	rm -f $(OBJECTS) $(TARGET)
run: $(TARGET)
	./$(TARGET)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/$(TARGET)
	cp $(TARGET) /home/dimbo/Programs/home_work_1/bin/$(TARGET)
uninstall:
	rm -f /usr/local/bin/$(TARGET)
	rm -f /home/dimbo/Programs/home_work_1/bin/$(TARGET)
help:
	@echo "Доступные цели:"
	@echo "  all       - Сборка всего проекта (по умолчанию)"
	@echo "  clean     - Удалить объектные файлы и бинарник"
	@echo "  run       - Запустить программу"
	@echo "  install   - Установить бинарник в /usr/local/bin (и др.)"
	@echo "  uninstall - Удалить установленный бинарник"
	@echo "  help      - Вывести это сообщение"
.PHONY: all clean run install uninstall help

CXX      := g++
SRCDIR   := src
INCDIR   := include
TARGET   := chat_server

LIB_TCP_DIR       := ../libs/tcp
LIB_THREADING_DIR := ../libs/threading

# Список исходных файлов
SRCS     := $(SRCDIR)/main.cpp \
            $(SRCDIR)/Server.cpp \
            $(SRCDIR)/ClientHandler.cpp \
            $(SRCDIR)/Logger.cpp \
            $(SRCDIR)/Database.cpp

CXXFLAGS := -std=c++17 \
            -I$(INCDIR) \
            -I$(LIB_TCP_DIR)/include \
            -I$(LIB_THREADING_DIR)/include \
            -I/ucrt64/include

LDFLAGS  := -L$(LIB_TCP_DIR)       -ltcp \
            -L$(LIB_THREADING_DIR) -lthreading \
            -lws2_32               \
            -lMswsock              \
            -lAdvapi32             \
            -L/ucrt64/lib -lpqxx -lpq \
            -pthread

.PHONY: all build libs server run clean

all: build

build: libs server

libs:
	$(MAKE) -C $(LIB_TCP_DIR)
	$(MAKE) -C $(LIB_THREADING_DIR)

server:
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

run: build
	./$(TARGET) 0.0.0.0 8080

clean:
	rm -f $(TARGET)
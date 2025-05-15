CXX      := g++
SRCDIR   := src
INCDIR   := include
TARGET   := chat_server

LIB_TCP_DIR       := ../libs/tcp
LIB_THREADING_DIR := ../libs/threading

CXXFLAGS := -std=c++17 \
            -I$(INCDIR) \
            -I$(LIB_TCP_DIR)/include \
            -I$(LIB_THREADING_DIR)/include

LDFLAGS  := -L$(LIB_TCP_DIR)       -ltcp \
            -L$(LIB_THREADING_DIR) -lthreading \
            -lws2_32               \
            -lMswsock              \
            -lAdvapi32             \
            -pthread

.PHONY: all build libs server run clean

all: build

build: libs server

libs:
	$(MAKE) -C $(LIB_TCP_DIR)
	$(MAKE) -C $(LIB_THREADING_DIR)

server:
	$(CXX) $(CXXFLAGS) \
	  -o $(TARGET) \
	  $(SRCDIR)/main.cpp \
	  $(SRCDIR)/Server.cpp \
	  $(SRCDIR)/ClientHandler.cpp \
	  $(SRCDIR)/Logger.cpp \
	  $(LDFLAGS)

run: build
	./$(TARGET) 0.0.0.0 8080

clean:
	rm -f $(TARGET)

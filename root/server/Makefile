CXX      = g++
SRCDIR   = src
INCDIR   = include
TARGET   = chat_server

CXXFLAGS = -std=c++17 \
           -I$(INCDIR) \
           -I../libs/tcp/include \
           -I../libs/threading/include

LDFLAGS  = -L../libs/tcp       -ltcp \
           -L../libs/threading -lthreading \
           -lws2_32 \
           -lMswsock \
           -lAdvapi32 \
           -pthread

.PHONY: all build libs server run clean

all: build

build: libs server

libs:
	$(MAKE) -C ../libs/tcp
	$(MAKE) -C ../libs/threading

server:
	$(CXX) $(CXXFLAGS) \
	  -o $(TARGET) \
	  $(SRCDIR)/main.cpp $(SRCDIR)/Server.cpp \
      $(SRCDIR)/ClientHandler.cpp $(SRCDIR)/Logger.cpp \
	  $(LDFLAGS)

run: build
	./$(TARGET) 0.0.0.0 8080

clean:
	rm -f $(TARGET)

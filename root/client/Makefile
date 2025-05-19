CXX      = g++
SRCDIR   = src
INCDIR   = include
TARGET   = chat_client
CXXFLAGS = -std=c++17 \
           -I$(INCDIR) \
           -I../libs/tcp/include \
           -I../libs/threading/include
LDFLAGS  = -L../libs/tcp       -ltcp \
           -L../libs/threading -lthreading \
           -lws2_32              \
           -lMswsock             \
           -lAdvapi32            \
           -pthread
.PHONY: all build libs client run clean
all: build
build: libs client
libs:
	$(MAKE) -C ../libs/tcp
	$(MAKE) -C ../libs/threading
client:
	$(CXX) $(CXXFLAGS) \
	  -o $(TARGET) \
	  $(SRCDIR)/main.cpp $(SRCDIR)/Client.cpp $(SRCDIR)/UI.cpp \
	  $(LDFLAGS)
run: build
	./$(TARGET) 127.0.0.1:8080
clean:
	rm -f $(TARGET)

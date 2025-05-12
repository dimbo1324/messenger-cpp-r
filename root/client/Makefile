CXX = g++
CXXFLAGS = -std=c++17 -I../libs/tcp/include -I../libs/threading/include
LDFLAGS = -L../libs/tcp -ltcp -L../libs/threading -lthreading -pthread
TARGET = chat_client
SRCDIR = src
INCDIR = include
all: build
build: libs client
libs:
	$(MAKE) -C ../libs/tcp
	$(MAKE) -C ../libs/threading
client:
	$(CXX) $(CXXFLAGS) -o $(TARGET) \
	  $(SRCDIR)/main.cpp $(SRCDIR)/Client.cpp $(SRCDIR)/UI.cpp \
	  $(LDFLAGS)
run: build
	./$(TARGET) 127.0.0.1:8080
clean:
	-rm -f $(TARGET)

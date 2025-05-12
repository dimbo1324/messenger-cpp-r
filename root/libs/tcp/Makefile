CXX := g++
AR  := ar
CXXFLAGS := -std=c++17 -Wall -Iinclude
SRC := $(wildcard src/*.cpp)
OBJ := $(SRC:src/%.cpp=build/%.o)
TARGET := libtcp.a
all: $(TARGET)
build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(TARGET): $(OBJ)
	$(AR) rcs $@ $^
build:
	mkdir -p build
clean:
	rm -rf build $(TARGET)
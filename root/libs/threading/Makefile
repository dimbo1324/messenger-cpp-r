CXX := C:/msys64/ucrt64/bin/g++.exe
AR  := C:/msys64/ucrt64/bin/ar.exe
CXXFLAGS := -std=c++17 -Wall -Iinclude
SRC := $(wildcard src/*.cpp)
OBJ := $(patsubst src/%.cpp,build/%.o,$(SRC))
TARGET := libthreading.a

all: $(TARGET)

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	$(AR) rcs $@ $^

build:
	mkdir -p build

clean:
	rm -rf build $(TARGET)
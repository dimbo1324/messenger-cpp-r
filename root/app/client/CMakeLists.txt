cmake_minimum_required(VERSION 3.21)
project(chat_client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(chat_client
    src/main.cpp
    src/Client.cpp
    src/UI.cpp
)

target_include_directories(chat_client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(chat_client PRIVATE
    tcp
    threading
)

message(STATUS "chat_client target created")

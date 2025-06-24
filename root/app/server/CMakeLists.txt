cmake_minimum_required(VERSION 3.21)
project(chat_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBPQXX REQUIRED libpqxx)

add_library(server_core
    src/Server.cpp
    src/ClientHandler.cpp
    src/Database.cpp
    src/Logger.cpp
)

target_include_directories(server_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(server_core PRIVATE
    tcp
    threading
    ${LIBPQXX_LIBRARIES}
)
target_compile_options(server_core PRIVATE ${LIBPQXX_CFLAGS})

add_executable(chat_server
    src/main.cpp
)
target_link_libraries(chat_server PRIVATE server_core)

message(STATUS "chat_server and server_core targets created")

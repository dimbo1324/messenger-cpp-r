cmake_minimum_required(VERSION 3.21)
project(chat_server LANGUAGES CXX)

find_package(libpqxx CONFIG QUIET)
if(NOT libpqxx_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBPQXX REQUIRED libpqxx)
endif()

add_library(server_core
    src/Auth.cpp
    src/Config.cpp
    src/Server.cpp
    src/ClientHandler.cpp
    src/Database.cpp
    src/Logger.cpp
    src/SessionRegistry.cpp
)

target_include_directories(server_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(server_core PUBLIC
    tcp
    threading
)

if(TARGET project_options)
    target_link_libraries(server_core PUBLIC project_options)
endif()

if(libpqxx_FOUND)
    target_link_libraries(server_core PRIVATE libpqxx::pqxx)
else()
    target_include_directories(server_core PRIVATE ${LIBPQXX_INCLUDE_DIRS})
    target_link_directories(server_core PRIVATE ${LIBPQXX_LIBRARY_DIRS})
    target_link_libraries(server_core PRIVATE ${LIBPQXX_LIBRARIES})
    target_compile_options(server_core PRIVATE ${LIBPQXX_CFLAGS_OTHER})
endif()

add_executable(chat_server
    src/main.cpp
)
target_link_libraries(chat_server PRIVATE server_core)

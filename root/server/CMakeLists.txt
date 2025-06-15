cmake_minimum_required(VERSION 3.16)
project(chat_server)

set(CMAKE_CXX_STANDARD 17)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBPQXX REQUIRED libpqxx)

add_library(server_core
    src/Server.cpp
    src/ClientHandler.cpp
    src/Database.cpp
    src/Logger.cpp
)
target_include_directories(server_core PUBLIC include)
target_link_libraries(server_core
    PRIVATE
      tcp             
      threading       
      ${LIBPQXX_LIBRARIES}
)
target_compile_options(server_core PRIVATE ${LIBPQXX_CFLAGS})

add_executable(chat_server src/main.cpp)
target_link_libraries(chat_server PRIVATE server_core)

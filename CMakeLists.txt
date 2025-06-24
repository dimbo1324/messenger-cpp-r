cmake_minimum_required(VERSION 3.21)
project(cpp_messenger LANGUAGES CXX)

message(STATUS "========================================")
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Build configuration:")
message(STATUS "  CMake version: ${CMAKE_VERSION}")
message(STATUS "  Source dir:    ${CMAKE_SOURCE_DIR}")
message(STATUS "  Build dir:     ${CMAKE_BINARY_DIR}")
message(STATUS "  Compiler:      ${CMAKE_CXX_COMPILER}")
message(STATUS "  Compiler ID:   ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "  Compiler ver:  ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "========================================")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(QT_PATH "" CACHE PATH "Path to Qt installation (root of Qt6)")
if(QT_PATH)
    list(APPEND CMAKE_PREFIX_PATH "${QT_PATH}")
    message(STATUS "Qt path (from cache): ${QT_PATH}")
endif()

add_subdirectory(root/app/libs/tcp)
add_subdirectory(root/app/libs/threading)

add_subdirectory(root/app/client)

add_subdirectory(root/app/server)

add_subdirectory(root/serverGUI)


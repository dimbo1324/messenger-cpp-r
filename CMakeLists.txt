cmake_minimum_required(VERSION 3.21)
project(cpp_messenger LANGUAGES CXX)

option(BUILD_CLIENT "Build console chat client" ON)
option(BUILD_SERVER "Build chat server" ON)
option(BUILD_GUI "Build Qt admin GUI" ON)
option(BUILD_TESTS "Build unit tests" ON)
option(USE_STUB "Build GUI with stub data instead of the real server protocol" OFF)
option(ENABLE_TLS "Reserve TLS-related configuration hooks. Full TLS transport is not implemented yet." OFF)
option(ENABLE_WARNINGS "Enable stricter compiler warnings" ON)
option(ENABLE_SANITIZERS "Enable Address/Undefined sanitizers for supported Debug builds" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(QT_PATH "" CACHE PATH "Path to Qt installation (root of Qt6)")
if(QT_PATH)
    list(APPEND CMAKE_PREFIX_PATH "${QT_PATH}")
    message(STATUS "Qt path (from cache): ${QT_PATH}")
endif()

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)

if(ENABLE_WARNINGS)
    if(MSVC)
        target_compile_options(project_options INTERFACE /W4 /permissive-)
    else()
        target_compile_options(project_options INTERFACE -Wall -Wextra -Wpedantic)
    endif()
endif()

if(ENABLE_SANITIZERS AND NOT MSVC)
    target_compile_options(project_options INTERFACE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(project_options INTERFACE -fsanitize=address,undefined)
endif()

if(ENABLE_TLS)
    target_compile_definitions(project_options INTERFACE MESSENGER_TLS_CONFIG_ENABLED=1)
endif()

add_subdirectory(root/app/libs/tcp)
add_subdirectory(root/app/libs/threading)

if(BUILD_CLIENT)
    add_subdirectory(root/app/client)
endif()

if(BUILD_SERVER)
    add_subdirectory(root/app/server)
endif()

if(BUILD_GUI)
    add_subdirectory(root/serverGUI)
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

cmake_minimum_required(VERSION 3.10)
project(threading LANGUAGES CXX)

set(THREADING_SOURCES
    src/Logger.cpp
    src/ThreadPool.cpp
)

add_library(threading STATIC ${THREADING_SOURCES})

target_include_directories(threading PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_library(threading::threading ALIAS threading)
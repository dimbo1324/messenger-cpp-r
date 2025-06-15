cmake_minimum_required(VERSION 3.10)
project(tcp LANGUAGES CXX)

set(TCP_SOURCES
    src/SocketFactory.cpp
    src/TcpSocketLinux.cpp
    src/TcpSocketWin.cpp
)

add_library(tcp STATIC ${TCP_SOURCES})

target_include_directories(tcp PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_library(tcp::tcp ALIAS tcp)
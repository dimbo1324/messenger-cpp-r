cmake_minimum_required(VERSION 3.21)
project(tcp LANGUAGES CXX)

add_library(tcp STATIC
    src/Protocol.cpp
    src/SocketFactory.cpp
    src/SocketOps.cpp
    src/TcpSocketLinux.cpp
    src/TcpSocketWin.cpp
)

target_include_directories(tcp PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(TARGET project_options)
    target_link_libraries(tcp PUBLIC project_options)
endif()

if(WIN32)
    target_link_libraries(tcp PUBLIC ws2_32)
endif()

add_library(tcp::tcp ALIAS tcp)

cmake_minimum_required(VERSION 3.16)
project(server_gui LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBPQXX REQUIRED libpqxx)

set(SERVER_GUI_SOURCES
    src/main.cpp
    src/mainwindowserver.cpp
)

set(SERVER_GUI_HEADERS
    include/MainWindowServer.h
)

set(SERVER_GUI_UIS
    ui/MainWindowServer.ui
)

add_executable(server_gui
    ${SERVER_GUI_SOURCES}
    ${SERVER_GUI_HEADERS}
    ${SERVER_GUI_UIS}
)

target_include_directories(server_gui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/libs/tcp/include
    ${CMAKE_SOURCE_DIR}/libs/threading/include
    ${LIBPQXX_INCLUDE_DIRS}
)

target_link_libraries(server_gui PRIVATE
    Qt6::Widgets
    server_core
    tcp
    threading
    ${LIBPQXX_LIBRARIES}
)

target_compile_options(server_gui PRIVATE ${LIBPQXX_CFLAGS})

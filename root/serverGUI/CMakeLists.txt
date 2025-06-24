cmake_minimum_required(VERSION 3.21)
project(server_gui LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui")

find_package(Qt6 REQUIRED COMPONENTS Widgets Network)
message(STATUS "Qt6 version: ${Qt6_VERSION}")

file(GLOB SERVERGUI_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB SERVERGUI_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
file(GLOB SERVERGUI_UIS "${CMAKE_CURRENT_SOURCE_DIR}/ui/*.ui")

add_executable(server_gui
    ${SERVERGUI_SOURCES}
    ${SERVERGUI_HEADERS}
    ${SERVERGUI_UIS}
)

target_include_directories(server_gui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(server_gui PRIVATE
    Qt6::Widgets
    Qt6::Network
)


target_compile_definitions(server_gui PRIVATE USE_STUB)

if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
endif()

message(STATUS "server_gui target created")

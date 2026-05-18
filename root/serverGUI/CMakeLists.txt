cmake_minimum_required(VERSION 3.21)
project(server_gui LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui")

find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

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

if(TARGET project_options)
    target_link_libraries(server_gui PRIVATE project_options)
endif()

if(USE_STUB)
    target_compile_definitions(server_gui PRIVATE USE_STUB)
endif()

if(WIN32)
    set_target_properties(server_gui PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

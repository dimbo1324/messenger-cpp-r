cmake_minimum_required(VERSION 3.21)  # Qt 6 рекомендует минимум 3.21
project(server_gui LANGUAGES CXX)

# Детальная диагностика компилятора
message(STATUS "========================================")
message(STATUS "Build configuration:")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Compiler version: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "========================================")

# Настройки стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Автоматические системы Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui")

# Явное указание пути к Qt
set(QT_PATH "D:/QtHome/6.5.3/msvc2019_64")
list(APPEND CMAKE_PREFIX_PATH "${QT_PATH}")
message(STATUS "Qt path: ${QT_PATH}")

# Поиск Qt с диагностикой, добавлен Network
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Network)
message(STATUS "Qt version: ${Qt6_VERSION}")
message(STATUS "Qt libraries: ${Qt6_LIBRARIES}")

# Подпроекты
add_subdirectory(
    ${CMAKE_SOURCE_DIR}/../libs/tcp
    ${CMAKE_BINARY_DIR}/libs/tcp_build
)

add_subdirectory(
    ${CMAKE_SOURCE_DIR}/../libs/threading
    ${CMAKE_BINARY_DIR}/libs/threading_build
)

# Источники
file(GLOB SERVER_GUI_SOURCES "src/*.cpp")
file(GLOB SERVER_GUI_HEADERS "include/*.h")
file(GLOB SERVER_GUI_UIS "ui/*.ui")

# Исполняемый файл
add_executable(server_gui
    ${SERVER_GUI_SOURCES}
    ${SERVER_GUI_HEADERS}
    ${SERVER_GUI_UIS}
)

# Директории include
target_include_directories(server_gui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/../libs/tcp/include
    ${CMAKE_SOURCE_DIR}/../libs/threading/include
    ${CMAKE_CURRENT_BINARY_DIR}  # для ui_*.h
)

# Определения препроцессора
add_compile_definitions(USE_STUB)

# Линковка
target_link_libraries(server_gui PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Network
    tcp
    threading
)

# Дополнительные настройки для Windows
if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
endif()

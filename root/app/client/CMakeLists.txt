cmake_minimum_required(VERSION 3.21)
project(chat_client LANGUAGES CXX)

add_executable(chat_client
    src/main.cpp
    src/Client.cpp
    src/UI.cpp
)

target_include_directories(chat_client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(chat_client PRIVATE
    tcp
    threading
)

if(TARGET project_options)
    target_link_libraries(chat_client PRIVATE project_options)
endif()

cmake_minimum_required(VERSION 3.6)
project(so_pro)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        src/main.cpp src/common.hpp
        src/Arguments.cpp src/Arguments.hpp
        src/GUI.cpp src/GUI.hpp
        src/Semaphore.cpp src/Semaphore.hpp
        src/URL.cpp src/URL.hpp
        )
add_executable(so_pro ${SOURCE_FILES})

target_link_libraries(so_pro
        boost_filesystem
        boost_program_options
        boost_system
        curl
        ncurses
        pthread)

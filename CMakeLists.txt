cmake_minimum_required(VERSION 3.6)
project(so_pro)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp)
add_executable(so_pro ${SOURCE_FILES})

target_link_libraries(so_pro ncurses)
FIND_PACKAGE(Boost COMPONENTS program_options REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(so_pro ${Boost_LIBRARIES})

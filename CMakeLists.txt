cmake_minimum_required(VERSION 3.6)
project(so_pro)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp URL.cpp URL.hpp Semaphore.cpp Semaphore.hpp GUI.cpp GUI.hpp Arguments.cpp Arguments.hpp)
add_executable(so_pro ${SOURCE_FILES})

# ncurses
target_link_libraries(so_pro ncurses)

# pthread
target_link_libraries(so_pro pthread)

# boost
#find_package(Boost COMPONENTS program_options REQUIRED)
#find_package(Boost COMPONENTS filesystem REQUIRED)
#INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
#target_link_libraries(so_pro ${Boost_LIBRARIES})
target_link_libraries(so_pro boost_program_options)
target_link_libraries(so_pro boost_filesystem)
target_link_libraries(so_pro boost_system)
target_link_libraries(so_pro curl)

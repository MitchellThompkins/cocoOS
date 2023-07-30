cmake_minimum_required(VERSION 3.22)

#add_compile_options(-std=c++11 -Wall -Werror -v)
#add_compile_options(-std=c++11 -v)

project(test
    LANGUAGES CXX C ASM
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(FORCE_COLORED_OUPTUT "Always produce ANSI-colored output" ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# Print more options
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(test)

add_library(cocoOS
    src/os_assert.c
    src/os_cbk.c
    src/os_event.c
    src/os_kernel.c
    src/os_msgqueue.c
    src/os_sem.c
    src/os_task.c
)

target_include_directories(cocoOS
    PUBLIC
        inc)

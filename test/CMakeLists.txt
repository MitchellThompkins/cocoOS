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

#add_subdirectory(..)
#add_subdirectory(test)
add_subdirectory(platforms)

include(FetchContent)
FetchContent_Declare(
    CppUTest
    GIT_REPOSITORY https://github.com/cpputest/cpputest.git
    GIT_TAG        master # or use release tag, eg. v4.0
)

# Set this to ON if you want to have the CppUTests in your project as well.
set(TESTS OFF CACHE BOOL "Switch off CppUTest Test build")
FetchContent_MakeAvailable(CppUTest)

add_executable(test
    src/main.cpp
)

target_link_libraries(test
    PRIVATE
        platform
        CppUTest::CppUTest
        CppUTest::CppUTestExt
)


add_custom_command(
    TARGET test POST_BUILD
    COMMAND
        "${CMAKE_OBJCOPY}"
        --output-target binary
        --only-section=.entry
        --only-section=.text
        --only-section=.rodata
        --only-section=.ARM.exidx
        --only-section=.ARM.bss
        --only-section=.ARM.data
        "$<TARGET_FILE:test>"
        "$<TARGET_FILE_DIR:test>/test.bin"
    VERBATIM
)

target_link_options(test PRIVATE "-Wl,-Map=$<TARGET_FILE_DIR:test>/test.map")


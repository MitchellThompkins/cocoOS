if(DEFINED BUILD_COCOOS_UNIT_TESTS)
    cmake_minimum_required(VERSION 3.22)

    project(test
        LANGUAGES CXX C ASM
        )

    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

    add_definitions(-DCOCOOS_UNIT_TESTS)

    option(FORCE_COLORED_OUPTUT "Always produce ANSI-colored output" ON)

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    # Print more options
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    set(CMAKE_VERBOSE_MAKEFILE ON)

    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

    add_subdirectory(tests)
endif()

add_subdirectory(os)

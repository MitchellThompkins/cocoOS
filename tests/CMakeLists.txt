add_subdirectory(platforms)
add_subdirectory(mocks)

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
    src/os_task.test.cpp
)

target_link_libraries(test
    PRIVATE
        platform
        mocks
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

add_subdirectory(platforms)
add_subdirectory(os_task)
add_subdirectory(os_event)
add_subdirectory(os_kernel)
add_subdirectory(os_sem)

#TODO(@mthompkins): Compile with -Wall
#add_compile_options(-std=c++14 -Wall -Werror -v)
add_compile_options(-std=c++14)

include(FetchContent)
FetchContent_Declare(
    CppUTest
    GIT_REPOSITORY https://github.com/cpputest/cpputest.git
    GIT_TAG        master # or use release tag, eg. v4.0
)

# Set this to ON if you want to have the CppUTests in your project as well.
set(TESTS OFF CACHE BOOL "Switch off CppUTest Test build")
FetchContent_MakeAvailable(CppUTest)


add_library(user_os_config INTERFACE)
target_include_directories(user_os_config
    INTERFACE
        user_config
)

add_library(cpputest_main
    src/main.cpp

)

target_include_directories(cpputest_main
    PUBLIC
        include
)

target_link_libraries(cpputest_main
    PUBLIC
        CppUTest::CppUTest
        CppUTest::CppUTestExt
)

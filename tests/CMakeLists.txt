add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-std=c++14> -Wall)

add_subdirectory(platforms)
add_subdirectory(os_task)
add_subdirectory(os_event)
add_subdirectory(os_kernel)
add_subdirectory(os_sem)
add_subdirectory(os_msgqueue)
add_subdirectory(os_utils)
add_subdirectory(integration)

include(FetchContent)
FetchContent_Declare(
    CppUTest
    GIT_REPOSITORY https://github.com/cpputest/cpputest.git
    GIT_TAG        79c066ef # last verified-working master commit; v4.0 lacks CppUTest::CppUTest alias
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

add_subdirectory(arm)
add_subdirectory(x86_64)

target_include_directories(platform
    PUBLIC
        include
)

target_sources(platform
    PUBLIC
        src/system_time.c
)

target_link_libraries(platform
    PUBLIC
        os_kernel_intf
)

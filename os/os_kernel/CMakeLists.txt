add_library(os_kernel
    src/os_kernel.c
)

target_include_directories(os_kernel
    PRIVATE
        include
)

target_link_libraries(os_kernel
    PRIVATE
        os_task
)

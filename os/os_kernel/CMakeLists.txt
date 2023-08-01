# Interface
add_library(os_kernel_intf INTERFACE)
target_include_directories(os_kernel_intf
    INTERFACE
        include
)

# Implementation
add_library(os_kernel_impl
    src/os_kernel.c
)
target_link_libraries(os_kernel_impl
    PUBLIC
        os_kernel_intf
        os_task_intf
)

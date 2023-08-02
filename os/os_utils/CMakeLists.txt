# Interface
add_library(os_utils_intf INTERFACE)
target_include_directories(os_utils_intf
    INTERFACE
        include
)

# Implementation
add_library(os_utils_impl
    src/os_assert.c
)
target_link_libraries(os_utils_impl
    PUBLIC
        os_utils_intf
        os_kernel_intf
)

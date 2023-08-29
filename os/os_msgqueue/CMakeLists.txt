# Interface
add_library(os_msgqueue_intf INTERFACE)
target_include_directories(os_msgqueue_intf
    INTERFACE
        include
)
target_link_libraries(os_msgqueue_intf
    INTERFACE
        os_event_intf
        os_utils_intf
)

# Implementation
add_library(os_msgqueue_impl
    src/os_msgqueue.c
)
target_link_libraries(os_msgqueue_impl
    PUBLIC
        os_msgqueue_intf
        os_kernel_intf
)

add_subdirectory(mocks)

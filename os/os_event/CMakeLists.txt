# Interface
add_library(os_event_intf INTERFACE)
target_include_directories(os_event_intf
    INTERFACE
        include
)

# Implementation
add_library(os_event_impl
    src/os_event.c
)
target_link_libraries(os_event_impl
    PRIVATE
        os_event_intf
        os_kernel_intf
)

# Interface
add_library(os_msgqueue_intf INTERFACE)
target_include_directories(os_msgqueue_intf
    INTERFACE
        include
)

# Implementation
add_library(os_msgqueue_impl
    src/os_msgqueue.c
)
target_link_libraries(os_msgqueue_impl
    PUBLIC
        os_msgqueue_intf
)

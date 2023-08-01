# Interface
add_library(os_task_intf INTERFACE)
target_include_directories(os_task_intf
    INTERFACE
        include
)
target_link_libraries(os_task_intf
    INTERFACE
        os_msgqueue_intf
        os_event_intf
        os_sem_intf
        os_utils_intf
)

# Implementation
add_library(os_task_impl
    src/os_task.c
)
target_link_libraries(os_task_impl
    PUBLIC
        os_task_intf
        os_sem_intf
        os_utils_intf
)

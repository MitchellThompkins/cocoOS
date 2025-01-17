# Interface
add_library(os_kernel_intf INTERFACE)
target_include_directories(os_kernel_intf
    INTERFACE
        include
)
target_link_libraries(os_kernel_intf
    INTERFACE
        os_msgqueue_intf
        os_task_intf
        os_sem_intf
)

# Implementation
add_library(os_kernel_impl
    src/os_kernel.c
    src/os_cbk.c
)
target_link_libraries(os_kernel_impl
    PUBLIC
        os_kernel_intf
        os_task_intf
        os_utils_intf
        os_event_intf
)

if(DEFINED BUILD_COCOOS_UNIT_TESTS)
    add_subdirectory(mocks)
endif()

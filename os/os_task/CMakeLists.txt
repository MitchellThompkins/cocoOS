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
        #TODO(@tthompkins): review if this needs to be mock for building unit
        #testsing
        os_msgqueue_impl
        os_task_intf
        os_sem_intf
        os_utils_intf
        os_kernel_intf #TODO(@tthompkins doesn't this conflict if I call the mock versio too?
)

if(DEFINED BUILD_COCOOS_UNIT_TESTS)
    add_subdirectory(mocks)
endif()

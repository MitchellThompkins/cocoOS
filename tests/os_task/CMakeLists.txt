add_executable(test_os_task
    src/os_task.test.cpp
)

add_library(user_os_config INTERFACE)
target_include_directories(user_os_config
    INTERFACE
        include
)

target_link_libraries(test_os_task
    PUBLIC
        cpputest_main
        platform

        user_os_config

        #TODO(@mthompkins): Do I need to add the intf first?
        #os_msgqueue_intf
        #os_event_intf
        #os_sem_intf
        #os_task_intf
        #os_kernel_intf

        # The linkage order matters b/c the impl's don't depend on each other
        os_kernel_impl_mock
        os_msgqueue_impl
        os_event_impl
        os_sem_impl
        os_task_impl

        # Mocks
        os_utils_impl_mock
)

include(make_test)
make_test(test_os_task)

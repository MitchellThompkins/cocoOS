add_executable(test_os_msgqueue
    src/os_msgqueue.test.cpp
)

target_link_libraries(test_os_msgqueue
    PUBLIC
        cpputest_main
        platform

        user_os_config

        # The linkage order matters b/c the impl's don't depend on each other
        os_msgqueue_impl

        # Mocks
        os_event_impl_mock
        os_utils_impl_mock
        os_kernel_impl_mock
        os_task_impl_mock
)


include(make_test)
make_test(test_os_msgqueue)

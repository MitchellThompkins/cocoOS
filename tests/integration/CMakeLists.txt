add_executable(test_integration
    src/integration.test.cpp
)

target_link_libraries(test_integration
    PUBLIC
        cpputest_main
        platform
        user_os_config

        # All real impls — no mocks; the point is that macros compile and run
        os_kernel_impl
        os_task_impl
        os_event_impl
        os_sem_impl
        os_msgqueue_impl
        os_utils_impl
)

include(make_test)
make_test(test_integration)

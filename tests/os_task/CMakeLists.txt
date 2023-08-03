add_executable(test_os_task
    src/os_task.test.cpp
)

target_link_libraries(test_os_task
    PUBLIC
        cpputest_main
        platform

        # The linkage order matters :(
        os_kernel_impl
        os_event_impl
        os_msgqueue_impl
        os_sem_impl
        os_task_impl

        #os_utils_impl
        os_utils_impl_mock

        #CppUTest::CppUTest
        #CppUTest::CppUTestExt
)

include(make_test)
make_test(test_os_task)

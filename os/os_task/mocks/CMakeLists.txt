# Mock Implementation
add_library(os_task_impl_mock
    src/mock_os_task.c
)
target_link_libraries(os_task_impl_mock
    PUBLIC
        os_task_intf
        os_event_intf
        os_msgqueue_intf
        os_sem_intf

        CppUTest::CppUTest
        CppUTest::CppUTestExt
)

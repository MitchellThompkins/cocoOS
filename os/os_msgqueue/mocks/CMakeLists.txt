# Mock Implementation
add_library(os_msgqueue_impl_mock
    src/mock_os_msgqueue.c
)
target_link_libraries(os_msgqueue_impl_mock
    PUBLIC
        os_utils_intf
        os_msgqueue_intf

        CppUTest::CppUTest
        CppUTest::CppUTestExt
)

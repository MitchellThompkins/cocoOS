# Mock Implementation
add_library(os_sem_impl_mock
    src/mock_os_sem.c
)
target_link_libraries(os_sem_impl_mock
    PUBLIC
        os_sem_intf
        os_kernel_intf

        CppUTest::CppUTest
        CppUTest::CppUTestExt
)

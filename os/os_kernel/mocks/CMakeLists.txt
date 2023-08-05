# Mock Implementation
add_library(os_kernel_impl_mock
    src/mock_os_kernel.c
)
target_link_libraries(os_kernel_impl_mock
    PUBLIC
        os_utils_intf
        os_kernel_intf

        CppUTest::CppUTest
        CppUTest::CppUTestExt
)

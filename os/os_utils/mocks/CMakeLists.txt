# Mock Implementation
add_library(os_utils_impl_mock
    src/mock_os_assert.c
)
target_link_libraries(os_utils_impl_mock
    PUBLIC
        os_utils_intf
        os_kernel_intf

        CppUTest::CppUTest
        CppUTest::CppUTestExt
)

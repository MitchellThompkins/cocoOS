# Mock Implementation
add_library(os_event_impl_mock
    src/mock_os_event.c
)
target_link_libraries(os_event_impl_mock
    PUBLIC
        os_utils_intf
        os_event_intf

        CppUTest::CppUTest
        CppUTest::CppUTestExt
)

add_executable(test_os_utils
    src/os_utils.test.cpp
)

target_link_libraries(test_os_utils
    PUBLIC
        cpputest_main
        platform
        user_os_config
        os_kernel_impl_mock
        os_utils_impl
)

include(make_test)
make_test(test_os_utils)

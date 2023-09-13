add_executable(test_os_event
    src/os_event.test.cpp
)

target_link_libraries(test_os_event
    PUBLIC
        cpputest_main
        platform

        user_os_config

        #TODO(@mthompkins): Do I need to add the intf first?
        #os_msgqueue_intf
        #os_event_intf
        #os_sem_intf
        #os_task_intf
        #os_event_intf

        # The linkage order matters b/c the impl's don't depend on each other
        os_event_impl

        # Mocks
        os_utils_impl_mock
        os_msgqueue_impl_mock
        os_kernel_impl_mock
        os_task_impl_mock
        os_sem_impl_mock

)


include(make_test)
make_test(test_os_event)

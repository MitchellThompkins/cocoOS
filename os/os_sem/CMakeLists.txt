# Interface
add_library(os_sem_intf INTERFACE)
target_include_directories(os_sem_intf
    INTERFACE
        include
)

# Implementation
add_library(os_sem_impl
    src/os_sem.c
)
target_link_libraries(os_sem_impl
    PUBLIC
        os_sem_intf
        os_utils_intf
)

add_subdirectory(mocks)

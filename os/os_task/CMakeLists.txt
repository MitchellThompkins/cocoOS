add_library(os_task
    src/os_task.c
)

target_include_directories(os_task
    PRIVATE
        include
)

target_link_libraries(os_task
    PRIVATE
        os_utils
)

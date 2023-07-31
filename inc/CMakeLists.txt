target_include_directories(cocoOS
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(os_assert
    PUBLIC
        os_assert.h
)

target_include_directories(os_kernel
    PUBLIC
        os_applAPI.h
)

target_include_directories(os_task
    PUBLIC
        os_task.h
)

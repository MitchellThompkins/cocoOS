add_subdirectory(os_event)
add_subdirectory(os_kernel)
add_subdirectory(os_msgqueue)
add_subdirectory(os_sem)
add_subdirectory(os_task)
add_subdirectory(os_utils)


add_library(cocoOs
        os_event/src/os_event.c
        os_kernel/src/os_kernel.c
        os_kernel/src/os_cbk.c
        os_msgqueue/src/os_msgqueue.c
        os_sem/src/os_sem.c
        os_task/src/os_task.c
        os_utils/src/os_assert.c
)

target_include_directories(cocoOs
    PUBLIC
        os_event/include/
        os_kernel/include/
        os_msgqueue/include/
        os_sem/include/
        os_task/include/
        os_utils/include/
)

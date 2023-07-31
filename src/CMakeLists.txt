add_library(cocoOS
    os_assert.c
    os_cbk.c
    os_event.c
    os_kernel.c
    os_msgqueue.c
    os_sem.c
    os_task.c
)

add_library(os_assert
    os_assert.c
)

add_library(os_kernel
    os_kernel.c
)

add_library(os_task
    os_task.c
)

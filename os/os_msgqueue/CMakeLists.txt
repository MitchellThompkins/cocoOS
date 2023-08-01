add_library(os_msgqueue
    src/os_msgqueue.c
)

target_include_directories(os_msgqueue
    PRIVATE
        include
)

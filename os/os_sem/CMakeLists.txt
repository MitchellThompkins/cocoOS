add_library(os_sem
    src/os_sem.c
)

target_include_directories(os_sem
    PRIVATE
        include
)

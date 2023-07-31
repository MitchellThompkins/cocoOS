add_library(os_kernel
    src/os_kernel.c
)

target_include_directories(os_kernel
    PUBLIC
        include
)

add_library(os_utils
    src/os_assert.c
    src/os_cbk.c
)

target_include_directories(os_utils
    PUBLIC
        include
)

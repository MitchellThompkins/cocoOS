add_library(os_utils
    os_assert.c
    os_cbk.c
)

target_include_directories(os_utils
    PUBLIC
        include
)

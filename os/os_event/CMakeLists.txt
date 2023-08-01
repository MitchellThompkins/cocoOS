add_library(os_event
    src/os_event.c
)

target_include_directories(os_event
    PRIVATE
        include
)

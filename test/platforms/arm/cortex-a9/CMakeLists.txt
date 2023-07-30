if("${PLATFORM}" STREQUAL "CORTEX_A9")
    add_library(platform
        src/angel.c
        src/angel.S
        src/cpu_a9.c
        src/crt1.c
        src/gic.c
        src/irq.c
        src/platform.c
        src/startup.S
        src/timer.c
    )

    target_include_directories(platform
        PUBLIC
            include)

    target_link_options(platform
        INTERFACE
            "-T${CMAKE_CURRENT_SOURCE_DIR}/cortex_a9.ld"
    )
endif()

if("${PLATFORM}" STREQUAL "CORTEX_M4")
    add_library(platform
        src/angel.c
        src/angel.S
        src/cpu_m4.c
        src/crt1.c
        src/platform.c
        src/startup.S
        src/timer.c
    )

    target_include_directories(platform
        PUBLIC
            include)

    target_link_libraries(platform
        PUBLIC
            cpputest_main)

    target_link_options(platform
        INTERFACE
            "-T${CMAKE_CURRENT_SOURCE_DIR}/cortex_m4.ld"
    )
endif()

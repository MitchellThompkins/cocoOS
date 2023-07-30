if("${PLATFORM}" STREQUAL "CORTEX_M4")
    add_library(platform
        src/startup.S
        src/platform.c
    )

    target_link_options(platform
        INTERFACE
            "-T${CMAKE_CURRENT_SOURCE_DIR}/cortex_m4.ld"
    )
endif()

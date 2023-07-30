if("${PLATFORM}" STREQUAL "X86_64")
    add_library(platform
        src/platform.c
    )
endif()

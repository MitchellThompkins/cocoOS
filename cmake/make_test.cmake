function(make_test target)

    add_custom_command(
        TARGET ${target} POST_BUILD
        COMMAND
            "${CMAKE_OBJCOPY}"
            --output-target binary
            --only-section=.entry
            --only-section=.text
            --only-section=.rodata
            --only-section=.ARM.exidx
            --only-section=.ARM.bss
            --only-section=.ARM.data
            "$<TARGET_FILE:${target}>"
            "$<TARGET_FILE_DIR:${target}>/${target}.bin"
        VERBATIM
    )

    target_link_options(${target} PRIVATE "-Wl,-Map=$<TARGET_FILE_DIR:${target}>/${target}.map")

endfunction()

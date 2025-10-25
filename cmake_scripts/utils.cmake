function(build_target TARGET_NAME)
    add_executable(${TARGET_NAME}.elf
        $<TARGET_OBJECTS:HAL>
        $<TARGET_OBJECTS:App>
        ${SOURCES}
        ${TARGET_NAME}.cpp
        ${LINKER_SCRIPT}
    )

    target_include_directories(${TARGET_NAME}.elf PUBLIC
        ../HAL
        ../HAL/CMSIS/Include
        ../app
    )

    # set_target_properties(${TARGET_NAME}.elf PROPERTIES OUTPUT_NAME ${TARGET_NAME})

    set(CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")# -u _printf_float")

    set(HEX_FILE ${PROJECT_BINARY_DIR}/${TARGET_NAME}.hex)
    set(BIN_FILE ${PROJECT_BINARY_DIR}/${TARGET_NAME}.bin)

    add_custom_command(TARGET ${TARGET_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${TARGET_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${TARGET_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE} \nBuilding ${BIN_FILE}")
endfunction()

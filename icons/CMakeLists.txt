if(NOT INKSCAPE_EXECUTABLE)
    find_program(INKSCAPE_EXECUTABLE inkscape COMMENT "Inkscape executable")
endif()

if(INKSCAPE_EXECUTABLE)

    get_filename_component(MSUPROJ_SVG msuproj.svg ABSOLUTE)

    set(MSUPROJ_ICON "${CMAKE_CURRENT_BINARY_DIR}/msuproj.png")
    set(MSUPROJ_ICON ${MSUPROJ_ICON} PARENT_SCOPE)
    add_custom_command(OUTPUT ${MSUPROJ_ICON}
        COMMAND ${INKSCAPE_EXECUTABLE} -z -e ${MSUPROJ_ICON} ${MSUPROJ_SVG}
        COMMENT "Generating icons/msuproj.png")

    if(WIN32)
        if(NOT CONVERT_EXECUTABLE)
            find_program(CONVERT_EXECUTABLE convert COMMENT "ImageMagick Convert utility executable")
        endif()
        if(CONVERT_EXECUTABLE)
            set(MSUPROJ_WIN_ICON_SRC "${CMAKE_CURRENT_BINARY_DIR}/msuproj256.png")
            set(MSUPROJ_WIN_ICON "${CMAKE_CURRENT_BINARY_DIR}/msuproj.ico")
            set(MSUPROJ_WIN_ICON ${MSUPROJ_WIN_ICON} PARENT_SCOPE)
            add_custom_command(OUTPUT ${MSUPROJ_WIN_ICON}
                COMMAND ${INKSCAPE_EXECUTABLE} -z -w 256 -e ${MSUPROJ_WIN_ICON_SRC} ${MSUPROJ_SVG}
                COMMAND ${CONVERT_EXECUTABLE} ${MSUPROJ_WIN_ICON_SRC} -define icon:auto-resize=\"256,48,32,16\" ${MSUPROJ_WIN_ICON}
                COMMENT "Generating icons/msuproj.ico")
        else()
            message(WARNING "ImageMagick is not found. Windows icon will not be generated.")
        endif()
    endif()

    add_custom_target(icons ALL
        DEPENDS ${MSUPROJ_ICON} ${DOXYGEN_ICON} ${MSUPROJ_WIN_ICON}
        SOURCES ${MSUPROJ_SVG}
        COMMENT "Generate project icons")

else()
    message(WARNING "Inkscape is not found. Icons will not be generated.")
endif()

##################### Configuring Icons ####################

if(BUILD_QT)

    set(PNG_QRC ${CMAKE_BINARY_DIR}/res/icons/icons.qrc)
    configure_file(${RESOURCES_DIR}/icons/icons.qrc.in ${PNG_QRC} COPYONLY)

    if(NOT CONVERT)
        unset(CONVERT CACHE)
        find_program(CONVERT convert)
    endif()
    if(CONVERT)
        message(STATUS "Found Imagemagick - generating PNG icons")
        file(GLOB_RECURSE SVG_FILES ${RESOURCES_DIR}/icons/*.svg)
        foreach(SVG ${SVG_FILES})
            get_filename_component(ICON ${SVG} NAME)
            string(REPLACE "svg" "png" PNG ${ICON})
            set(PNG ${CMAKE_BINARY_DIR}/res/icons/${PNG})
            if(NOT EXISTS ${PNG})
                message(STATUS "    Converting ${ICON}")
                execute_process(COMMAND ${CONVERT} -background none -quantize transparent +dither -colors 15 ${SVG} ${PNG})
            endif()
        endforeach()
        if(WIN32 AND NOT EXISTS ${CMAKE_BINARY_DIR}/res/win32/${CMAKE_PROJECT_NAME}.ico)
            message(STATUS "    Converting ${CMAKE_PROJECT_NAME}.ico")
            file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/res/win32)
            execute_process(COMMAND ${CONVERT} -background none -quantize transparent ${RESOURCES_DIR}/icons/${CMAKE_PROJECT_NAME}.svg
                                    ( -clone 0 -resize 256 )
                                    ( -clone 0 -resize 96 )
                                    ( -clone 0 -resize 48 )
                                    ( -clone 0 -resize 32 )
                                    ( -clone 0 -resize 16 )
                                    -background none -quantize transparent ${CMAKE_BINARY_DIR}/res/win32/${CMAKE_PROJECT_NAME}.ico)
        endif()
    else()
        message(WARNING "Imagemagick convert utility was not found. "
                        "Convert SVG icons to PNG manually and put them to '${CMAKE_BINARY_DIR}/res/icons'")
    endif()

endif()
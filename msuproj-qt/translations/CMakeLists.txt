if(Qt5LinguistTools_FOUND)

    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/translations/")

    cpack_add_component_group(msuproj.qt.translation
        DISPLAY_NAME "Translations"
        PARENT_GROUP msuproj.qt)

    set(LANG_PRIORITY ${MSUPROJ_QT_LOCALES_SIZE})

    foreach(LOCALE IN LISTS MSUPROJ_QT_LOCALES)

        if("${LOCALE}" STREQUAL "en")
            continue()
        endif()

        set(TS_FILE translations/msuproj-qt_${LOCALE}.ts)
        list(APPEND TS_FILES ${TS_FILE})
        add_custom_command(OUTPUT ${TS_FILE}
            COMMAND $ENV{QTDIR}/bin/lupdate -silent src/ -ts ${TS_FILE}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

        set(QM_FILE "${CMAKE_CURRENT_BINARY_DIR}/translations/msuproj-qt_${LOCALE}.qm")
        list(APPEND QM_FILES ${QM_FILE})
        add_custom_command(OUTPUT ${QM_FILE}
            COMMAND $ENV{QTDIR}/bin/lrelease -silent ${TS_FILE} -qm ${QM_FILE}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

        install(FILES "${CMAKE_CURRENT_BINARY_DIR}/translations/msuproj-qt_${LOCALE}.qm"
            DESTINATION ${INSTALL_PATH_I18N}
            COMPONENT msuproj.qt.translation.${LOCALE})
        cpack_add_component(msuproj.qt.translation.${LOCALE}
            DISPLAY_NAME "${${LOCALE}_LANG}"
            DESCRIPTION  "MSUProj-Qt ${${LOCALE}_LANG} translation."
            GROUP msuproj.qt.translation
            DEPENDS msuproj.qt.app)
        cpack_ifw_configure_component(msuproj.qt.translation.${LOCALE}
            PRIORITY ${LANG_PRIORITY})
        math(EXPR LANG_PRIORITY "${LANG_PRIORITY} - 1")

    endforeach()

    add_custom_target(update_ts
        DEPENDS ${TS_FILES}
        COMMENT "Updating MSUProj-Qt translations")

    list(APPEND MSUPROJ_QT_SOURCES
        ${TS_FILES}
        ${QM_FILES})

endif()

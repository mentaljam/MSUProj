#### Find Doxygen
if(NOT DOXYGEN_EXECUTABLE)
    find_program(DOXYGEN_EXECUTABLE doxygen COMMENT "Doxygen executable")
endif()
if(NOT DOXYGEN_EXECUTABLE)
    message(SEND_ERROR "Doxygen was not found, MSUProj-Qt manuals will not be generated.")
endif()

#### Find qcollectiongenerator
find_program(QCOLLECTIONGENERATOR_EXECUTABLE qcollectiongenerator
    PATHS "$ENV{QTDIR}/bin"
    COMMENT "qcollectiongenerator executable")
if(NOT QCOLLECTIONGENERATOR_EXECUTABLE)
    message(SEND_ERROR "qcollectiongenerator was not found, MSUProj-Qt manuals will not be generated.")
endif()

if(DOXYGEN_EXECUTABLE AND QCOLLECTIONGENERATOR_EXECUTABLE)

    #### OMZ Doxygen module
    include(OMZDoxyConf)

    cpack_add_component_group(msuproj.qt.man
        DISPLAY_NAME "Manuals"
        PARENT_GROUP msuproj.qt)

    set(MAN_PRIORITY ${MSUPROJ_QT_LOCALES_SIZE})

    foreach(LOCALE IN LISTS MSUPROJ_QT_LOCALES)
        list(APPEND MSUPROJ_QT_SOURCES help/${LOCALE}.dox)
        list(APPEND QHP_FILES msuproj-qt_${LOCALE}.qch)
        list(APPEND QHP_FILES_TARGETS help/msuproj-qt_${LOCALE}.qch)
        set(OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/help/${LOCALE}")
        configure_doxy_file("${OUTPUT_DIRECTORY}/doxyfile"
            FORMATS QHP
            INPUT "${CMAKE_CURRENT_SOURCE_DIR}/help/${LOCALE}.dox"
            PROJECT_NAME "MSUProj-Qt"
#            ${PROJECT_LOGO}
            OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}"
            QCH_FILE "${CMAKE_CURRENT_BINARY_DIR}/help/msuproj-qt_${LOCALE}.qch"
            QHP_NAMESPACE "amigos.msuproj-qt.${LOCALE}"
            QHP_VIRTUAL_FOLDER ${LOCALE}
            QHP_CUST_FILTER_NAME ${LOCALE}
            QHP_CUST_FILTER_ATTRS ${LOCALE}
            QHP_SECT_FILTER_ATTRS ${LOCALE}
            OUTPUT_LANGUAGE ${${LOCALE}_LANG}
            HTML_EXTRA_STYLESHEET "${CMAKE_CURRENT_SOURCE_DIR}/help/msuproj.css"
            QUIET)
        add_custom_command(OUTPUT help/msuproj-qt_${LOCALE}.qch
            COMMAND ${DOXYGEN_EXECUTABLE} "${OUTPUT_DIRECTORY}/doxyfile"
            COMMENT "Generating Qt help file msuproj-qt/help/msuproj-qt_${LOCALE}.qch")
        install(FILES "${CMAKE_CURRENT_BINARY_DIR}/help/msuproj-qt_${LOCALE}.qch"
            DESTINATION ${INSTALL_PATH_MAN}
            COMPONENT   msuproj.qt.man.${LOCALE})
        cpack_add_component(msuproj.qt.man.${LOCALE}
            DISPLAY_NAME "${${LOCALE}_LANG}"
            DESCRIPTION  "MSUProj-Qt ${${LOCALE}_LANG} manual."
            GROUP msuproj.qt.man
            DEPENDS msuproj.qt.app)
        cpack_ifw_configure_component(msuproj.qt.man.${LOCALE}
            PRIORITY ${MAN_PRIORITY})
        math(EXPR MAN_PRIORITY "${MAN_PRIORITY} - 1")
    endforeach()

    set(QHC_FILE "${CMAKE_CURRENT_BINARY_DIR}/help/msuproj-qt.qhc")
    configure_qhcp_file("${QHC_FILE}p" "${QHP_FILES}")
    add_custom_command(OUTPUT help/msuproj-qt.qhc
        COMMAND ${QCOLLECTIONGENERATOR_EXECUTABLE} help/msuproj-qt.qhcp -o help/msuproj-qt.qhc
        DEPENDS ${QHP_FILES_TARGETS}
        COMMENT "Generating Qt help collection file msuproj-qt/help/msuproj-qt.qhc")
    install(FILES "${QHC_FILE}"
        DESTINATION ${INSTALL_PATH_MAN}
        COMPONENT   msuproj.qt.app)

    list(APPEND MSUPROJ_QT_SOURCES
        "${QHC_FILE}"
        help/msuproj.css)

endif()

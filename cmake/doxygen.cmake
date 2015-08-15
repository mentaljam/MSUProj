################### MSUProj Documentation ##################

include(documentation_locales)

if(DOXYGEN_FOUND)

    #### Update GitHub Documentation ####

    if(GIT_EXECUTABLE)

        doxy_add_target(INPUT ${CMAKE_SOURCE_DIR}/src/msuproj
                              ${CMAKE_SOURCE_DIR}/README.md
                              ${CMAKE_BINARY_DIR}/sources.h
                              ${DOC_DIR}/developer/translations.dox
                        NAME MSUProj-Dev
                        LOGO ${RESOURCES_DIR}/icons/msuproj.svg
                        OUT_DIRECTORY ${CMAKE_BINARY_DIR}/doc/developer
                        LANGUAGE English
                        FORMATS HTML
                        HTML_STYLESHEET ${DOC_DIR}/msuproj.css
        )
        add_custom_target(update_gh-page
                          COMMAND ${CMAKE_COMMAND} -E remove_directory .git
                          COMMAND ${GIT_EXECUTABLE} init
                          COMMAND ${GIT_EXECUTABLE} remote add github git@github.com:mentaljam/MSUProj.git
                          COMMAND ${GIT_EXECUTABLE} checkout --orphan gh-pages
                          COMMAND ${GIT_EXECUTABLE} add -A
                          COMMAND ${GIT_EXECUTABLE} commit -m "Update documentation for version ${V_VERSION} (${V_DATE})"
                          COMMAND ${GIT_EXECUTABLE} push -f github gh-pages
                          DEPENDS build_docs
                          WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/doc/developer/html
                          COMMENT "Upload project documentation to GitHub pages"
                          VERBATIM
        )

        set(PROJECT_FILES ${PROJECT_FILES} ${DOC_DIR}/developer/translations.dox)

    endif()

    #### MSUProj-Qt help ####

    if(INSTALL_DOCS AND BUILD_QT)

        add_custom_target(install_help_files ALL
                          DEPENDS build_docs
                          COMMENT "Prepare manuals for installation"
        )

        foreach(LOCALE ${DOXY_LOCALES})
            list(APPEND QHP_FILES msuproj-qt_${LOCALE}.qch)
            doxy_add_target(QHP
                            INPUT ${DOC_DIR}/msuproj-qt/${LOCALE}.dox
                            NAME MSUProj-Qt
                            LOGO ${CMAKE_BINARY_DIR}/res/icons/msuproj.png
                            OUT_DIRECTORY ${CMAKE_BINARY_DIR}/doc/msuproj-qt/${LOCALE}
                            QCH_FILE ${CMAKE_BINARY_DIR}/doc/msuproj-qt/msuproj-qt_${LOCALE}.qch
                            QHP_NAMESPACE amigos.msuproj-qt.${LOCALE}
                            QHP_VIRTUAL_FOLDER ${LOCALE}
                            QHP_FILTER_NAME ${LOCALE}
                            QHP_FILTER_ATTRS ${LOCALE}
                            LANGUAGE ${${LOCALE}_LANG}
                            HTML_STYLESHEET ${DOC_DIR}/msuproj.css
            )
            install(FILES       ${CMAKE_BINARY_DIR}/doc/msuproj-qt/msuproj-qt_${LOCALE}.qch
                    DESTINATION ${INSTALL_PATH_MAN}
                    COMPONENT   qt_man_${LOCALE}
                    OPTIONAL
            )
            string(TOUPPER ${LOCALE} LOCALE_UPPER)
            set(CPACK_COMPONENT_QT_MAN_${LOCALE_UPPER}_GROUP   man)
            set(CPACK_COMPONENT_QT_MAN_${LOCALE_UPPER}_DEPENDS doc)
            set(CPACK_COMPONENT_QT_MAN_${LOCALE_UPPER}_DISPLAY_NAME "MSUProj-Qt ${LOCALE} help")
            set(PROJECT_FILES ${PROJECT_FILES} ${DOC_DIR}/msuproj-qt/${LOCALE}.dox)
        endforeach()

        doxy_generate_qhc(${CMAKE_BINARY_DIR}/doc/msuproj-qt/msuproj-qt.qhc
                          "${QHP_FILES}"
        )
        install(FILES       ${CMAKE_BINARY_DIR}/doc/msuproj-qt/msuproj-qt.qhc
                DESTINATION ${INSTALL_PATH_MAN}
                COMPONENT   qt
                OPTIONAL
        )

    endif()

    set(PROJECT_FILES ${PROJECT_FILES} ${DOC_DIR}/msuproj.css)

endif()

################### MSUProj Documentation ##################

include(Locales)

if(INSTALL_DOCS AND DOXYGEN_FOUND)

    #### Update GitHub Documentation ####

    if(GIT_EXECUTABLE)

        file(GLOB_RECURSE DOC_DEV_SOURCES
             RELATIVE ${CMAKE_SOURCE_DIR}
             ${CMAKE_SOURCE_DIR}/src/msuproj/*)

        write_sources_versions(${CMAKE_BINARY_DIR}/doc/developer/sources.h
                               ${DOC_DEV_SOURCES})
        doxy_add_target(INPUT ${CMAKE_SOURCE_DIR}/src/msuproj
                              ${CMAKE_BINARY_DIR}/doc/developer/sources.h
                              ${DOC_DIR}/developer/translations.dox
                              ${CMAKE_SOURCE_DIR}/README.md
                        PROJECT_NAME "MSUProj-Dev"
                        PROJECT_LOGO ${RESOURCES_DIR}/icons/msuproj.svg
                        OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/doc/developer
                        OUTPUT_LANGUAGE English
                        FORMATS HTML
                        HTML_EXTRA_STYLESHEET ${DOC_DIR}/msuproj.css
                        RECURSIVE
                        USE_MDFILE_AS_MAINPAGE README.md
        )
        add_custom_target(update_gh-page
                          COMMAND ${CMAKE_COMMAND} -E remove_directory .git
                          COMMAND ${GIT_EXECUTABLE} init
                          COMMAND ${GIT_EXECUTABLE} remote add github git@github.com:mentaljam/MSUProj.git
                          COMMAND ${GIT_EXECUTABLE} checkout --orphan gh-pages
                          COMMAND ${GIT_EXECUTABLE} add -A
                          COMMAND ${GIT_EXECUTABLE} commit -m "Update documentation for version ${MSUPROJ_VERSION_STRING} (${MSUPROJ_VERSION_DATE})"
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
                            PROJECT_NAME MSUProj-Qt
                            PROJECT_LOGO ${CMAKE_BINARY_DIR}/res/icons/msuproj.png
                            OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/doc/msuproj-qt/${LOCALE}
                            QCH_FILE ${CMAKE_BINARY_DIR}/doc/msuproj-qt/msuproj-qt_${LOCALE}.qch
                            QHP_NAMESPACE amigos.msuproj-qt.${LOCALE}
                            QHP_VIRTUAL_FOLDER ${LOCALE}
                            QHP_CUST_FILTER_NAME ${LOCALE}
                            QHP_CUST_FILTER_ATTRS ${LOCALE}
                            QHP_SECT_FILTER_ATTRS ${LOCALE}
                            OUTPUT_LANGUAGE ${${LOCALE}_LANG}
                            HTML_EXTRA_STYLESHEET ${DOC_DIR}/msuproj.css
            )
            install(FILES       ${CMAKE_BINARY_DIR}/doc/msuproj-qt/msuproj-qt_${LOCALE}.qch
                    DESTINATION ${INSTALL_PATH_MAN}
                    COMPONENT   qtman.${LOCALE}
                    OPTIONAL
            )
            string(TOUPPER ${LOCALE} LOCALE_UPPER)
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

#################### Localization files ####################

if(Qt5LinguistTools_FOUND)
    message(STATUS "To update translations run \"make update_translations\"")
    file(GLOB TS_FILES ${PROJECT_SOURCE_DIR}/i18n/msuproj-qt*.ts)
    set_property(SOURCE ${TS_FILES} PROPERTY OUTPUT_LOCATION ${CMAKE_BINARY_DIR}/i18n)
    add_custom_target(update_translations
                      COMMAND lupdate ${PROJECT_SOURCE_DIR}/src/ -ts ${TS_FILES}
                      COMMENT "Updating kmssproj-qt translations files")
    add_custom_target(update_translations_clean
                      COMMAND lupdate ${PROJECT_SOURCE_DIR}/src/ -ts ${TS_FILES} -no-obsolete
                      COMMENT "Updating kmssproj-qt translations files with '-no-obsolete' key")
    qt5_add_translation(QM_FILES ${TS_FILES})
    install(FILES ${QM_FILES}
            DESTINATION ${INSTALL_PATH_I18N}
            COMPONENT qt)
endif()

################## MSUProj-GUI executable ##################

add_executable(msuproj-qt main.cpp
                          mainwindow.cpp
                          mainwindow.ui
                          ${QM_FILES}  ${TS_FILES})
target_link_libraries(msuproj-qt msuproj Qt5::Widgets)

install(TARGETS msuproj-qt DESTINATION ${INSTALL_PATH_BIN} COMPONENT qt)
set(CPACK_COMPONENT_QT_GROUP msuproj)


#################### Linux desktop entry ###################

if(UNIX)
    configure_file(${RESOURCES_DIR}/linux/msuproj-qt.desktop.in
                   ${CMAKE_BINARY_DIR}/res/linux/msuproj-qt.desktop)
    install(FILES ${CMAKE_BINARY_DIR}/res/linux/msuproj-qt.desktop
            DESTINATION share/applications
            COMPONENT qt)
endif()

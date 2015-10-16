###################### Configuring Qt ######################

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Help REQUIRED)
include_directories(${Qt5Widgets_INCLUDE_DIRS})
set_qt_defenitions()
set(QT_VERSION ${Qt5Widgets_VERSION} PARENT_SCOPE)
message(STATUS "Using Qt v${Qt5Widgets_VERSION}")


#################### Localization files ####################

find_package(Qt5LinguistTools)
if(Qt5LinguistTools_FOUND)
    message(STATUS "To update translations run \"make update_translations\"")
    file(GLOB TS_FILES ${PROJECT_SOURCE_DIR}/i18n/msuproj-qt_*.ts)
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
else()
    message(AUTHOR_WARNING "Qt5LinguistTools were not found, translations will not be generated")
endif()


##################### Configuring Icons ####################

if(NOT CONVERT_EXECUTABLE)
    unset(CONVERT_EXECUTABLE CACHE)
    find_program(CONVERT_EXECUTABLE convert)
endif()

if(CONVERT_EXECUTABLE)
    message(STATUS "Found Imagemagick - ${CONVERT_EXECUTABLE}")
    set(PNG_QRC ${CMAKE_BINARY_DIR}/res/icons/icons.qrc)
    configure_file(${RESOURCES_DIR}/icons/icons.qrc.in ${PNG_QRC})
    file(GLOB_RECURSE SVG_FILES ${RESOURCES_DIR}/icons/*.svg)
    set_property(SOURCE ${SVG_FILES} PROPERTY OUTPUT_LOCATION ${CMAKE_BINARY_DIR}/res/icons)
    convert_png(SVG_FILES PNG_FILES)
    list(APPEND AUTOGEN_TARGET_DEPENDS ${PNG_FILES})
    set(PROJECT_FILES ${PROJECT_FILES} ${SVG_FILES} PARENT_SCOPE)
else()
    message(WARNING "Imagemagick convert utility was not found. Icons would not be generated")
endif()


################## MSUProj-GUI executable ##################

file(GLOB MSUPROJQT_SOURCES  *.cpp
                             *.ui
)
list(APPEND MSUPROJQT_SOURCES ${QM_FILES}
                              ${TS_FILES}
                              ${PNG_QRC}
)

if(WIN32)
    set(VER_FILEDESCRIPTION_STR  "MSUProj Qt Interface")
    set(VER_INTERNALNAME_STR     "msuproj-qt")
    set(VER_ORIGINALFILENAME_STR "msuproj-qt.exe")
    set(VER_PRODUCTNAME_STR      "MSUProj-Qt")
    if(CONVERT_EXECUTABLE)
        set(IDI_ICON             "IDI_ICON1 ICON DISCARDABLE \"msuproj.ico\"")
        set(SVG_FILES ${RESOURCES_DIR}/icons/msuproj.svg)
        set_property(SOURCE ${SVG_FILES} PROPERTY OUTPUT_LOCATION ${CMAKE_BINARY_DIR}/res/win32)
        convert_ico(SVG_FILES ICO_OUTPUT)
        list(APPEND AUTOGEN_TARGET_DEPENDS ${ICO_OUTPUT})
    endif()
    set(WIN_RC ${CMAKE_BINARY_DIR}/res/win32/msuproj-qt.rc)
    configure_file(${RESOURCES_DIR}/win32/msuproj.rc.in ${WIN_RC})
    list(APPEND MSUPROJQT_SOURCES ${WIN_RC})
endif()

if(MSVC)
    add_executable(msuproj-qt WIN32 ${MSUPROJQT_SOURCES})
    set_target_properties(msuproj-qt PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
else()
    add_executable(msuproj-qt WIN32 ${MSUPROJQT_SOURCES})
endif()
if(AUTOGEN_TARGET_DEPENDS)
    set_target_properties(msuproj-qt PROPERTIES AUTOGEN_TARGET_DEPENDS "${AUTOGEN_TARGET_DEPENDS}")
endif()
set_target_properties(msuproj-qt PROPERTIES AUTOMOC YES
                                            AUTOUIC YES
                                            AUTORCC YES
)
target_link_libraries(msuproj-qt msuproj Qt5::Widgets Qt5::Help)

install(TARGETS msuproj-qt
        DESTINATION ${INSTALL_PATH_BIN}
        COMPONENT qt)


#################### Linux desktop entry ###################

if(UNIX)
    configure_file(${RESOURCES_DIR}/linux/msuproj-qt.desktop.in
                   ${CMAKE_BINARY_DIR}/res/linux/msuproj-qt.desktop)
    install(FILES ${CMAKE_BINARY_DIR}/res/linux/msuproj-qt.desktop
            DESTINATION share/applications
            COMPONENT qt)
    install(FILES ${RESOURCES_DIR}/icons/${CMAKE_PROJECT_NAME}.svg
            DESTINATION share/icons/hicolor/scalable/apps
            COMPONENT qt)
endif()

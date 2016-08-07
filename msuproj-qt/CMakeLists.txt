###################### Configuring Qt ######################

#### Find Qt
include(cmake/FindQt.cmake)

#### Locales
include(cmake/Locales.cmake)

#### MSUProj-Qt sources
set(MSUPROJ_QT_SOURCES
    src/main.cpp
    src/warper.cpp
    src/mainwindow.cpp
    src/mainwindow.ui
    src/settings.cpp
    src/settingswindow.cpp
    src/settingswindow.ui
    src/helpbrowser.cpp
    src/helpwindow.cpp)

#### Translations
include(translations/CMakeLists.txt)

#### Manuals
include(help/CMakeLists.txt)

#### Icon
if(MSUPROJ_ICON)
    configure_file(icons.qrc.in icons.qrc)
    list(APPEND MSUPROJ_QT_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/icons.qrc")
endif()

####  Windows RC file
if(WIN32)
    # Windows ico
    if(MSUPROJ_WIN_ICON)
        set(RC_ICON_OPTION ICON "${MSUPROJ_WIN_ICON}")
    endif()
    configure_rcfile(msuproj-qt.rc
        ${RC_ICON_OPTION}
        VERSION          ${PROJECT_VERSION}
        COMPANYNAME      ${CPACK_PACKAGE_VENDOR}
        FILEDESCRIPTION  "MSUProj Qt Interface"
        INTERNALNAME     "msuproj-qt"
        LEGALCOPYRIGHT   ${COPYRIGHT}
        ORIGINALFILENAME "msuproj-qt.exe"
        PRODUCTNAME      "MSUProj-Qt")
    list(APPEND MSUPROJ_QT_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/msuproj-qt.rc")
endif()

if(MSVC)
    add_executable(msuproj-qt WIN32 ${MSUPROJ_QT_SOURCES})
    set_target_properties(msuproj-qt PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
elseif(WIN32)
    add_executable(msuproj-qt WIN32 ${MSUPROJ_QT_SOURCES})
else()
    add_executable(msuproj-qt ${MSUPROJ_QT_SOURCES})
endif()

set_target_properties(msuproj-qt PROPERTIES
    AUTOMOC YES
    AUTOUIC YES
    AUTORCC YES)

target_include_directories(msuproj-qt PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(msuproj-qt msuproj ${QT_LINK_LIBRARIES})

install(TARGETS msuproj-qt
    DESTINATION ${INSTALL_PATH_BIN}
    COMPONENT msuproj.qt.app)
cpack_add_component_group(msuproj.qt
    DISPLAY_NAME "MSUProj-Qt"
    DESCRIPTION  "The graphical user interface for MSUProj based on Qt."
    PARENT_GROUP msuproj)
cpack_ifw_configure_component_group(msuproj.qt
    SCRIPT "${CMAKE_SOURCE_DIR}/package/qtifw/component-msuproj_qt.qs"
    PRIORITY 1)
cpack_add_component(msuproj.qt.app
    DISPLAY_NAME "MSUProj-Qt Binary"
    GROUP msuproj.qt
    ${COMPONENT_DEPENDS})
cpack_ifw_configure_component(msuproj.qt.app
    PRIORITY 1)

#### Linux desktop entry
if(UNIX)
    configure_file(msuproj-qt.desktop.in msuproj-qt.desktop)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/msuproj-qt.desktop
        DESTINATION share/applications
        COMPONENT msuproj.qt.app)
    install(FILES ../icons/msuproj.svg
        DESTINATION share/icons/hicolor/scalable/apps
        COMPONENT msuproj.qt.app)
endif()

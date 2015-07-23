######################## MSUProj-GUI #######################


add_executable(msuproj-qt main.cpp
                          mainwindow.cpp
                          mainwindow.ui)
target_link_libraries(msuproj-qt msuproj Qt5::Widgets)

install(TARGETS msuproj-qt DESTINATION ${INSTALL_PATH_BIN} COMPONENT qt)
set(CPACK_COMPONENT_QT_GROUP msuproj)

if(UNIX)
    configure_file(${RESOURCES_DIR}/linux/msuproj-qt.desktop.in
                   ${CMAKE_BINARY_DIR}/res/linux/msuproj-qt.desktop)
    install(FILES ${CMAKE_BINARY_DIR}/res/linux/msuproj-qt.desktop
            DESTINATION share/applications
            COMPONENT qt)
endif()

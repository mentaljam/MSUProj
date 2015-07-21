######################## MSUProj-GUI #######################


add_executable(msuproj-qt main.cpp
                          mainwindow.cpp
                          mainwindow.ui)
target_link_libraries(msuproj-qt msuproj Qt5::Widgets)

install(TARGETS msuproj-qt DESTINATION ${INSTALL_PATH_BIN} COMPONENT qt)
set(COMPONENTS_ALL "${COMPONENTS_ALL} qt")

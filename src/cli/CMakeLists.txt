######################## MSUProj-CLI #######################


add_executable(msuproj-cli msuproj-cli.cpp)
target_link_libraries(msuproj-cli msuproj)

install(TARGETS msuproj-cli DESTINATION ${INSTALL_PATH_BIN} COMPONENT cli)

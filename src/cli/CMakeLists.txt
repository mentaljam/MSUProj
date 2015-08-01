######################## MSUProj-CLI #######################

set(MSUPROJCLI_SOURCES msuproj-cli.cpp)

if(WIN32)
    set(WIN_RC ${CMAKE_BINARY_DIR}/res/win32/msuproj-cli.rc)
    unset(IDI_ICON)
    set(VER_FILEDESCRIPTION_STR "MSUProj CLI Interface")
    set(VER_INTERNALNAME_STR    "msuproj-cli")
    set(VER_ORIGINALFILENAME_STR "msuproj-cli.exe")
    set(VER_PRODUCTNAME_STR     "MSUProj-CLI")
    configure_file(${RESOURCES_DIR}/win32/msuproj.rc.in ${WIN_RC})
    list(APPEND MSUPROJCLI_SOURCES ${WIN_RC})
endif()

add_executable(msuproj-cli ${MSUPROJCLI_SOURCES})
target_link_libraries(msuproj-cli msuproj)

install(TARGETS msuproj-cli
        DESTINATION ${INSTALL_PATH_BIN}
        COMPONENT cli)
set(CPACK_COMPONENTS_ALL ${CPACK_COMPONENTS_ALL} cli PARENT_SCOPE)

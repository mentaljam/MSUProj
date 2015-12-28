######################## MSUProj-CLI #######################

set(MSUPROJCLI_SOURCES msuproj-cli.cpp)

if(WIN32)
    set(WIN_RC ${CMAKE_BINARY_DIR}/res/win32/msuproj-cli.rc)
    unset(IDI_ICON)
    set(VER_FILEDESCRIPTION_STR  "MSUProj Console Utility")
    set(VER_INTERNALNAME_STR     "msuproj-cli")
    set(VER_ORIGINALFILENAME_STR "msuproj-cli.exe")
    set(VER_PRODUCTNAME_STR      "MSUProj Console Utility")
    if(DEFINED PROJECT_VERSION_TWEAK)
        set(RC_VERSION_TWEAK ${PROJECT_VERSION_TWEAK})
    else()
        set(RC_VERSION_TWEAK 0)
    endif()
    configure_file(${RESOURCES_DIR}/win32/msuproj.rc.in ${WIN_RC})
    list(APPEND MSUPROJCLI_SOURCES ${WIN_RC})
endif()

add_executable(msuproj-cli ${MSUPROJCLI_SOURCES})
target_link_libraries(msuproj-cli msuproj)

install(TARGETS msuproj-cli
        DESTINATION ${INSTALL_PATH_BIN}
        COMPONENT cli)

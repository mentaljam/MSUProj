######################## MSUProj-CLI #######################

set(MSUPROJ_CLI_SOURCES src/msuproj-cli.cpp)

####  Windows RC file
if(WIN32)
    configure_rcfile(msuproj-cli.rc
        VERSION          ${PROJECT_VERSION}
        COMPANYNAME      ${CPACK_PACKAGE_VENDOR}
        FILEDESCRIPTION  "MSUProj Console Application"
        INTERNALNAME     "msuproj-cli"
        LEGALCOPYRIGHT   ${COPYRIGHT}
        ORIGINALFILENAME "msuproj-cli.exe"
        PRODUCTNAME      "MSUProj CLI")
    list(APPEND MSUPROJ_CLI_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/msuproj-cli.rc")
endif()

add_executable(msuproj-cli ${MSUPROJ_CLI_SOURCES})
target_link_libraries(msuproj-cli msuproj)

set(CHECK_PREREQUISITES ${CHECK_PREREQUISITES} $<TARGET_FILE:msuproj-cli> PARENT_SCOPE)

install(TARGETS msuproj-cli
    DESTINATION ${INSTALL_PATH_BIN}
    COMPONENT msuproj.cli)
cpack_add_component(msuproj.cli
    DISPLAY_NAME "MSUProj CLI"
    DESCRIPTION  "MSUProj console application."
    GROUP msuproj
    ${COMPONENT_DEPENDS})
cpack_ifw_configure_component(msuproj.cli
    PRIORITY 2
    ${COMPONENT_DEPENDS})

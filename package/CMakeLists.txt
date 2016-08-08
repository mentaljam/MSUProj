#### Common info
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MSUProj Project")
set(CPACK_PACKAGE_DESCRIPTION_FILE    "${CMAKE_SOURCE_DIR}/package/description.txt")
set(CPACK_PACKAGE_VENDOR  "NTs OMZ")
set(CPACK_PACKAGE_CONTACT "Petr Tsymbarovich <petr@tsymbarovich.ru>")
set(COPYRIGHT "Research Center for Earth Operative Monitoring (NTs OMZ) <www.ntsomz.ru>")
set(WEB       "https://github.com/mentaljam/MSUProj")
set(REPO_URL  "ftp://AMIGOS:robonuka@185.26.115.106:2121/Download/PETR/MSUProj")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_BINARY_DIR}/package/license.txt")

#### Configure license file
configure_single_license_file(${CPACK_RESOURCE_FILE_LICENSE}
    FILE "MSUProj License" LICENSE.MSUProj.txt
    FILE "GDAL Logo License" LICENSE.GDALLogoColor.txt)

#### Do not merge groups in installer
set(CPACK_COMPONENTS_GROUPING IGNORE)

#### Ignore these files in source package
set(CPACK_SOURCE_IGNORE_FILES
    "/\\\\.git/"
    "/\\\\.git"
    "/\\\\.gitignore"
    "/\\\\.gitmodules"
    "/\\\\CMakeLists.txt.user")

set(CPACK_PROJECT_CONFIG_FILE "${CMAKE_CURRENT_LIST_DIR}/CPackProjectConfigFile.cmake")

#### Strip files on install and for packages
if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    set(CMAKE_INSTALL_DO_STRIP TRUE)
else()
    set(CMAKE_INSTALL_DO_STRIP FALSE)
endif()
set(CPACK_STRIP_FILES ${CMAKE_INSTALL_DO_STRIP})

#### Generate package name
include(OMZCPack)
generate_package_name()

#### QtIFW configuration
set(CPACK_IFW_PACKAGE_TITLE ${CPACK_PACKAGE_DESCRIPTION_SUMMARY})
set(CPACK_IFW_PRODUCT_URL ${WEB})
set(CPACK_IFW_PACKAGE_MAINTENANCE_TOOL_NAME "msuproj-setup")
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(CPACK_IFW_VERBOSE ON)
endif()
if(MSUPROJ_ICON)
    set(CPACK_IFW_PACKAGE_WINDOW_ICON "${MSUPROJ_ICON}")
endif()
if(MSUPROJ_WIN_ICON)
    set(CPACK_IFW_PACKAGE_ICON "${MSUPROJ_WIN_ICON}")
endif()
configure_file(package/qtifw/controller.qs.in package/controller.qs @ONLY)
set(CPACK_IFW_PACKAGE_CONTROL_SCRIPT "${CMAKE_BINARY_DIR}/package/controller.qs")

#### Include CPack Modules
include(CPack)
include(CPackComponent)
include(CPackIFW)

#### Main component group
cpack_add_component_group(msuproj
    DISPLAY_NAME "MSUProj")
cpack_ifw_configure_component_group(msuproj
    SCRIPT package/qtifw/component-msuproj.qs)

#### Install documentation
set(DOC_FILES
    LICENSE.MSUProj.txt
    LICENSE.GDALLogoColor.txt
    CHANGELOG.txt
    TODO.txt)
install(FILES ${DOC_FILES}
    DESTINATION ${INSTALL_PATH_DOCS}
    COMPONENT msuproj.doc)
cpack_add_component(msuproj.doc
    DISPLAY_NAME "Documentation"
    REQUIRED
    GROUP msuproj)

#### Package repository
if(WIN32)
    cpack_ifw_add_repository(qtifwrepo
        URL "${REPO_URL}/ifw/win/${TARGET_ARCHITECTURE}"
        DISPLAY_NAME "MSUProj QtIFW Repository")
endif()

#### Show package configuration files
add_custom_target(package_files
    SOURCES ${CPACK_PROJECT_CONFIG_FILE}
            package/qtifw/component-msuproj.qs
            package/qtifw/component-msuproj_qt.qs
            package/qtifw/component-runtime_msvc.qs
            ${DOC_FILES}
    COMMENT "Dummy target to show package configuration files in Qt Creator interface")

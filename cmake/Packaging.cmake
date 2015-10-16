#################### Common information ####################

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MSUProj Project")
set(CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_SOURCE_DIR}/doc/description.txt)
set(CPACK_PACKAGE_VENDOR  "NTs OMZ")
set(CPACK_PACKAGE_CONTACT "Petr Tsymbarovich <petr@tsymbarovich.ru>")
set(COPYRIGHT             "Research Center for Earth Operative Monitoring (NTs OMZ) <www.ntsomz.ru>")
set(WEB                   "https://github.com/mentaljam/MSUProj")


################# CPack project config file ################

set(CPACK_PROJECT_CONFIG_FILE
    ${CMAKE_BINARY_DIR}/CPackProjectConfigFile.cmake)
configure_file(cmake/CPackProjectConfigFile.cmake.in
               ${CPACK_PROJECT_CONFIG_FILE}
               @ONLY)


################# Generators configuration #################

#### Common
set(CPACK_PACKAGE_EXECUTABLES  "msuproj-qt;MSUProj-Qt")
set(CPACK_CREATE_DESKTOP_LINKS "msuproj-qt")
set(CPACK_SOURCE_IGNORE_FILES "/\\\\.git/"
                              "/\\\\.tx/"
                              "/\\\\3rd/"
                              "/\\\\cmake/OMZModules/"
                              "/\\\\.gitignore"
                              "/\\\\.gitmodules"
                              "/\\\\CMakeLists.txt.user")

#### DEB
read_debian_description()
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE  ${WEB})
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
if(BUILD_PPA_PACKAGE)
    set(CPACK_DEBIAN_BUILD_DEPENDS omzmodules libgdal-dev
        CACHE STRING "Common debian source build dependencies")
    set(CPACK_DEBIAN_QT_BUILD_DEPENDS qtbase5-dev
                                      qttools5-dev
                                      qttools5-dev-tools
                                      libqt5sql5-sqlite
                                      qt5-default
                                      imagemagick
                                      librsvg2-bin
                                      libxml2
                                      git
                                      doxygen
        CACHE STRING "Qt debian source build dependencies")
    set(CPACK_DEBIAN_DISTRIBUTION_NAMES vivid wily
        CACHE STRING "PPA distributions names")
    set(CPACK_DEBIAN_PPA "ppa:mentaljam/amigos"
        CACHE STRING "PPA name")
    set(CPACK_DEBIAN_CHANGELOG "  * Read on project page ${WEB}")
    set(CPACK_DEBIAN_CMAKE_ARGUMENTS -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
                                     -DBUILD_CLI=${BUILD_CLI}
                                     -DBUILD_QT=${BUILD_QT}
                                     -DBUILD_TOOLS=${BUILD_TOOLS}
                                     -DINSTALL_DEV=${INSTALL_DEV}
                                     -DV_DATE=${V_DATE})
    include(OMZDebuildConf)
endif()
set(CPACK_DEBIAN_PACKAGE_SECTION  "Science")

#### RPM
set(CPACK_RPM_PACKAGE_LICENSE "Zlib")
set(CPACK_RPM_PACKAGE_URL     ${WEB})

#### NSIS
set(CPACK_NSIS_MUI_ICON ${CMAKE_BINARY_DIR}/res/win32/${CMAKE_PROJECT_NAME}.ico)
set(CPACK_NSIS_INSTALLED_ICON_NAME "msuproj-qt.exe")
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_MODIFY_PATH ON)
set(CPACK_NSIS_EXECUTABLES_DIRECTORY .)
set(CPACK_NSIS_MUI_FINISHPAGE_RUN msuproj-qt)
set(CPACK_NSIS_CONTACT ${CPACK_PACKAGE_CONTACT})
set(CPACK_NSIS_URL_INFO_ABOUT ${WEB})
set(CPACK_NSIS_MENU_LINKS
    "https://github.com/mentaljam/MSUProj"        "Project Page"
    "https://github.com/mentaljam/MSUProj/issues" "Issues and Suggestions"
    "http://meteor.robonuka.ru/"                  "Robonuka Meteor-M N2 Site")

#### QtIFW
set(CPACK_IFW_PRODUCT_URL ${WEB})
set(CPACK_IFW_PACKAGE_WINDOW_ICON ${CMAKE_BINARY_DIR}/res/icons/${CMAKE_PROJECT_NAME}.png)
if(WIN32)
    set(CPACK_IFW_PACKAGE_ICON ${CMAKE_BINARY_DIR}/res/win32/${CMAKE_PROJECT_NAME}.ico)
endif()
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(CPACK_IFW_VERBOSE ON)
endif()
set(CPACK_IFW_PACKAGE_CONTROL_SCRIPT ${RESOURCES_DIR}/qtifw/controller.qs)
file(GLOB IFW_SCRIPTS ${RESOURCES_DIR}/qtifw/*.qs)
list(APPEND PROJECT_FILES ${IFW_SCRIPTS})


###################### Including CPack #####################

include(CPack)
include(CPackIFW)


##################### QtIFW repository #####################

if(WIN32)
    set(REPO "win")
else()
    set(REPO ${CMAKE_SYSTEM_NAME})
endif()
cpack_ifw_add_repository(winrepo
     URL "ftp://AMIGOS:robonuka@185.26.115.106:2121/Download/PETR/MSUProj/ifw/${REPO}/${COMPILED_ARCH}/"
     DISPLAY_NAME "MSUProj QtIFW Repository")


#################### Additional targets ####################

#### Runtime

if(WIN32 AND INSTALL_RUNTIME)

    ## GDAL
    get_filename_component(DLL_PATH ${GDAL_LIBRARIES} DIRECTORY)
    string(REPLACE "lib" "bin" DLL_PATH ${DLL_PATH})
    file(GLOB GDAL_DLLS ${DLL_PATH}/*gdal*.dll
                        ${DLL_PATH}/*geos*.dll
                        ${DLL_PATH}/*proj*.dll)
    install(FILES       ${GDAL_DLLS}
            DESTINATION ${INSTALL_PATH_BIN}
            COMPONENT   runtime.gdal)

    ## C++ libs
    if(MINGW)
        get_filename_component(CXX_PATH ${CMAKE_CXX_COMPILER} PATH)
        if(ARCH EQUAL 64)
            set(GCC_RUNTIME_MASK ${CXX_PATH}/libstdc++_64-*.dll
                                 ${CXX_PATH}/libgcc*64*.dll)
        else()
            set(GCC_RUNTIME_MASK ${CXX_PATH}/libstdc++-*.dll
                                 ${CXX_PATH}/libgcc*sjlj*.dll)
        endif()
        file(GLOB GCC_RUNTIME ${GCC_RUNTIME_MASK})
        list(APPEND CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS ${GCC_RUNTIME})
    endif()
    if(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS OR MSVC)
        set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ${INSTALL_PATH_BIN})
        set(CMAKE_INSTALL_SYSTEM_RUNTIME_COMPONENT runtime.cpp)
        include(InstallRequiredSystemLibraries)
        if(MINGW AND CMAKE_BUILD_TYPE STREQUAL "Release")
            install(CODE "message(STATUS \"Stripping runtime libraries\")
                         file(GLOB_RECURSE DLLS \${CMAKE_INSTALL_PREFIX}/*.dll)
                         execute_process(COMMAND ${CXX_PATH}/strip \${DLLS})"
                    COMPONENT runtime.gdal)
        endif()
    else()
        message(AUTHOR_WARNING "Could not find runtime shared libraries for package forming")
    endif()

    set(QTDIR $ENV{QTDIR})
    string(REPLACE "\\" "/" QTDIR ${QTDIR})
    if(BUILD_QT AND QTDIR)
        install(FILES ${QTDIR}/bin/Qt5Core.dll
                      ${QTDIR}/bin/Qt5Gui.dll
                      ${QTDIR}/bin/Qt5Widgets.dll
                      ${QTDIR}/bin/Qt5Help.dll
                      ${QTDIR}/bin/Qt5Network.dll
                      ${QTDIR}/bin/Qt5Sql.dll
                      ${QTDIR}/bin/Qt5CLucene.dll
                DESTINATION ${INSTALL_PATH_BIN}
                COMPONENT runtime.qt)
        install(FILES ${QTDIR}/plugins/platforms/qwindows.dll
                DESTINATION  ${INSTALL_PATH_BIN}/platforms
                COMPONENT runtime.qt)
        install(FILES ${QTDIR}/plugins/imageformats/qjpeg.dll
                      ${QTDIR}/plugins/imageformats/qwbmp.dll
                DESTINATION ${INSTALL_PATH_BIN}/imageformats
                COMPONENT runtime.qt)
        install(FILES ${QTDIR}/plugins/sqldrivers/qsqlite.dll
                DESTINATION  ${INSTALL_PATH_BIN}/sqldrivers
                COMPONENT runtime.qt)
#        file(GLOB QTQMS ${QTDIR}/translations/qtbase*.qm)
#        install(FILES ${QTQMS}
#                DESTINATION ${INSTALL_PATH_I18N}
#                COMPONENT runtime.qt)
    endif()

endif()

#### Documentation
file(GLOB LICENSE_FILES LICENSE*)
install(FILES ${LICENSE_FILES} TODO.txt CHANGELOG.txt
        DESTINATION ${INSTALL_PATH_DOCS}
        COMPONENT doc)
set(PROJECT_FILES ${PROJECT_FILES} ${LICENSE_FILES}
                                   TODO.txt
                                   CHANGELOG.txt
                                   README.md)


################# Installation components ##################

#### Common

if(BUILD_SHARED_LIBS)
    set(DEPENDS DEPENDS lib)
endif()

cpack_add_component(doc HIDDEN REQUIRED)
cpack_ifw_configure_component(doc
                              SCRIPT ${RESOURCES_DIR}/qtifw/component-doc.qs)

cpack_add_component(lib
                    DISPLAY_NAME "MSUProj ${MSUPROJ_VERSION_STRING} Shared"
                    DESCRIPTION  "MSUProj shared library")
cpack_ifw_configure_component(lib
                              PRIORITY 10)

cpack_add_component(cli
                    DISPLAY_NAME "MSUProj ${MSUPROJ_VERSION_STRING} CLI"
                    DESCRIPTION  "MSUProj command line interface application"
                    ${DEPENDS})
cpack_ifw_configure_component(cli
                              PRIORITY 9)

cpack_add_component(qt
                    DISPLAY_NAME "MSUProj-Qt ${MSUPROJ_VERSION_STRING}"
                    DESCRIPTION  "MSUProj graphical user interface application"
                    ${DEPENDS})
cpack_ifw_configure_component(qt
                              SCRIPT ${RESOURCES_DIR}/qtifw/component-qt.qs
                              PRIORITY 8)

#### Development
cpack_add_component_group(dev DISPLAY_NAME "Development")

    cpack_add_component(libdev DISABLED
                        DISPLAY_NAME "MSUProj Static"
                        DESCRIPTION  "MSUProj static library"
                        GROUP        dev
                        ${DEPENDS})

    cpack_add_component(headers DISABLED
                        DISPLAY_NAME "Headers"
                        DESCRIPTION  "MSUProj library headers"
                        GROUP        dev)

#### Tools
cpack_add_component_group(tools DISPLAY_NAME "Tools")
cpack_ifw_configure_component_group(tools
                                    PRIORITY 6)

    cpack_add_component(gcpthiner DISABLED
                        DISPLAY_NAME "GCPThiner"
                        DESCRIPTION  "A tool to reduce GCPs number and their step size in .gcp files"
                        GROUP        tools)

#### Manuals
cpack_add_component_group(qt_man DISPLAY_NAME "Manuals")
cpack_ifw_configure_component_group(qt_man
                                    PRIORITY 7)
    set(LOCALE_PRIORITY ${DOXY_LOCALES_SIZE})
    math(EXPR LOCALE_PRIORITY "${LOCALE_PRIORITY} + 1")
    foreach(LOCALE ${DOXY_LOCALES})
        cpack_add_component(qt_man.${LOCALE}
                            DISPLAY_NAME "${${LOCALE}_LANG} MSUProj-Qt help"
                            DEPENDS      qt
                            GROUP        qt_man)
        cpack_ifw_configure_component(qt_man.${LOCALE}
                                      PRIORITY ${LOCALE_PRIORITY})
        math(EXPR LOCALE_PRIORITY "${LOCALE_PRIORITY} - 1")
    endforeach()

#### Runtime
cpack_add_component_group(runtime
                          DISPLAY_NAME "Runtime"
                          DESCRIPTION  "Thirdparty runtime libraries (GDAL, GEOS, Proj, Qt, C++ standard library)")
cpack_ifw_configure_component_group(runtime
                                    PRIORITY 5)

    cpack_add_component(runtime.gdal
                        DISPLAY_NAME "GDAL ${GDAL_VERSION}"
                        DESCRIPTION  "GDAL, GEOS and Proj libraries"
                        GROUP runtime)
    cpack_ifw_configure_component(runtime.gdal
                                  VERSION ${GDAL_VERSION}
                                  PRIORITY 3)

    cpack_add_component(runtime.cpp
                        DISPLAY_NAME "${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}"
                        DESCRIPTION  "C++ standard library"
                        GROUP runtime)
    cpack_ifw_configure_component(runtime.cpp
                                  VERSION ${CMAKE_CXX_COMPILER_VERSION}
                                  PRIORITY 2)

    if(BUILD_QT)
        cpack_add_component(runtime.qt
                            DISPLAY_NAME "Qt ${QT_VERSION}"
                            DESCRIPTION  "Qt libraries"
                            GROUP runtime)
        cpack_ifw_configure_component(runtime.qt
                                      VERSION ${QT_VERSION}
                                      PRIORITY 1)
    endif()

#################### Common information ####################

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MSUProj Project")
set(CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_SOURCE_DIR}/doc/description.txt)
set(CPACK_PACKAGE_VENDOR  "NTs OMZ")
set(CPACK_PACKAGE_CONTACT "Petr Tsymbarovich <petr@tsymbarovich.ru>")
set(COPYRIGHT             "Research Center for Earth Operative Monitoring (NTs OMZ) <www.ntsomz.ru>")
set(WEB                   "https://github.com/mentaljam/MSUProj")


################# Installation components ##################

set(CPACK_COMPONENT_GROUP_MSUPROJ_DISPLAY_NAME "MSUProj")
set(CPACK_COMPONENT_GROUP_MSUPROJ_EXPANDED TRUE)

    set(CPACK_COMPONENT_LIB_GROUP        msuproj)
    set(CPACK_COMPONENT_LIB_DEPENDS      doc)
    set(CPACK_COMPONENT_LIB_DISPLAY_NAME "Shared Library")
    set(CPACK_COMPONENT_LIB_DESCRIPTION  "MSUProj shared library")

    set(CPACK_COMPONENT_CLI_GROUP        msuproj)
    set(CPACK_COMPONENT_CLI_DEPENDS      doc)
    set(CPACK_COMPONENT_CLI_DISPLAY_NAME "MSUProj-CLI")
    set(CPACK_COMPONENT_CLI_DESCRIPTION  "MSUProj command line interface application")

    set(CPACK_COMPONENT_QT_GROUP         msuproj)
    set(CPACK_COMPONENT_QT_DEPENDS       doc)
    set(CPACK_COMPONENT_QT_DISPLAY_NAME  "MSUProj-Qt")
    set(CPACK_COMPONENT_QT_DESCRIPTION   "MSUProj graphical user interface application")

    set(CPACK_COMPONENT_RUNTIME_GROUP        msuproj)
    set(CPACK_COMPONENT_RUNTIME_DEPENDS      doc)
    set(CPACK_COMPONENT_RUNTIME_DISPLAY_NAME "Runtime")
    set(CPACK_COMPONENT_RUNTIME_DESCRIPTION  "Thirdparty runtime libraries (GDAL, GEOS, Proj, Qt, C++ standard library)")

set(CPACK_COMPONENT_GROUP_DEV_DISPLAY_NAME "Development")

    set(CPACK_COMPONENT_LIBDEV_GROUP         dev)
    set(CPACK_COMPONENT_LIBDEV_DISABLED      ON)
    set(CPACK_COMPONENT_LIBDEV_DEPENDS       doc)
    set(CPACK_COMPONENT_LIBDEV_DISPLAY_NAME  "Static Library")
    set(CPACK_COMPONENT_LIBDEV_DESCRIPTION   "MSUProj static library")

    set(CPACK_COMPONENT_HEADERS_GROUP        dev)
    set(CPACK_COMPONENT_HEADERS_DISABLED     ON)
    set(CPACK_COMPONENT_HEADERS_DEPENDS      doc)
    set(CPACK_COMPONENT_HEADERS_DISPLAY_NAME "Headers")
    set(CPACK_COMPONENT_HEADERS_DESCRIPTION  "MSUProj library headers")

set(CPACK_COMPONENT_GROUP_TOOLS_DISPLAY_NAME "Tools")

    set(CPACK_COMPONENT_GCPTHINER_GROUP        tools)
    set(CPACK_COMPONENT_GCPTHINER_DISABLED     ON)
    set(CPACK_COMPONENT_GCPTHINER_DEPENDS      doc)
    set(CPACK_COMPONENT_GCPTHINER_DISPLAY_NAME "GCPThiner")
    set(CPACK_COMPONENT_GCPTHINER_DESCRIPTION  "A tool to reduce GCPs number and their step size in .gcp files")

if(BUILD_SHARED_LIBS)
    list(APPEND CPACK_COMPONENT_CLI_DEPENDS lib)
    list(APPEND CPACK_COMPONENT_QT_DEPENDS lib)
endif()


#################### Additional targets ####################

if(WIN32)

    if(INSTALL_RUNTIME)

        list(APPEND CPACK_COMPONENTS_ALL runtime)
        get_filename_component(DLL_PATH ${GDAL_LIBRARIES} DIRECTORY)
        string(REPLACE "lib" "bin" DLL_PATH ${DLL_PATH})
        file(GLOB GDAL_DLLS ${DLL_PATH}/*gdal*.dll
                            ${DLL_PATH}/*geos*.dll
                            ${DLL_PATH}/*proj*.dll)
        list(APPEND CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS ${GDAL_DLLS})

        if(MINGW)
            get_filename_component(CXX_PATH ${CMAKE_CXX_COMPILER} PATH)
            if(ARCH EQUAL 64)
                set(GCC_RUNTIME_MASK ${CXX_PATH}/libstdc++_64*.dll
                                     ${CXX_PATH}/libgcc*64*.dll)
            else()
                set(GCC_RUNTIME_MASK ${CXX_PATH}/libstdc++*.dll
                                     ${CXX_PATH}/libgcc*sjlj*.dll)
            endif()
            file(GLOB GCC_RUNTIME ${GCC_RUNTIME_MASK})
            list(APPEND CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS ${GCC_RUNTIME})
        endif()

        if(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
            set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ${INSTALL_PATH_BIN})
            set(CMAKE_INSTALL_SYSTEM_RUNTIME_COMPONENT runtime)
            include(InstallRequiredSystemLibraries)
            if(MINGW AND CMAKE_BUILD_TYPE STREQUAL "Release")
                install(CODE "message(STATUS \"Stripping runtime libraries\")
                             file(GLOB_RECURSE DLLS \${CMAKE_INSTALL_PREFIX}/*.dll)
                             execute_process(COMMAND ${CXX_PATH}/strip \${DLLS})"
                        COMPONENT runtime)
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
                    DESTINATION ${INSTALL_PATH_BIN}
                    COMPONENT runtime)
            install(FILES ${QTDIR}/plugins/platforms/qwindows.dll
                    DESTINATION  ${INSTALL_PATH_BIN}/platforms
                    COMPONENT runtime)
            install(FILES ${QTDIR}/plugins/imageformats/qjpeg.dll
                          ${QTDIR}/plugins/imageformats/qwbmp.dll
                    DESTINATION ${INSTALL_PATH_BIN}/imageformats
                    COMPONENT runtime)
            file(GLOB QTQMS ${QTDIR}/translations/qtbase*.qm)
            install(FILES ${QTQMS}
                    DESTINATION ${INSTALL_PATH_I18N}
                    COMPONENT runtime)
        endif()

    endif()

    if(DOXYGEN_FOUND AND HHC)
        message(STATUS "WARNING! Before building package make shure manual have been compiled")
        install(CODE "execute_process(COMMAND ${CMAKE_BUILD_TOOL} manual)" COMPONENT man)
        install(FILES ${CHM_FILE} DESTINATION ${INSTALL_PATH_DOCS} COMPONENT man OPTIONAL)
        list(APPEND CPACK_COMPONENTS_ALL man)
    endif()

endif()

file(GLOB LICENSE_FILES LICENSE*)
install(FILES ${LICENSE_FILES} TODO.txt CHANGELOG.txt
        DESTINATION ${INSTALL_PATH_DOCS}
        COMPONENT doc)
set(CPACK_COMPONENT_DOC_HIDDEN ON)
list(APPEND CPACK_COMPONENTS_ALL doc)


##################### Packaging rules ######################

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
    set(CPACK_DEBIAN_QT_BUILD_DEPENDS imagemagick qttools5-dev-tools qtbase5-dev librsvg2-bin libxml2
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

string(REPLACE ";" "\n" CHECK_PREREQUISITES "${CHECK_PREREQUISITES}")
file(GENERATE
    OUTPUT "${CMAKE_BINARY_DIR}/check_prerequisites.txt"
    CONTENT "${CHECK_PREREQUISITES}")

cpack_add_component_group(runtime
    DISPLAY_NAME "Runtime"
    DESCRIPTION  "Third-party runtime libraries")
cpack_ifw_configure_component_group(runtime
    VERSION 1.0)

#### Scan and install GDAL libraries
set(RUNTIME_TYPE "GDAL")
configure_file(cmake/InstallRuntimeScript.cmake.in InstallRuntimeScriptGdal.cmake @ONLY)
install(SCRIPT "${CMAKE_BINARY_DIR}/InstallRuntimeScriptGdal.cmake"
    COMPONENT runtime.gdal)
cpack_add_component(runtime.gdal
    DISPLAY_NAME "GDAL"
    DESCRIPTION  "GDAL libraries"
    GROUP        runtime)
cpack_ifw_configure_component(runtime.gdal
    VERSION ${GDAL_VERSION})

#### Scan and install GCC libraries
set(RUNTIME_TYPE "GCC")
configure_file(cmake/InstallRuntimeScript.cmake.in InstallRuntimeScriptGcc.cmake @ONLY)
install(SCRIPT "${CMAKE_BINARY_DIR}/InstallRuntimeScriptGcc.cmake"
    COMPONENT runtime.gcc)
cpack_add_component(runtime.gcc
    DISPLAY_NAME "GCC"
    DESCRIPTION  "GCC libraries"
    GROUP        runtime)
cpack_ifw_configure_component(runtime.gcc
    VERSION ${CMAKE_CXX_COMPILER_VERSION})

#### MSVC
if(MSVC)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ${INSTALL_PATH_BIN})
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_COMPONENT   runtime.cpp)
    include(InstallRequiredSystemLibraries)
    cpack_add_component(runtime.msvc
        DISPLAY_NAME "MSVC"
        GROUP        runtime)
    cpack_ifw_configure_component(runtime.msvc
        VERSION ${MSVC_VERSION})
endif()

##### Qt
file(TO_CMAKE_PATH $ENV{QTDIR} QTDIR)
if(BUILD_MSUPROJ_QT AND QTDIR)
    if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
        set(QT_DLL_SUFFIX "d")
    endif()
    install(FILES ${QTDIR}/bin/Qt5Core${QT_DLL_SUFFIX}.dll
                  ${QTDIR}/bin/Qt5Gui${QT_DLL_SUFFIX}.dll
                  ${QTDIR}/bin/Qt5Widgets${QT_DLL_SUFFIX}.dll
                  ${QTDIR}/bin/Qt5Help${QT_DLL_SUFFIX}.dll
                  ${QTDIR}/bin/Qt5Network${QT_DLL_SUFFIX}.dll
                  ${QTDIR}/bin/Qt5Sql${QT_DLL_SUFFIX}.dll
                  ${QTDIR}/bin/Qt5CLucene${QT_DLL_SUFFIX}.dll
                  ${QTDIR}/bin/Qt5WinExtras${QT_DLL_SUFFIX}.dll
        DESTINATION ${INSTALL_PATH_BIN}
        COMPONENT runtime.qt)
    if(MSVC)
        install(FILES ${QTDIR}/bin/libGLESV2.dll
            DESTINATION ${INSTALL_PATH_BIN}
            COMPONENT runtime.qt)
    endif()
    install(FILES ${QTDIR}/plugins/platforms/qwindows.dll
        DESTINATION  ${INSTALL_PATH_BIN}/platforms
        COMPONENT runtime.qt)
    install(FILES ${QTDIR}/plugins/sqldrivers/qsqlite.dll
        DESTINATION ${INSTALL_PATH_BIN}/sqldrivers
        COMPONENT runtime.qt)
    install(FILES ${QTDIR}/translations/qtbase_ru.qm
        DESTINATION ${INSTALL_PATH_I18N}
        COMPONENT runtime.qt)

    cpack_add_component(runtime.qt
        DISPLAY_NAME "Qt Framework"
        GROUP        runtime.qt)
    cpack_ifw_configure_component(runtime.qt
        VERSION ${QT_VERSION})
endif()

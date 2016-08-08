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
if(CMAKE_COMPILER_IS_GNUCC )
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
endif()

#### MSVC
if(MSVC)
    if("${TARGET_ARCHITECTURE}" STREQUAL "amd64")
        set(VCREDIST_NAME "vcredist_x64.exe")
    elseif("${TARGET_ARCHITECTURE}" STREQUAL "i386")
        set(VCREDIST_NAME "vcredist_x86.exe")
    endif()
    if(VCREDIST_NAME)
        file(GLOB_RECURSE VCREDIST "$ENV{VSInstallDir}VC/redist/*/${VCREDIST_NAME}")
        file(TO_CMAKE_PATH "${VCREDIST}" VCREDIST)
        install(FILES "${VCREDIST}"
            DESTINATION ${INSTALL_PATH_BIN}
            COMPONENT runtime.msvc)
        cpack_add_component(runtime.msvc
            DISPLAY_NAME "MSVC"
            GROUP        runtime)
        cpack_ifw_configure_component(runtime.msvc
            SCRIPT "${CMAKE_SOURCE_DIR}/package/qtifw/component-runtime_msvc.qs"
            VERSION ${MSVC_VERSION})
    endif()
endif()

##### Qt
file(TO_CMAKE_PATH $ENV{QTDIR} QTDIR)
if(BUILD_MSUPROJ_QT AND QTDIR)
    if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
        set(QT_DLL_SUFFIX "d")
    endif()
    install(FILES "${QTDIR}/bin/Qt5Core${QT_DLL_SUFFIX}.dll"
                  "${QTDIR}/bin/Qt5Gui${QT_DLL_SUFFIX}.dll"
                  "${QTDIR}/bin/Qt5Widgets${QT_DLL_SUFFIX}.dll"
                  "${QTDIR}/bin/Qt5Help${QT_DLL_SUFFIX}.dll"
                  "${QTDIR}/bin/Qt5Network${QT_DLL_SUFFIX}.dll"
                  "${QTDIR}/bin/Qt5Sql${QT_DLL_SUFFIX}.dll"
                  "${QTDIR}/bin/Qt5CLucene${QT_DLL_SUFFIX}.dll"
                  "${QTDIR}/bin/Qt5WinExtras${QT_DLL_SUFFIX}.dll"
        DESTINATION ${INSTALL_PATH_BIN}
        COMPONENT runtime.qt)
    if(MSVC AND EXISTS "${QTDIR}/bin/libGLESV2.dll")
        install(FILES "${QTDIR}/bin/libGLESV2.dll"
            DESTINATION ${INSTALL_PATH_BIN}
            COMPONENT runtime.qt)
    endif()
    install(FILES "${QTDIR}/plugins/platforms/qwindows${QT_DLL_SUFFIX}.dll"
        DESTINATION  ${INSTALL_PATH_BIN}/platforms
        COMPONENT runtime.qt)
    install(FILES "${QTDIR}/plugins/sqldrivers/qsqlite${QT_DLL_SUFFIX}.dll"
        DESTINATION ${INSTALL_PATH_BIN}/sqldrivers
        COMPONENT runtime.qt)
    install(FILES "${QTDIR}/plugins/imageformats/qjpeg${QT_DLL_SUFFIX}.dll"
                  "${QTDIR}/plugins/imageformats/qwbmp${QT_DLL_SUFFIX}.dll"
            DESTINATION ${INSTALL_PATH_BIN}/imageformats
            COMPONENT runtime.qt)
    file(GLOB QT_TRANSLATIONS "${QTDIR}/translations/qtbase_*.qm")
    install(FILES ${QT_TRANSLATIONS}
        DESTINATION ${INSTALL_PATH_I18N}
        COMPONENT runtime.qt)

    cpack_add_component(runtime.qt
        DISPLAY_NAME "Qt Framework"
        GROUP        runtime)
    cpack_ifw_configure_component(runtime.qt
        VERSION ${QT_VERSION})
endif()

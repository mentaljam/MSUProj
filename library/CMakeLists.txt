#### Version file
configure_file(msuproj_version.h.in msuproj_version.h)

#### Check library type
if(NOT BUILD_MSUPROJ_CLI AND NOT BUILD_MSUPROJ_QT AND NOT BUILD_TOOLS AND NOT BUILD_SHARED_LIBS AND NOT INSTALL_DEV)
    message(WARNING "No targets were provided - building shared library")
    set(BUILD_SHARED_LIBS ON)
    set(INSTALL_DEV ON)
endif()

if(BUILD_SHARED_LIBS)
    set(COMPONENT_DEPENDS DEPENDS msuproj.lib PARENT_SCOPE)
endif()

#### Sources
set(MSUPROJ_SOURCES
    src/logoimage.hpp
    src/msucore.cpp
    src/msupixeltransformer.cpp
    src/msutriangle.cpp
    src/msugcpgrid.cpp
    src/msuproj.cpp)

####  Windows RC file
if(WIN32 AND BUILD_SHARED_LIBS)
    configure_rcfile(msuproj.dll.rc
        VERSION          ${PROJECT_VERSION}
        COMPANYNAME      ${CPACK_PACKAGE_VENDOR}
        FILEDESCRIPTION  "MSUProj shared library"
        INTERNALNAME     "msuproj_dll"
        LEGALCOPYRIGHT   ${COPYRIGHT}
        ORIGINALFILENAME "msuproj-${PROJECT_VERSION_MAJOR}.dll"
        PRODUCTNAME      "MSUProj")
    list(APPEND MSUPROJ_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/msuproj.dll.rc")
endif()

add_library(msuproj ${MSUPROJ_SOURCES})

target_link_libraries(msuproj ${GDAL_LIBRARIES})

include(GenerateExportHeader)
generate_export_header(msuproj EXPORT_FILE_NAME msuproj_export.h)

set_target_properties(msuproj PROPERTIES
    VERSION   ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    OUTPUT_NAME msuproj)

if(BUILD_SHARED_LIBS)

    set(CHECK_PREREQUISITES ${CHECK_PREREQUISITES} $<TARGET_FILE:msuproj> PARENT_SCOPE)

    # Install shared library
    if(WIN32)
        set_target_properties(msuproj PROPERTIES SUFFIX -${PROJECT_VERSION_MAJOR}.dll)
        install(TARGETS msuproj
            RUNTIME DESTINATION ${INSTALL_PATH_BIN}
            COMPONENT msuproj.lib)
    else()
        set(CMAKE_INSTALL_RPATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} PARENT_SCOPE)
        set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE PARENT_SCOPE)
        install(TARGETS msuproj
            EXPORT  MSUProjSharedConfig
            LIBRARY DESTINATION ${INSTALL_PATH_LIB}
            COMPONENT msuproj.lib)
    endif()
    cpack_add_component(msuproj.lib
        DISPLAY_NAME "MSUProj Shared"
        DESCRIPTION  "MSUProj shared library."
        GROUP msuproj)
    cpack_ifw_configure_component(msuproj.lib
        PRIORITY 3)

    if(INSTALL_DEV)
        # Install export files for shared library
        if(WIN32)
            install(TARGETS msuproj
                EXPORT  MSUProjSharedConfig
                ARCHIVE DESTINATION ${INSTALL_PATH_LIB}
                COMPONENT msuproj.dev.lib)
            cpack_add_component(msuproj.dev.lib
                DISPLAY_NAME "MSUProj Export"
                DESCRIPTION  "Export file for the MSUProj shared library."
                GROUP msuproj.dev)
        endif()
        install(EXPORT MSUProjSharedConfig
            DESTINATION ${INSTALL_PATH_CMAKE}
            COMPONENT msuproj.dev.cmake)
        cpack_add_component(msuproj.dev.cmake
            DISPLAY_NAME "MSUProj CMake"
            DESCRIPTION  "CMake export files for the MSUProj shared library."
            GROUP msuproj.dev)
    endif()

elseif(INSTALL_DEV)

    # Install static library
    install(TARGETS msuproj
        EXPORT  MSUProjStaticConfig
        DESTINATION ${INSTALL_PATH_LIB}
        COMPONENT msuproj.dev.lib)
    cpack_add_component(msuproj.dev.lib
        DISPLAY_NAME "MSUProj Static"
        DESCRIPTION  "MSUProj static library."
        GROUP msuproj.dev)
    install(EXPORT MSUProjStaticConfig
        DESTINATION ${INSTALL_PATH_CMAKE}
        COMPONENT msuproj.dev.cmake)
    cpack_add_component(msuproj.dev.cmake
        DISPLAY_NAME "MSUProj CMake"
        DESCRIPTION  "CMake export files for the MSUProj static library."
        GROUP msuproj.dev)

endif()

if(INSTALL_DEV)

    cpack_add_component_group(msuproj.dev
        DISPLAY_NAME "MSUProj Development Files"
        PARENT_GROUP msuproj)

    # Install headers
    file(GLOB H_FILES src/*.h)
    list(APPEND H_FILES
        ${CMAKE_CURRENT_BINARY_DIR}/msuproj_version.h
        ${CMAKE_CURRENT_BINARY_DIR}/msuproj_export.h)

    install(FILES ${H_FILES}
            DESTINATION ${INSTALL_PATH_INCLUDE}
            COMPONENT msuproj.dev.headers)
    cpack_add_component(msuproj.dev.headers
        DISPLAY_NAME "MSUProj Headers"
        DESCRIPTION  "Headers for the MSUProj library."
        GROUP msuproj.dev)

endif()

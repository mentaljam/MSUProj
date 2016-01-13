##################### MSUProj Library ######################

if(NOT BUILD_CLI AND NOT BUILD_QT AND NOT BUILD_TOOLS AND NOT INSTALL_DEV)
    message(AUTHOR_WARNING "No targets were provided - building shared library")
    set(INSTALL_DEV ON)
endif()

file(GLOB MSUPROJ_SOURCES *.cpp *.hpp)

if(WIN32)
    set(WIN_RC ${CMAKE_BINARY_DIR}/res/win32/msuproj.dll.rc)
    unset(IDI_ICON)
    set(VER_FILEDESCRIPTION_STR  "MSUProj Shared Library")
    set(VER_INTERNALNAME_STR     "msuproj_dll")
    set(VER_ORIGINALFILENAME_STR "msuproj-${PROJECT_VERSION_MAJOR}.dll")
    set(VER_PRODUCTNAME_STR      "MSUProj")
    if(DEFINED PROJECT_VERSION_TWEAK)
        set(RC_VERSION_TWEAK ${PROJECT_VERSION_TWEAK})
    else()
        set(RC_VERSION_TWEAK 0)
    endif()
    configure_file(${RESOURCES_DIR}/win32/msuproj.rc.in ${WIN_RC})
    list(APPEND MSUPROJ_SOURCES ${WIN_RC})
endif()

add_library(msuproj ${MSUPROJ_SOURCES})
target_link_libraries(msuproj ${GDAL_LIBRARIES})

if(BUILD_SHARED_LIBS)

    set_target_properties(msuproj
                          PROPERTIES VERSION   ${PROJECT_VERSION}
                                     SOVERSION ${PROJECT_VERSION_MAJOR}
                                     OUTPUT_NAME msuproj)
    if(WIN32)
        set_target_properties(msuproj PROPERTIES SUFFIX -${PROJECT_VERSION_MAJOR}.dll)
        install(TARGETS msuproj
                RUNTIME DESTINATION ${INSTALL_PATH_BIN}
                COMPONENT lib)
    else()
        set(CMAKE_INSTALL_RPATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} PARENT_SCOPE)
        set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE PARENT_SCOPE)
        install(TARGETS msuproj
                EXPORT  MSUProjSharedConfig
                LIBRARY DESTINATION ${INSTALL_PATH_LIB}
                COMPONENT lib)
    endif()

    if(INSTALL_DEV)

        if(WIN32)
            install(TARGETS msuproj
                    EXPORT  MSUProjSharedConfig
                    ARCHIVE DESTINATION ${INSTALL_PATH_LIB}
                    COMPONENT libdev)
        endif()
        install(EXPORT MSUProjSharedConfig
                DESTINATION ${INSTALL_PATH_CMAKE}
                COMPONENT libdev)

        add_library(msuproj-dev STATIC msuproj.cpp logoimage.hpp)
        target_compile_definitions(msuproj-dev PRIVATE "-DMSUPROJ_STATIC_DEFINE=1")
        target_link_libraries(msuproj-dev ${GDAL_LIBRARIES})
        set_target_properties(msuproj-dev
                              PROPERTIES VERSION   ${PROJECT_VERSION}
                                         SOVERSION ${PROJECT_VERSION_MAJOR}
                                         OUTPUT_NAME msuproj-dev)
        set(MSUPROJ_DEV msuproj-dev)

    endif()

else()

    set_target_properties(msuproj
                          PROPERTIES VERSION   ${PROJECT_VERSION}
                                     SOVERSION ${PROJECT_VERSION_MAJOR}
                                     OUTPUT_NAME msuproj-dev)
    set(MSUPROJ_DEV msuproj)

endif()

if(INSTALL_DEV)

    # Library and CMake export files
    install(TARGETS ${MSUPROJ_DEV}
            EXPORT  MSUProjStaticConfig
            DESTINATION ${INSTALL_PATH_LIB}
            COMPONENT libdev)
    install(EXPORT MSUProjStaticConfig
            DESTINATION ${INSTALL_PATH_CMAKE}
            COMPONENT libdev)

    # Headers
    file(GLOB H_FILES *.h)
    list(APPEND H_FILES ${CMAKE_BINARY_DIR}/msuproj_version.h
                        ${CMAKE_BINARY_DIR}/msuproj_export.h)
    foreach(H_FILE ${H_FILES})
        get_filename_component(H_FILE_NAME ${H_FILE} NAME)
        set(OUT_H_FILE ${CMAKE_BINARY_DIR}/include/${H_FILE_NAME})
        list(APPEND OUT_H_FILES ${OUT_H_FILE})
        if(NOT EXISTS ${OUT_H_FILE})
            remove_comments(${H_FILE} ${OUT_H_FILE})
        endif()
    endforeach()

    install(FILES ${OUT_H_FILES}
            DESTINATION ${INSTALL_PATH_DEV}
            COMPONENT headers)

endif()

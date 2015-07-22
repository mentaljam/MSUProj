##################### MSUProj Library ######################

add_library(msuproj msuproj.cpp logoimage.hpp)
target_link_libraries(msuproj ${GDAL_LIBRARIES})

if(BUILD_SHARED_LIBS)
    install(TARGETS msuproj
            RUNTIME DESTINATION ${INSTALL_PATH_BIN}
            LIBRARY DESTINATION ${INSTALL_PATH_LIB}
            COMPONENT lib)
elseif(INSTALL_DEV)
    install(TARGETS msuproj
            ARCHIVE DESTINATION ${INSTALL_PATH_LIB}
            COMPONENT libdev)
endif()

if(INSTALL_DEV)
    set(HDR_OUT_FILE ${CMAKE_BINARY_DIR}/include/msuproj.h)
    if(NOT EXISTS ${HDR_OUT_FILE})
        remove_comments(msuproj.h ${HDR_OUT_FILE})
    endif()
    install(FILES ${HDR_OUT_FILE}
            DESTINATION ${INSTALL_PATH_DEV}
            COMPONENT headers)
endif()

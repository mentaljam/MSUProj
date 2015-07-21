##################### MSUProj Library ######################

add_library(msuproj msuproj.cpp logoimage.hpp)
target_link_libraries(msuproj ${GDAL_LIBRARIES})

if(BUILD_SHARED_LIBS)
    install(TARGETS msuproj
            RUNTIME DESTINATION ${INSTALL_PATH_BIN}
            LIBRARY DESTINATION ${INSTALL_PATH_LIB}
            COMPONENT lib)
    set(COMPONENTS_ALL "${COMPONENTS_ALL} lib")
else()
    install(TARGETS msuproj
            ARCHIVE DESTINATION ${INSTALL_PATH_LIB}
            COMPONENT dev)
    set(COMPONENTS_ALL "${COMPONENTS_ALL} lib-dev")
endif()

set(HDR_OUT_FILE ${CMAKE_BINARY_DIR}/include/msuproj.h)
if(NOT EXISTS ${HDR_OUT_FILE})
    remove_comments(msuproj.h ${HDR_OUT_FILE})
endif()

install(FILES ${HDR_OUT_FILE}
        DESTINATION ${INSTALL_PATH_DEV}
        COMPONENT headers)
set(COMPONENTS_ALL "${COMPONENTS_ALL} headers")

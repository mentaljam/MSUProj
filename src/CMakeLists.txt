####################### MSUProj-CLI ########################

add_executable(msuproj-cli msuproj-cli.cpp
                           msuproj.cpp
                           logoimage.hpp)
target_link_libraries(msuproj-cli ${GDAL_LIBRARIES})

install(TARGETS msuproj-cli DESTINATION ${INSTALL_PATH_BIN} COMPONENT app)
set(CPACK_COMPONENT_APP_REQUIRED ON)

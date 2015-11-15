###################### MSUProj Tools #######################


add_executable(image2class EXCLUDE_FROM_ALL image2class.cpp)
target_link_libraries(image2class ${GDAL_LIBRARIES})

add_executable(gcp-thinner gcp-thinner.cpp ../msuproj/msucore.cpp)
install(TARGETS gcp-thinner
        DESTINATION ${INSTALL_PATH_BIN}
        COMPONENT gcpthinner OPTIONAL)

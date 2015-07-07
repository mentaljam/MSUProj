###################### MSUProj Tools #######################


add_executable(image2class EXCLUDE_FROM_ALL image2class.cpp)
target_link_libraries(image2class ${GDAL_LIBRARIES})

add_executable(gcp-thiner gcp-thiner.cpp)
install(TARGETS gcp-thiner DESTINATION ${INSTALL_PATH_BIN} COMPONENT tools OPTIONAL)

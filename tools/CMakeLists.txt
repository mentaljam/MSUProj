add_executable(image2class EXCLUDE_FROM_ALL
    src/image2class.cpp)
target_link_libraries(image2class ${GDAL_LIBRARIES})

add_executable(gcp-thinner EXCLUDE_FROM_ALL
    src/gcp-thinner.cpp
    ../library/src/msucore.cpp)

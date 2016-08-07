#### GDAL paths
set(GDAL_PREFIX "" CACHE PATH "Additional path for searching GDAL")

#### Find GDAL and include its directories
if(GDAL_PREFIX)
    list(APPEND CMAKE_SYSTEM_PREFIX_PATH ${GDAL_PREFIX})
endif()
find_package(GDAL REQUIRED)
include_directories(${GDAL_INCLUDE_DIRS})

##### Determine GDAL version
get_defenition("gdal_version.h" "GDAL_RELEASE_NAME" "${GDAL_INCLUDE_DIRS}" GDAL_VERSION)

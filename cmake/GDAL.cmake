####################### GDAL library #######################

#### Find GDAL and include its directories
if(SEARCH_PREFIX)
    list(APPEND CMAKE_SYSTEM_PREFIX_PATH ${SEARCH_PREFIX})
endif()

find_package(GDAL)

if(NOT GDAL_FOUND)
    message(FATAL_ERROR "Could not find GDAL, try to install gdal-dev "
                        "or specify SEARCH_PREFIX")
endif()

include_directories(${GDAL_INCLUDE_DIRS})

#### Determine GDAL version
if(NOT DEFINED GDAL_VERSION)
    message(STATUS "Retrieving GDAL version")
    get_defenition("gdal_version.h" "GDAL_RELEASE_NAME" GDAL_VERSION)
    set(GDAL_VERSION ${GDAL_VERSION} CACHE STRING "GDAL version")
endif()

message(STATUS "Using GDAL v${GDAL_VERSION}")

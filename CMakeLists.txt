cmake_minimum_required(VERSION 3.3)

#### OMZ modules
find_package(OMZModules PATHS third-party)

#### Project version
get_version_from_git(VERSION VERSION_DATE)
add_changelog_target(print_changelog)

#### Set project
project("MSUProj" VERSION ${VERSION})
omz_init_vars()

#### Build options
include(CMakeDependentOption)
option(GDAL_PROJ4_STATIC "GDAL is built with static Proj4" OFF)
option(BUILD_MSUPROJ_CLI "Build the command line application" ON)
option(BUILD_MSUPROJ_QT "Build the qt application" ON)
option(WITH_UPDATES_ACTION "Enable update mechanism for MSUProj-Qt" ON)
option(INSTALL_DEV "Install development files" OFF)
option(INSTALL_RUNTIME "Install runtime libraries on Windows" ON)
cmake_dependent_option(INSTALL_MAN "Install documentation" ON
    BUILD_MSUPROJ_QT OFF)
cmake_dependent_option(WITH_UPDATES_ACTION "Updater functionality" ON
    BUILD_MSUPROJ_QT OFF)

#### Use c++11 standard
set(CMAKE_CXX_STANDARD 11)

#### Compiler flags
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /openmp")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}\
-funroll-loops \
-O3 \
-Wframe-larger-than=1024 \
-fopenmp")
endif()
if(NOT BUILD_SHARED_LIBS OR NOT MSVC)
    add_definitions(-DMSUPROJ_STATIC_DEFINE=1)
endif()

#### Find GDAL
include(cmake/FindGDAL.cmake)

#### Install paths
if(WIN32)
    set(INSTALL_PATH_BIN   .)
    set(INSTALL_PATH_I18N  translations)
    set(INSTALL_PATH_DOCS  doc/msuproj)
    set(INSTALL_PATH_MAN   help)
    set(INSTALL_PATH_INCLUDE include/msuproj)
    set(INSTALL_PATH_CMAKE cmake)
else()
    set(INSTALL_PATH_BIN   bin)
    set(INSTALL_PATH_I18N  share/msuproj/translations)
    set(INSTALL_PATH_DOCS  share/msuproj/doc)
    set(INSTALL_PATH_MAN   share/msuproj/help)
    set(INSTALL_PATH_INCLUDE share/include/msuproj)
    set(INSTALL_PATH_CMAKE share/msuproj/cmake)
endif()
set(INSTALL_PATH_LIB lib)

#### Icons
add_subdirectory(icons)

#### CPack configuration
include(package/CMakeLists.txt)

#### Library
include_directories(
    "library/src"
    "${CMAKE_BINARY_DIR}/library")
add_subdirectory(library)

#### Applications
if(BUILD_MSUPROJ_CLI)
    add_subdirectory(msuproj-cli)
endif()
if(BUILD_MSUPROJ_QT)
    add_subdirectory(msuproj-qt)
endif()

#### Tools
add_subdirectory(tools)

#### Developer documentation
add_subdirectory(doc)

#### Install runtime
if(WIN32 AND INSTALL_RUNTIME)
    include(cmake/InstallRuntime.cmake)
endif()

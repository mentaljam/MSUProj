cmake_minimum_required(VERSION 3.0)

project("msuproj")
set(PROJECT_NAME "MSUProj")


####################### Build options ######################

#### Install development files
option(INSTALL_DEV "Install development files" OFF)

#### Build cli
option(BUILD_CLI "Build command line application" OFF)

#### Build qt
option(BUILD_QT "Build qt application" OFF)

#### Build tools
option(BUILD_TOOLS "Build additional tools" OFF)

#### Install runtime
option(INSTALL_RUNTIME "Install runtime libraries on Windows" ON)

#### Install documentation
option(INSTALL_DOCS "Install documentation" ON)

#### Add "Check for updates" action on windows
option(WITH_UPDATES_ACTION "Check for updates action" ON)

#### Compiler flags
set(GCC_FLAGS "-std=gnu++11 -funroll-loops -O3 -Wframe-larger-than=1024 -fopenmp")


############### Setting additional variables ###############

#### GDAL paths (on Windows)
set(SEARCH_PREFIX "" CACHE PATH "Additional path for searching packages")


####################### Setting paths ######################

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(DOC_DIR         ${PROJECT_SOURCE_DIR}/doc)
set(RESOURCES_DIR   ${PROJECT_SOURCE_DIR}/res)
set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/3rd)

if(WIN32)
    set(INSTALL_PATH_BIN   .)
    set(INSTALL_PATH_I18N  i18n)
    set(INSTALL_PATH_DOCS  doc/${CMAKE_PROJECT_NAME})
    set(INSTALL_PATH_MAN   help)
    set(INSTALL_PATH_DEV   include/${CMAKE_PROJECT_NAME})
    set(INSTALL_PATH_CMAKE cmake)
else()
    set(INSTALL_PATH_BIN   bin)
    set(INSTALL_PATH_I18N  share/${CMAKE_PROJECT_NAME}/i18n)
    set(INSTALL_PATH_DOCS  share/${CMAKE_PROJECT_NAME}/doc)
    set(INSTALL_PATH_MAN   share/${CMAKE_PROJECT_NAME}/help)
    set(INSTALL_PATH_DEV   share/include/${CMAKE_PROJECT_NAME})
    set(INSTALL_PATH_CMAKE share/${CMAKE_PROJECT_NAME}/cmake)
endif()
set(INSTALL_PATH_LIB lib)


###################### CMake modules #######################

#### Set of CMake modules (https://github.com/mentaljam/OMZModules)
find_package(OMZModules)

#### Run OMZ preconfiguring script
include(OMZProjectPreConf)

#### Configure doxygen
include(OMZDoxyConf)

#### Find GDAL
include(GDAL)


########################## Targets #########################

######## Library ########
include_directories(src/msuproj)
add_subdirectory(src/msuproj msuproj)

##### Applications ######
if(BUILD_CLI)
    include_directories(src/cli)
    add_subdirectory(src/cli msuproj-cli)
endif()
if(BUILD_QT)
    include_directories(src/qt ${CMAKE_BINARY_DIR}/msuproj-qt)
    add_subdirectory(src/qt msuproj-qt)
endif()

######### Tools #########
if(BUILD_TOOLS)
    add_subdirectory(src/tools/ tools)
endif()


##################### Post configuration ###################

#### Configure doxygen targets
include(DoxygenTargets)

#### Configure packaging rules
include(Packaging)

#### Run OMZ postconfiguring script
include(OMZProjectPostConf)

cmake_minimum_required(VERSION 3.0)

project("msuproj")
set(PROJECT_NAME "MSUProj")


####################### Build options ######################

#### Build architecture
# 32 - 32bit, 64 - 64bit
set(ARCH "" CACHE STRING "Build x86 or x86-64 application")

#### Additional packages paths
# set if cmake can not find one of them
set(DOXYGEN_EXECUTABLE "" CACHE FILEPATH "Doxygen path")
set(GIT      "" CACHE FILEPATH "Git path (need for version number defining)")
set(HHC      "" CACHE FILEPATH "HTML Help generator path")
set(PDFLATEX "" CACHE FILEPATH "PDFLatex path")

#### Documentation
set(GENERATE_HTML     NO CACHE STRING "Documentation in HTML")
set(GENERATE_HTMLHELP NO CACHE STRING "Documentation in HTML Help")
set(GENERATE_LATEX    NO CACHE STRING "Documentation in LaTeX")
set(GENERATE_PDF      NO CACHE STRING "Documentation in PDF")
set(GENERATE_MAN      NO CACHE STRING "Documentation in MAN")
set(GENERATE_RTF      NO CACHE STRING "Documentation in RTF")
set(GENERATE_XML      NO CACHE STRING "Documentation in XML")

#### Install development files
option(INSTALL_DEV "Install development files" OFF)

#### Build cli
option(BUILD_CLI "Build command line application" OFF)

#### Build qt
option(BUILD_QT "Build qt application" OFF)

#### Build tools
option(BUILD_TOOLS "Build additional tools" OFF)

#### Compiler flags
set(GCC_FLAGS "-std=gnu++11 -funroll-loops -O3 -Wframe-larger-than=1024")


############### Setting additional variables ###############

set(QT ${BUILD_QT})
set(CMAKE_BUILD_TYPE "Release")

#### GDAL paths (on Windows)
set(SEARCH_PREFIX "" CACHE PATH "Additional path for searching packages")


####################### Setting paths ######################


set(DOC_DIR         ${PROJECT_SOURCE_DIR}/doc)
set(RESOURCES_DIR   ${PROJECT_SOURCE_DIR}/res)
set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/3rd)

if(WIN32)
    set(INSTALL_PATH_BIN  .)
    set(INSTALL_PATH_I18N i18n)
    set(INSTALL_PATH_DOCS doc/${CMAKE_PROJECT_NAME})
    set(INSTALL_PATH_DEV  include/${CMAKE_PROJECT_NAME})
else()
    set(INSTALL_PATH_BIN  bin)
    set(INSTALL_PATH_I18N share/${CMAKE_PROJECT_NAME}/i18n)
    set(INSTALL_PATH_DOCS share/${CMAKE_PROJECT_NAME}/doc)
    set(INSTALL_PATH_DEV  share/include/${CMAKE_PROJECT_NAME})
endif()
set(INSTALL_PATH_LIB lib)


###################### CMake modules #######################

include(OMZProjectPreConf)
include(OMZDoxyConf)
include(OMZFunctions)


####################### GDAL library #######################

if(WIN32 AND CMAKE_COMPILER_IS_GNUCC)
    list(APPEND CMAKE_SYSTEM_PREFIX_PATH ${SEARCH_PREFIX})
endif()

find_package(GDAL)

if(NOT GDAL_FOUND)
    message(FATAL_ERROR "Could not find GDAL, try to install gdal-dev "
                        "or specify CMAKE_SYSTEM_PREFIX_PATH")
endif()

include_directories(${GDAL_INCLUDE_DIRS})


########################## Targets #########################

######## Library ########
include_directories(src)
add_subdirectory(src/ msuproj)

##### Applications ######
if(BUILD_CLI)
    include_directories(src src/cli)
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

include(packaging)
include(OMZProjectPostConf)

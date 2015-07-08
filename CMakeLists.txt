cmake_minimum_required(VERSION 3.0)

project("msuproj")
set(PROJECT_NAME "MSUProj")


####################### Build options ######################

#### Build locale
set(LOCALE "Russian" CACHE STRING "Build locale (English or Russian)")

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

#### Build tools
option(BUILD_TOOLS "Build additional tools" OFF)

#### Compiler flags
set(GCC_FLAGS "-std=gnu++11 -funroll-loops -O3 -Wframe-larger-than=1024")


############### Setting additional variables ###############

set(QT NO)
set(VERSION_STATUS "")
set(CMAKE_BUILD_TYPE "Release")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME} Project
 ${PROJECT_NAME} is a project for georeferencing images from MSU-MR sensing equipment of Russian ERS satellite Meteor-M.
 Data from Meteor-M MSU-MR equipment is broadcasted at frequencies of 137.1 and 137.9 MHz in the LRPT format and can be obtained by everyone under \\\"Free Space\\\" program.
 All additional information in English can be found on meteor.robonuka.ru.
 This package contains ${PROJECT_NAME} command line utility.")
set(CPACK_PACKAGE_VENDOR "NTs OMZ")
set(COPYRIGHT "Research Center for Earth Operative Monitoring (NTs OMZ) <www.ntsomz.ru>")
set(CPACK_PACKAGE_CONTACT "Petr Tsymbarovich <tpr@ntsomz.ru>")
set(WEB "http://www.ntsomz.ru")
set(CPACK_SOURCE_IGNORE_FILES "/\\\\.git/"
                              "/\\\\3rd/"
                              "/\\\\cmake/"
                              "/\\\\.gitignore"
                              "/\\\\CMakeLists.txt.user")

set(DOC_DIR         ${PROJECT_SOURCE_DIR}/doc)
set(RESOURCES_DIR   ${PROJECT_SOURCE_DIR}/res)
set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/3rd)

#### GDAL paths (on Windows)
set(SEARCH_PREFIX "" CACHE PATH "Additional path for searching packages")


####################### Setting paths ######################


set(DOC_DIR         ${PROJECT_SOURCE_DIR}/doc)
set(RESOURCES_DIR   ${PROJECT_SOURCE_DIR}/res)
set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/3rd)

if(WIN32)
    set(INSTALL_PATH_BIN  .)
    set(INSTALL_PATH_DOCS doc/${CMAKE_PROJECT_NAME})
    set(INSTALL_PATH_DEV  include/${CMAKE_PROJECT_NAME})
else()
    set(INSTALL_PATH_BIN  bin)
    set(INSTALL_PATH_DOCS share/${CMAKE_PROJECT_NAME}/doc)
    set(INSTALL_PATH_DEV  share/include/${CMAKE_PROJECT_NAME})
endif()


###################### CMake modules #######################

include(myProjectPreConf)
include(myDoxyConf)


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

##### Applications ######
add_subdirectory(src/ msuproj-cli)

######### Tools #########
if(BUILD_TOOLS)
    add_subdirectory(src/tools/ tools)
endif()


##################### Post configuration ###################

include(packaging)
include(myProjectPostConf)

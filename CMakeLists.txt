cmake_minimum_required(VERSION 2.8)

project("msugeo")


####################### Build options ######################

#### Build architecture
# NO - 32bit, YES - 64bit
option(IF64 "Build x86-64 application" NO)

#### Additional packages paths
# set if cmake can not find one of them
set(DOXYGEN_EXECUTABLE "" CACHE FILEPATH "Doxygen path")
set(GIT      "" CACHE FILEPATH "Git path (need for version number defining)")
set(HHC      "" CACHE FILEPATH "HTML Help generator path")
set(PDFLATEX "" CACHE FILEPATH "PDFLatex path")

#### Documentation
set(DOC_LANGUAGE "English" CACHE STRING "Language (English or Russian)")
set(GENERATE_HTML     NO CACHE STRING "Documentation in HTML")
set(GENERATE_HTMLHELP NO CACHE STRING "Documentation in HTML Help")
set(GENERATE_LATEX    NO CACHE STRING "Documentation in LaTeX")
set(GENERATE_PDF      NO CACHE STRING "Documentation in PDF")
set(GENERATE_MAN      NO CACHE STRING "Documentation in MAN")
set(GENERATE_RTF      NO CACHE STRING "Documentation in RTF")
set(GENERATE_XML      NO CACHE STRING "Documentation in XML")

#### Compiler flags
set(GCC_FLAGS "-std=gnu++11 -funroll-loops -O3")
set(CMAKE_CXX_FLAGS "" CACHE STRING "CXX compiler flags")
set(CMAKE_AUTOMOC ON)


############### Setting additional variables ###############

set(VERSION_STATUS "")
set(CMAKE_BUILD_TYPE "Release")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "")
set(CPACK_PACKAGE_VENDOR "NTs OMZ")
set(COPYRIGHT "Research Center for Earth Operative Monitoring (NTs OMZ) <www.ntsomz.ru>")
set(CPACK_PACKAGE_CONTACT "Petr Tsymbarovich <tpr@ntsomz.ru>")
set(WEB "http://www.ntsomz.ru")
set(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/LICENSE.txt)
set(CPACK_SOURCE_IGNORE_FILES "/\\\\.git/"
                              "/\\\\3rd/"
                              "/\\\\.gitignore"
                              "/\\\\CMakeLists.txt.user")

set(DOC_DIR         ${PROJECT_SOURCE_DIR}/doc)
set(RESOURCES_DIR   ${PROJECT_SOURCE_DIR}/res)
set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/3rd)

#### GDAL paths (on Windows)
set(GDAL_INCLUDE_DIRS "C:\\msys\\1.0\\local\\include" CACHE FILEPATH "GDAL headers path")
set(GDAL_DLL ${CMAKE_BINARY_DIR}/libgdal-1.dll CACHE FILEPATH "GDAL precompiled dll")


###################### CMake modules #######################

include(myProjectConf)
include(myDoxyConf)


####################### GDAL library #######################

if(WIN32 AND CMAKE_COMPILER_IS_GNUCC AND EXISTS ${GDAL_DLL})
    add_library(GDAL STATIC IMPORTED)
    set_property(TARGET GDAL PROPERTY IMPORTED_LOCATION ${GDAL_DLL})
    set(GDAL_LIBRARIES GDAL)
    set(GDAL_FOUND YES)
endif(WIN32 AND CMAKE_COMPILER_IS_GNUCC AND EXISTS ${GDAL_DLL})

if(NOT GDAL_FOUND)
    find_package(GDAL)
endif(NOT GDAL_FOUND)

if(NOT GDAL_FOUND)
    message(FATAL_ERROR "Could not find GDAL, try to install gdal-dev")
endif(NOT GDAL_FOUND)

include_directories(${GDAL_INCLUDE_DIRS})


######################## Executables #######################

add_executable(msugeo-cli src/msugeo-cli.cpp
                          src/msugeo.cpp
                          src/logomark.hpp)
target_link_libraries(msugeo-cli ${GDAL_LIBRARIES})

add_executable(image2class EXCLUDE_FROM_ALL
                           src/tools/image2class.cpp)
target_link_libraries(image2class ${GDAL_LIBRARIES})

add_executable(gcp-thiner src/tools/gcp-thiner.cpp)


################### Setting install paths ##################

if(WIN32)
    set (BIN .)
    install(FILES ${CPACK_RESOURCE_FILE_LICENSE} DESTINATION . COMPONENT license)
    set(COMPONENTS_ALL license)
    if (EXISTS ${DLL_ARCH})
        install(FILES ${DLLS} DESTINATION . COMPONENT libs OPTIONAL)
        set(CPACK_COMPONENT_LIBS_DISPLAY_NAME "Runtime")
        set(CPACK_COMPONENT_LIBS_DESCRIPTION "Thirdparty runtime libraries")
        set(COMPONENTS_ALL "${COMPONENTS_ALL} libs")
    endif (EXISTS ${DLL_ARCH})
else(WIN32)
    set (BIN bin)
endif(WIN32)

install(TARGETS msugeo-cli DESTINATION ${BIN} COMPONENT cli)
install(TARGETS gcp-thiner DESTINATION ${BIN} COMPONENT tools OPTIONAL)

set(COMPONENTS_ALL "${COMPONENTS_ALL} cli tools")
set(CPACK_COMPONENT_CLI_DISPLAY_NAME "msugeo cli")
set(CPACK_COMPONENT_CLI_DESCRIPTION "msugeo command line utility")
set(CPACK_COMPONENT_TOOLS_DISPLAY_NAME "Tools")
set(CPACK_COMPONENT_TOOLS_DESCRIPTION "msugeo additional tools")
set(CPACK_COMPONENT_LICENSE_HIDDEN ON)
#set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_WIX_CULTURES "en-US")
include(CPack)

###################### Generated Files #####################

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${GENERATED_FILES}")

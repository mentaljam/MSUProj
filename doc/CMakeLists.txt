#### Find Doxygen
if(NOT DOXYGEN_EXECUTABLE)
    find_program(DOXYGEN_EXECUTABLE doxygen COMMENT "Doxygen executable")
endif()
if(NOT DOXYGEN_EXECUTABLE)
    message(WARNING "Doxygen was not found, MSUProj-Qt developer documentation will not be generated.")
endif()

#### Find git
if(NOT GIT_EXECUTABLE)
    find_program(GIT_EXECUTABLE git COMMENT "Git executable")
endif()
if(NOT GIT_EXECUTABLE)
    message(WARNING "Git was not found, MSUProj-Qt developer documentation will not be generated.")
endif()

if(DOXYGEN_EXECUTABLE AND GIT_EXECUTABLE)

     file(GLOB_RECURSE DOC_DEV_SOURCES RELATIVE "${CMAKE_SOURCE_DIR}"
          "${CMAKE_SOURCE_DIR}/library/src/*.h"
          "${CMAKE_SOURCE_DIR}/library/src/*.cpp")
     set(SOURCE_VERSIONS_FILE "${CMAKE_CURRENT_BINARY_DIR}/sources.h")
     write_sources_versions(${SOURCE_VERSIONS_FILE} "${DOC_DEV_SOURCES}")

     configure_doxy_file("${CMAKE_CURRENT_BINARY_DIR}/doxyfile"
         INPUT "${CMAKE_SOURCE_DIR}/library/src/"
               "${SOURCE_VERSIONS_FILE}"
               "${CMAKE_CURRENT_SOURCE_DIR}/translations.dox"
               "${CMAKE_SOURCE_DIR}/README.md"
         PROJECT_NAME "MSUProj-Dev"
         PROJECT_LOGO "${CMAKE_SOURCE_DIR}/icons/msuproj.svg"
         OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
         OUTPUT_LANGUAGE English
         FORMATS HTML
         RECURSIVE
         USE_MDFILE_AS_MAINPAGE README.md)
     add_custom_target(update_gh-page
         COMMAND ${DOXYGEN_EXECUTABLE} doxyfile
         COMMAND ${CMAKE_COMMAND} -E remove_directory html/.git
         COMMAND ${GIT_EXECUTABLE} -C html init
         COMMAND ${GIT_EXECUTABLE} -C html remote add github git@github.com:mentaljam/MSUProj.git
         COMMAND ${GIT_EXECUTABLE} -C html checkout --orphan gh-pages
         COMMAND ${GIT_EXECUTABLE} -C html add -A
         COMMAND ${GIT_EXECUTABLE} -C html commit -m "Update documentation for version ${PROJECT_VERSION} (${VERSION_DATE})"
         COMMAND ${GIT_EXECUTABLE} -C html push -f github gh-pages
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
         COMMENT "Upload project documentation to GitHub pages"
         VERBATIM)

else()
    message(WARNING "Doxygen was not found, MSUProj-Qt developer documentation will not be generated.")
endif()

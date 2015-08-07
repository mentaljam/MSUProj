############### Update GitHub Documentation ################

if(GIT AND DOXYGEN_EXECUTABLE)

    list(APPEND DOXYGEN_FORMATS "html")

    add_custom_target(update_gh-page
                      COMMAND ${CMAKE_COMMAND} -E remove_directory .git
                      COMMAND ${GIT} init
                      COMMAND ${GIT} remote add github git@github.com:mentaljam/MSUProj.git
                      COMMAND ${GIT} checkout --orphan gh-pages
                      COMMAND ${GIT} add -A
                      COMMAND ${GIT} commit -m "Update documentation for version ${V_VERSION} (${V_DATE})"
                      COMMAND ${GIT} push -f github gh-pages
                      DEPENDS build_docs
                      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/doc/developer/html
                      COMMENT "Upload project documentation to GitHub pages"
                      VERBATIM
    )

endif()


################### MSUProj Documentation ##################

if(DOXYGEN_FORMATS)

    add_doxy_target(INPUT ${CMAKE_SOURCE_DIR}/src/msuproj
                          ${CMAKE_BINARY_DIR}/sources.h
                    NAME "msuproj-dev"
                    OUT_DIRECTORY ${CMAKE_BINARY_DIR}/doc/developer
                    LANGUAGE "English"
                    FORMATS ${DOXYGEN_FORMATS}
    )

endif()

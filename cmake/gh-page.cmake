############### Update GitHub Documentation ################

if(GIT AND DOXYGEN_EXECUTABLE AND GENERATE_HTML)

    add_custom_target(update_gh-page
                      COMMAND ${CMAKE_COMMAND} -E remove_directory .git
                      COMMAND ${GIT} init
                      COMMAND ${GIT} remote add github git@github.com:mentaljam/MSUProj.git
                      COMMAND ${GIT} checkout --orphan gh-pages
                      COMMAND ${GIT} add -A
                      COMMAND ${GIT} commit -m "Update documentation for version ${V_VERSION} (${V_DATE})"
                      COMMAND ${GIT} push -f github gh-pages
                      DEPENDS dev_doc
                      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/doc/developer/html
                      COMMENT "Upload project documentation to GitHub pages"
                      VERBATIM
    )

endif()

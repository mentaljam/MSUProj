#### Common Qt modules
set(QT_MODULES
    Widgets
    Help)

#### Windows specific modules
if(WIN32)
    list(APPEND QT_MODULES WinExtras)
endif()

#### Find Qt libraries
find_package(Qt5 COMPONENTS ${QT_MODULES} REQUIRED)
foreach(MODULE ${QT_MODULES})
    list(APPEND QT_LINK_LIBRARIES Qt5::${MODULE})
endforeach()

#### Find Qt Linguist Tools for translations generation
find_package(Qt5LinguistTools)
if(NOT Qt5LinguistTools_FOUND)
    message(WARNING "Qt5LinguistTools were not found, translations will not be generated.")
endif()

#### Determine Qt version number
if(NOT QT_VERSION)
    execute_process(COMMAND $ENV{QTDIR}/bin/qmake -query QT_VERSION
        OUTPUT_VARIABLE QT_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(QT_VERSION ${QT_VERSION} CACHE STRING "Qt version")
    message(STATUS "Using Qt v${QT_VERSION}")
endif()

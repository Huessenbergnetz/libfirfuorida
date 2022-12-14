# SPDX-FileCopyrightText: (C) 2019-2022 Matthias Fehring <https://www.huessenbergnetz.de>
# SPDX-License-Identifier: LGPL-3.0-or-later

cmake_minimum_required(VERSION 3.12.4 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)

if (POLICY CMP0043)
  cmake_policy(SET CMP0043 NEW)
endif()
if (POLICY CMP0063)
  cmake_policy(SET CMP0063 NEW)
endif()

project(firfuorida
    VERSION 0.0.1
    DESCRIPTION "Qt based database migration library"
    HOMEPAGE_URL https://github.com/Huessenbergnetz/libfirfuorida
    LANGUAGES CXX)

include(GNUInstallDirs)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Sql REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} 5.6.0 REQUIRED COMPONENTS Core Sql)

set(FIRFUORIDA_API_LEVEL "0")

# Auto generate moc files
set(CMAKE_AUTOMOC ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

# Forbid in-tree building
if(${CMAKE_SOURCE_DIR} MATCHES ${CMAKE_BINARY_DIR})
      message(STATUS "Please do an out-of-tree build:")
      message(STATUS "rm -f CMakeCache.txt && mkdir build && cd build; cmake .. && make")
      message(FATAL_ERROR "In-tree-build detected!")
endif(${CMAKE_SOURCE_DIR} MATCHES ${CMAKE_BINARY_DIR})

option(ENABLE_TESTS "Build the unit tests" OFF)
option(ENABLE_ASAN "Enable the use of address sanitization" OFF)
option(BUILD_DOCS "Enable the build of doxygen docs" OFF)
option(BUILD_DOCS_QUIET "Tell doxygen to be quiet while building the documentation." OFF)

include(GenerateExportHeader)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

if (ENABLE_TESTS)
    enable_testing()
endif (ENABLE_TESTS)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "libfirfuorida default install prefix" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

# cmake config files
include(CMakePackageConfigHelpers)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/FirfuoridaQt${QT_VERSION_MAJOR}ConfigVersion.cmake
    VERSION ${firfuorida_VERSION}
    COMPATIBILITY AnyNewerVersion
)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/Firfuorida-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/FirfuoridaQt${QT_VERSION_MAJOR}Config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/FirfuoridaQt${QT_VERSION_MAJOR}
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/FirfuoridaQt${QT_VERSION_MAJOR}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/FirfuoridaQt${QT_VERSION_MAJOR}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/FirfuoridaQt${QT_VERSION_MAJOR}
    COMPONENT development
)
install(EXPORT FirfuoridaTargets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/FirfuoridaQt${QT_VERSION_MAJOR}
    FILE FirfuoridaQt${QT_VERSION_MAJOR}Targets.cmake
    NAMESPACE FirfuoridaQt${QT_VERSION_MAJOR}::
    COMPONENT development
)

add_subdirectory(Firfuorida)
add_subdirectory(cmd)
if(ENABLE_TESTS)
    add_subdirectory(tests)
endif(ENABLE_TESTS)

if (BUILD_DOCS)
    find_package(Doxygen REQUIRED dot)

    if (BUILD_DOCS_QUIET)
        set(DOXYGEN_QUIET "YES")
        set(DOXYGEN_WARNINGS "NO")
        set(DOXYGEN_WARN_IF_UNDOCUMENTED "NO")
        set(DOXYGEN_WARN_IF_DOC_ERROR "NO")
    else(BUILD_DOCS_QUIET)
        set(DOXYGEN_QUIET "NO")
        set(DOXYGEN_WARNINGS "YES")
        set(DOXYGEN_WARN_IF_UNDOCUMENTED "YES")
        set(DOXYGEN_WARN_IF_DOC_ERROR "YES")
    endif(BUILD_DOCS_QUIET)

    set(DOXYGEN_STRIP_FROM_PATH ${CMAKE_SOURCE_DIR})
    set(DOXYGEN_GENERATE_TAGFILE ${CMAKE_CURRENT_BINARY_DIR}/firfuorida.tags)
    set(DOXYGEN_BUILTIN_STL_SUPPORT "YES")
    set(DOXYGEN_VERBATIM_HEADERS "NO")
    set(DOXYGEN_ALWAYS_DETAILED_SEC "YES")
    set(DOXYGEN_HIDE_UNDOC_MEMBERS "YES")
    set(DOXYGEN_HIDE_UNDOC_CLASSES "YES")
    set(DOXYGEN_LAYOUT_FILE ${CMAKE_SOURCE_DIR}/dox/DoxygenLayout.xml)
    set(DOXYGEN_COLLABORATION_GRAPH "NO")
    set(DOXYGEN_EXAMPLE_PATH ${CMAKE_SOURCE_DIR}/dox/examples)
    set(DOXYGEN_SORT_BRIEF_DOCS "YES")
    set(DOXYGEN_SORT_MEMBERS_CTORS_1ST "YES")
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${CMAKE_SOURCE_DIR}/README.md)
    set(DOXYGEN_EXCLUDE ${CMAKE_SOURCE_DIR}/tests)
    set(DOXYGEN_SHOW_USED_FILES "NO")
    list(APPEND DOXYGEN_PREDEFINED "W_DOXYGEN")

    # Available from http://gcc.gnu.org/onlinedocs/libstdc++/latest-doxygen/libstdc++.tag
    set (DOCS_STLTAGFILE "${CMAKE_SOURCE_DIR}/scripts/libstdc++.tag" CACHE FILEPATH "Path to the doxygen tag file of the libstdc++")
    set (DOCS_STLLOCALHTMLDIR "${CMAKE_SOURCE_DIR}/scripts/libstdc++" CACHE PATH "Path to the directory containing the libstdc++ HTML doxygen documentation")

    if (EXISTS DOCS_STLTAGFILE)
        set(DOXYGEN_BUILTIN_STL_SUPPORT "NO")
        list(APPEND ONLINETAGS "${DOCS_STLTAGFILE}=http://gcc.gnu.org/onlinedocs/libstdc++/latest-doxygen/")
        if (IS_DIRECTORY "${DOCS_STLLOCALHTMLDIR}")
            list(APPEND LOCALHTMLTAGS "${DOCS_STLTAGFILE}=${DOCS_STLLOCALHTMLDIR}/")
        endif (IS_DIRECTORY "${DOCS_STLLOCALHTMLDIR}")
        list(APPEND QCHTAGS "${DOCS_STLTAGFILE}=qthelp://org.gnu.libstdcxx/libstdc++/")
    endif (EXISTS DOCS_STLTAGFILE)

    find_program(QMAKE_EXE_PATH NAMES qmake-qt${QT_VERSION_MAJOR} qmake${QT_VERSION_MAJOR} qmake)
    if(QMAKE_EXE_PATH)
        execute_process(COMMAND ${QMAKE_EXE_PATH} -query QT_INSTALL_DOCS OUTPUT_VARIABLE QT_INSTALL_DOCS OUTPUT_STRIP_TRAILING_WHITESPACE)
        if (NOT IS_DIRECTORY "${QT_INSTALL_DOCS}")
            set(QT_INSTALL_DOCS "/usr/share/qt${QT_VERSION_MAJOR}/doc")
        endif (NOT IS_DIRECTORY "${QT_INSTALL_DOCS}")
    else(QMAKE_EXE_PATH)
        set(QT_INSTALL_DOCS "/usr/share/qt${QT_VERSION_MAJOR}/doc")
    endif(QMAKE_EXE_PATH)

    set(QT_ONLINE_DOCS_URL "https://doc.qt.io/qt-${QT_VERSION_MAJOR}/")
    set(QT_DOCS_COMPONENTS "qtcore" "qtsql")

    set(DOCS_QTDOCSDIR ${QT_INSTALL_DOCS} CACHE PATH "Path to a directory containing the Qt Documentation")

    message(STATUS "Searching for Qt documentation at ${DOCS_QTDOCSDIR}")

    foreach(_QTDOCCOMP ${QT_DOCS_COMPONENTS})
        find_file(_QTDOCCOMPTAGS "${_QTDOCCOMP}.tags" PATHS "${DOCS_QTDOCSDIR}" PATH_SUFFIXES "${_QTDOCCOMP}" NO_DEFAULT_PATH)
        if (_QTDOCCOMPTAGS)
            message(STATUS "Found ${_QTDOCCOMP}.tags")
            list(APPEND ONLINETAGS "${_QTDOCCOMPTAGS}=${QT_ONLINE_DOCS_URL}")
            list(APPEND QCHTAGS "${_QTDOCCOMPTAGS}=qthelp://org.qt-project.${_QTDOCCOMP}.${QT_VERSION_MAJOR}${QT_VERSION_MINOR}${QT_VERSION_PATCH}/${_QTDOCCOMP}/")
            if (IS_DIRECTORY "${DOCS_QTDOCSDIR}/${_QTDOCCOMP}")
                message(STATUS "Found local html docs for ${_QTDOCCOMP}")
                list(APPEND LOCALHTMLTAGS "${_QTDOCCOMPTAGS}=${DOCS_QTDOCSDIR}/${_QTDOCCOMP}/")
            endif (IS_DIRECTORY "${DOCS_QTDOCSDIR}/${_QTDOCCOMP}")
        endif (_QTDOCCOMPTAGS)
        unset(_QTDOCCOMPTAGS CACHE)
    endforeach(_QTDOCCOMP ${QT_DOCS_COMPONENTS})

    set(DOXYGEN_HTML_TIMESTAMP "YES")
    set(DOXYGEN_TAGFILES ${ONLINETAGS})
    set(DOXYGEN_HTML_OUTPUT "webdocs")
    doxygen_add_docs(webdocs
        ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generate HTML documentation for online usage"
    )
    message(STATUS "Adding build target webdocs")

    set(DOXYGEN_HTML_TIMESTAMP "NO")
    set(DOXYGEN_TAGFILES ${LOCALHTMLTAGS})
    set(DOXYGEN_HTML_OUTPUT "htmldocs")
    doxygen_add_docs(htmldocs
        ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generate HTML documentation for local usage"
    )
    message(STATUS "Adding build target htmldocs")

    execute_process(COMMAND ${QMAKE_EXE_PATH} -query QT_INSTALL_LIBEXECS OUTPUT_VARIABLE QT_INSTALL_LIBEXECS OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${QMAKE_EXE_PATH} -query QT_INSTALL_BINS OUTPUT_VARIABLE QT_INSTALL_BINS OUTPUT_STRIP_TRAILING_WHITESPACE)
    find_program(DOXYGEN_QHG_LOCATION NAMES qhelpgenerator-qt${QT_VERSION_MAJOR} qhelpgenerator${QT_VERSION_MAJOR} qhelpgenerator HINTS ${QT_INSTALL_LIBEXECS} ${QT_INSTALL_BINS})
    if(DOXYGEN_QHG_LOCATION)
        set(DOXYGEN_TAGFILES ${QCHTAGS})
        set(DOXYGEN_HTML_OUTPUT "qtdocs")
        set(DOXYGEN_GENERATE_QHP "YES")
        set(DOXYGEN_QHP_NAMESPACE "de.huessenbergnetz.firfuorida.${PROJECT_VERSION_MAJOR}${PROJECT_VERSION_MINOR}${PROJECT_VERSION_PATCH}")
        set(DOXYGEN_QHP_VIRTUAL_FOLDER "libfirfuorida")
        set(DOXYGEN_QCH_FILE "${CMAKE_CURRENT_BINARY_DIR}/firfuorida.qch")
        set(DOXYGEN_SEARCHENGINE "NO")
        doxygen_add_docs(qtdocs
            ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generate Qt QCH documentation for local usage"
        )
        message(STATUS "Adding build target qtdocs")
    endif(DOXYGEN_QHG_LOCATION)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/firfuorida.qch
        DESTINATION ${DOCS_QTDOCSDIR}
        COMPONENT docs
        OPTIONAL
    )

    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/htmldocs/
        DESTINATION ${DOCS_QTDOCSDIR}/firfuorida
        COMPONENT docs
        OPTIONAL
    )

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/firfuorida.tags
        DESTINATION ${DOCS_QTDOCSDIR}/firfuorida
        COMPONENT development
        OPTIONAL
    )
endif (BUILD_DOCS)

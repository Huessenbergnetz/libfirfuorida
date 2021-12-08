# SPDX-FileCopyrightText: (C) 2019-2021 Matthias Fehring / www.huessenbergnetz.de
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

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Forbid in-tree building
if(${CMAKE_SOURCE_DIR} MATCHES ${CMAKE_BINARY_DIR})
      message(STATUS "Please do an out-of-tree build:")
      message(STATUS "rm -f CMakeCache.txt && mkdir build && cd build; cmake .. && make")
      message(FATAL_ERROR "In-tree-build detected!")
endif(${CMAKE_SOURCE_DIR} MATCHES ${CMAKE_BINARY_DIR})

option(ENABLE_TESTS "Build the unit tests" OFF)
option(ENABLE_ASAN "Enable the use of address sanitization" OFF)

if (ENABLE_TESTS)
    enable_testing()
endif (ENABLE_TESTS)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "libfirfuorida default install prefix" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

#set(CMAKE_INSTALL_LIBDIR "${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_LIBRARY_ARCHITECTURE}" CACHE PATH "Library install directory")

# cmake config files
configure_file(${CMAKE_MODULE_PATH}/Firfuorida-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/FirfuoridaQt${QT_VERSION_MAJOR}Config.cmake
    @ONLY
)
configure_file(${CMAKE_MODULE_PATH}/Firfuorida-config-version.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/FirfuoridaQt${QT_VERSION_MAJOR}ConfigVersion.cmake
    @ONLY
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/FirfuoridaConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/FirfuoridaConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/FirfuoridaQt${QT_VERSION_MAJOR}/
)
install(EXPORT FirfuoridaTargets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/FirfuoridaQt${QT_VERSION_MAJOR}/
    FILE FirfuoridaTargets.cmake
    NAMESPACE Firfuorida::
    COMPONENT Devel
)

add_subdirectory(Firfuorida)
add_subdirectory(cmd)
if(ENABLE_TESTS)
    add_subdirectory(tests)
endif(ENABLE_TESTS)

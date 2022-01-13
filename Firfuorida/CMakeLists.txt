# SPDX-FileCopyrightText: (C) 2019-2021 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: LGPL-3.0-or-later

set(firfuorida_SRC
    migrator.cpp
    migration.cpp
    table.cpp
    column.cpp
)

set(firfuorida_HEADERS
    firfuorida_global.h
    migrator.h
    Migrator
    migration.h
    Migration
    table.h
    Table
    column.h
)

set(firfuorida_PRIVATE_HEADERS
    migrator_p.h
    migration_p.h
    table_p.h
    column_p.h
)

add_library(FirfuoridaQt${QT_VERSION_MAJOR} SHARED
    ${firfuorida_SRC}
    ${firfuorida_HEADERS}
    ${firfuorida_PRIVATE_HEADERS}
)

add_library(FirfuoridaQt${QT_VERSION_MAJOR}::Core ALIAS FirfuoridaQt${QT_VERSION_MAJOR})

target_compile_features(FirfuoridaQt${QT_VERSION_MAJOR} PUBLIC cxx_std_14)

target_include_directories(FirfuoridaQt${QT_VERSION_MAJOR}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/firfuorida-qt${QT_VERSION_MAJOR}>
)

target_compile_definitions(FirfuoridaQt${QT_VERSION_MAJOR}
    PRIVATE
        QT_NO_KEYWORDS
        QT_NO_CAST_TO_ASCII
        QT_NO_CAST_FROM_ASCII
        QT_STRICT_ITERATORS
        QT_NO_URL_CAST_FROM_STRING
        QT_NO_CAST_FROM_BYTEARRAY
        QT_USE_QSTRINGBUILDER
        QT_NO_SIGNALS_SLOTS_KEYWORDS
        QT_USE_FAST_OPERATOR_PLUS
        QT_DISABLE_DEPRECATED_BEFORE=0x050900
        $<$<NOT:$<CONFIG:Debug>>:QT_NO_DEBUG_OUTPUT>
)

set_target_properties(FirfuoridaQt${QT_VERSION_MAJOR} PROPERTIES
    EXPORT_NAME Firfuorida
    VERSION ${PROJECT_VERSION}
    SOVERSION ${FIRFUORIDA_API_LEVEL}
    PUBLIC_HEADER "${firfuorida_HEADERS}"
)

if (ENABLE_ASAN)
    target_link_libraries(FirfuoridaQt${QT_VERSION_MAJOR}
        PRIVATE
            asan
    )
    target_compile_options(FirfuoridaQt${QT_VERSION_MAJOR}
        PRIVATE
            -fsanitize=address
            -fno-omit-frame-pointer
            -Wformat
            -Werror=format-security
            -Werror=array-bounds
            -g
            -ggdb
    )
    set_target_properties(FirfuoridaQt${QT_VERSION_MAJOR} PROPERTIES
        LINK_FLAGS -fsanitize=address
    )
endif (ENABLE_ASAN)

target_link_libraries(FirfuoridaQt${QT_VERSION_MAJOR}
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Sql
)

install(TARGETS FirfuoridaQt${QT_VERSION_MAJOR}
    EXPORT FirfuoridaTargets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT runtime
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT runtime
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT runtime
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/firfuorida-qt${QT_VERSION_MAJOR}/Firfuorida COMPONENT development
)

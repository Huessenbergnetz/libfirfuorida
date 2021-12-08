# SPDX-FileCopyrightText: (C) 2019-2021 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: LGPL-3.0-or-later

project(firfuorida_tests)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Test REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} 5.6.0 REQUIRED COMPONENTS Test)

add_library(migrationtest STATIC migrationtestobject.cpp migrationtestobject.h)

target_compile_features(migrationtest
    PRIVATE
        cxx_auto_type
    PUBLIC
        cxx_nullptr
        cxx_override
)

if (ENABLE_ASAN)
    target_link_libraries(migrationtest
        PRIVATE
            asan
    )
    target_compile_options(migrationtest
        PRIVATE
            -fsanitize=address
            -fno-omit-frame-pointer
            -Wformat
            -Werror=format-security
            -Werror=array-bounds
            -g
            -ggdb
    )
    set_target_properties(migrationtest PROPERTIES
        LINK_FLAGS -fsanitize=address
    )
endif (ENABLE_ASAN)

target_link_libraries(migrationtest
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Sql
        Qt${QT_VERSION_MAJOR}::Test
        FirfuoridaQt${QT_VERSION_MAJOR}
)

function(firfuorida_test _testname _link1 _link2 _link3)
    add_executable(${_testname}_exec ${_testname}.cpp)
    add_test(NAME ${_testname} COMMAND ${_testname}_exec)
    if (ENABLE_ASAN)
        target_link_libraries(${_testname}_exec
            PRIVATE
                asan
        )
        target_compile_options(${_testname}_exec
            PRIVATE
                -fsanitize=address
                -fno-omit-frame-pointer
                -Wformat
                -Werror=format-security
                -Werror=array-bounds
                -g
                -ggdb
        )
        set_target_properties(${_testname}_exec PROPERTIES
            LINK_FLAGS -fsanitize=address
        )
    endif (ENABLE_ASAN)
    target_link_libraries(${_testname}_exec PRIVATE migrationtest ${_link1} ${_link2} ${_link3})
    set_property(TEST ${_testname} PROPERTY ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/Firfuorida")
endfunction(firfuorida_test _testname _link1 _link2 _link3)

firfuorida_test(testmigration "" "" "")

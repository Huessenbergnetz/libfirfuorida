# SPDX-FileCopyrightText: (C) 2019-2021 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: LGPL-3.0-or-later

project(firfuorida_tests)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Test REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} 5.6.0 REQUIRED COMPONENTS Test)

set(testmigration_SRCS
    migrations/m20220119t181049_tiny.h
    migrations/m20220119t181049_tiny.cpp
    migrations/m20220119t181249_small.h
    migrations/m20220119t181249_small.cpp
    migrations/m20220119t181401_medium.h
    migrations/m20220119t181401_medium.cpp
    migrations/m20220119t181501_big.h
    migrations/m20220119t181501_big.cpp
)

function(firfuorida_test _testname _link1 _link2 _link3)
    add_executable(${_testname}_exec
        ${_testname}.cpp
        testmigrations.h
        testmigrations.cpp
        ${testmigration_SRCS}
    )
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
    target_link_libraries(${_testname}_exec
        PUBLIC
            Qt${QT_VERSION_MAJOR}::Core
            Qt${QT_VERSION_MAJOR}::Sql
            Qt${QT_VERSION_MAJOR}::Test
            FirfuoridaQt${QT_VERSION_MAJOR}
            ${_link1}
            ${_link2}
            ${_link3}
    )
    set_property(TEST ${_testname} PROPERTY ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/Firfuorida")
endfunction(firfuorida_test _testname _link1 _link2 _link3)

firfuorida_test(testmysqlmigrations "" "" "")
firfuorida_test(testsqlitemigrations "" "" "")

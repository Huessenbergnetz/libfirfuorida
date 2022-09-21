# SPDX-FileCopyrightText: (C) 2019-2021 Matthias Fehring / www.huessenbergnetz.de
# SPDX-License-Identifier: LGPL-3.0-or-later

project(firfuorida_tests)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Test REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} 5.6.0 REQUIRED COMPONENTS Test)

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

set(testmigration_SRCS
    migrations/m20220119t181049_tiny.h
    migrations/m20220119t181049_tiny.cpp
    migrations/m20220119t181249_small.h
    migrations/m20220119t181249_small.cpp
    migrations/m20220119t181401_medium.h
    migrations/m20220119t181401_medium.cpp
    migrations/m20220119t181501_big.h
    migrations/m20220119t181501_big.cpp
    migrations/m20220120t145652_tests1.h
    migrations/m20220120t145652_tests1.cpp
    migrations/m20220121t083111_defaults.h
    migrations/m20220121t083111_defaults.cpp
    migrations/m20220129t115726_foreignkey1.h
    migrations/m20220129t115726_foreignkey1.cpp
    migrations/m20220129t115731_foreignkey2.h
    migrations/m20220129t115731_foreignkey2.cpp
    migrations/m20220218t084654_drop_column.h
    migrations/m20220218t084654_drop_column.cpp
)

function(firfuorida_testmigration _testname _link1 _link2 _link3)
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
endfunction(firfuorida_testmigration _testname _link1 _link2 _link3)

firfuorida_test(testerrorobject "" "" "")
firfuorida_testmigration(testmysqlmigrations "" "" "")
firfuorida_testmigration(testsqlitemigrations "" "" "")

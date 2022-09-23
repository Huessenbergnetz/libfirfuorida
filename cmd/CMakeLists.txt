# SPDX-FileCopyrightText: (C) 2019-2022 Matthias Fehring <https://www.huessenbergnetz.de>
# SPDX-License-Identifier: LGPL-3.0-or-later

add_executable(firfuoridacmd
    main.cpp
)

target_link_libraries(firfuoridacmd
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
)

target_compile_options(firfuoridacmd
    PRIVATE
        -Wall
        -Wcast-align
        -Wno-uninitialized
        -Wempty-body
        -Wformat-security
        -Wformat
        -Winit-self
)

target_compile_definitions(firfuoridacmd
    PRIVATE
        QT_NO_KEYWORDS
        QT_NO_CAST_TO_ASCII
        QT_NO_CAST_FROM_ASCII
        QT_STRICT_ITERATORS
        QT_NO_URL_CAST_FROM_STRING
        QT_NO_CAST_FROM_BYTEARRAY
        QT_USE_QSTRINGBUILDER
        $<$<NOT:$<CONFIG:Debug>>:QT_NO_DEBUG_OUTPUT>
)

set_target_properties(firfuoridacmd PROPERTIES
    OUTPUT_NAME firfuorida-qt${QT_VERSION_MAJOR}
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

if (ENABLE_ASAN)
    target_link_libraries(firfuoridacmd
        PRIVATE
            asan
    )
    target_compile_options(firfuoridacmd
        PRIVATE
            -fsanitize=address
            -fno-omit-frame-pointer
            -Wformat
            -Werror=format-security
            -Werror=array-bounds
            -g
            -ggdb
    )
    set_target_properties(firfuoridacmd PROPERTIES
        LINK_FLAGS -fsanitize=address
    )
endif (ENABLE_ASAN)

install(TARGETS firfuoridacmd DESTINATION ${CMAKE_INSTALL_BINDIR})

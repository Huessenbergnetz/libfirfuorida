/*
 * SPDX-FileCopyrightText: (C) 2019-2022 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef FIRFUORIDA_GLOBAL_H
#define FIRFUORIDA_GLOBAL_H

#include <QtGlobal>

// until Qt 5.9, Q_DECLARE_PRIVATE_D misses the qGetPtrHelper
#if (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
#define F_DECLARE_PRIVATE_D(Dptr, Class) Q_DECLARE_PRIVATE_D(Dptr, Class)
#else
#define F_DECLARE_PRIVATE_D(Dptr, Class) \
    inline Class##Private* d_func() { return reinterpret_cast<Class##Private *>(qGetPtrHelper(Dptr)); } \
    inline const Class##Private* d_func() const { return reinterpret_cast<const Class##Private *>(qGetPtrHelper(Dptr)); } \
    friend class Class##Private;
#endif

#endif // FIRFUORIDA_GLOBAL_H

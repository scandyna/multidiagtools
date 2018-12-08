/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_ASSERT_H
#define MDT_ASSERT_H

#include <QtGlobal>

/*! \brief A Q_ASSERT() like macro that works on constexpr function
 *
 * Mdt uses Q_ASSERT() to check preconditions, and will probably used it until
 *  C++ defines tools for contract programming.
 *
 * Currently, Q_ASSERT() cannot be used in constexpr functions.
 *  There is a solution proposed here:
 *  https://git.qt.io/consulting-usa/qtbase-xcb-rendering/commit/8ea27bb1c669e21100a6a042b0378b3346bdf671
 *  This proposal is currently not in Qt (for shure not in 5.5, I guess also not in 5.9).
 *  MDT_ASSERT() is implemented this way.
 *
 * \note MDT_ASSERT() is a temporary solution.
 *   It should only be used in constexpr functions.
 *   For assertions in all other code, Q_ASSERT() should be used.
 *
 * \note MDT_ASSERT() is part of Error_Core library, to avoid adding other dependencies
 *   (Error_Core is used by many Mdt libraries).
 */
#if !defined(MDT_ASSERT)
#  if defined(QT_NO_DEBUG) && !defined(QT_FORCE_ASSERTS)
#    define MDT_ASSERT(cond) static_cast<void>(false && (cond))
#  else
#    define MDT_ASSERT(cond) ((cond) ? static_cast<void>(0) : qt_assert(#cond, __FILE__, __LINE__))
#  endif
#endif // #if !defined(MDT_ASSERT)

#endif // #ifndef MDT_ASSERT_H

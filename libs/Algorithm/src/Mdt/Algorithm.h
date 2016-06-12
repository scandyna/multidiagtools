/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_ALGORITHM_H
#define MDT_ALGORITHM_H

#include <QString>
#include <QChar>

namespace Mdt{

/*! \brief Some helper that could be usefull
 */
namespace Algorithm{

  /*! \brief Get a string with first and last char removed in str if they match c
   */
  QString removeFirstLastCharIf(const QString & str, QChar c);

}} // namespace Mdt{ namespace Algorithm{

#endif // #ifndef MDT_ALGORITHM_H

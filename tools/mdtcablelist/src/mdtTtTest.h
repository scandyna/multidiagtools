/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#ifndef MDT_TT_TEST_H
#define MDT_TT_TEST_H

#include "mdtClBase.h"
#include <QVariant>

/*! \brief Helper class for test edition
 */
class mdtTtTest : public mdtClBase
{
 public:

  /*! \brief Constructor
   */
  mdtTtTest(QSqlDatabase db);

  /*! \brief Add a test item
   */
  bool addTestItem(const QVariant & testId, const QVariant & testLinkBusAId, const QVariant & testLinkBusBId, const QVariant & expectedValue);

 private:

  Q_DISABLE_COPY(mdtTtTest);
};

#endif // #ifndef MDT_TT_TEST_H

/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include <QSqlDatabase>
#include <QVariant>

/*! \brief Helper class to manipulate test result data
 */
class mdtTtTest : public mdtClBase
{
 public:

  /*! \brief Constructor
   */
  mdtTtTest(QSqlDatabase db);

  /*! \brief Set test model
   *
   * Will also add a test item for each test item .
   */
  bool setTestModel(const QVariant & testResultId, const QVariant & testId);

  /*! \brief Add a test item
   */
  bool addItem(const QVariant & testResultId, const QVariant & testItemId);

  /*! \brief Add a test items based on given test ID
   */
  bool addItemsByTestId(const QVariant & testResultId, const QVariant & testId);

 private:

  Q_DISABLE_COPY(mdtTtTest);
};

#endif // #ifndef MDT_TT_TEST_H

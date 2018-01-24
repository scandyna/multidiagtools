/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_TEST_LIB_ITEM_MODEL_INSERT_ROW_TEST_H
#define MDT_TEST_LIB_ITEM_MODEL_INSERT_ROW_TEST_H

#include "MdtTestLib_CoreExport.h"
#include <QObject>
#include <QAbstractItemModel>

namespace Mdt{ namespace TestLib{

  /*! \brief Class to check that inserting rows works on a item model
   *
   * Typical usage:
   * \code
   * MyModel model;
   * Mdt::TestLib::ItemModelInsertRowTest insertRowTest(&model);
   * \endcode
   */
  class MDT_TESTLIB_CORE_EXPORT ItemModelInsertRowTest : public QObject
  {
   Q_OBJECT

   public:

    ItemModelInsertRowTest(QAbstractItemModel *model, QObject *parent = nullptr);

   private:

    void runTests(QAbstractItemModel *model);

    static bool isEditableAt(QAbstractItemModel * model, int row, int column);
  };

}} // namespace Mdt{ namespace TestLib{

#endif // #ifndef MDT_TEST_LIB_ITEM_MODEL_INSERT_ROW_TEST_H

/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#ifndef MDT_DATA_TABLE_TEST_OLD_H
#define MDT_DATA_TABLE_TEST_OLD_H

#include "mdtTest.h"

class mdtDataTableTest : public mdtTest
{
 Q_OBJECT

 private slots:

  void sandbox();

  // Dataset creations test
  void createDataSetTest();

  // Add/edit/remove data test
  void editDataTest();

  // Field map test
  void fieldMapItemCopyTest();
  void fieldMapCopyTest();
  void fieldMapFieldsMappedFilterTest();
  void fieldMapHeaderTest();
  void fieldMapTest();
  void fieldMapAutoMapTest();
  void fieldMapDataTest();

  // CSV export/import tests
  void csvExportTest();
  void csvImportTest();

  // Table copy test
  void copyTableTest();
};

#endif  // #ifndef MDT_DATA_TABLE_TEST_OLD_H
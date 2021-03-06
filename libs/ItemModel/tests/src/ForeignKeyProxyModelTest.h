/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#ifndef MDT_ITEM_MODEL_FOREIGN_KEY_PROXY_MODEL_TEST_H
#define MDT_ITEM_MODEL_FOREIGN_KEY_PROXY_MODEL_TEST_H

#include "TestBase.h"
#include "Mdt/ItemModel/ColumnList.h"
#include <QList>

class ForeignKeyProxyModelTest : public TestBase
{
 Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void mapItemFlagsTest();
  void mapItemTest();
  void mapAddGetTest();
  void mapMostRestrictiveFlagsForColumnTest();
  void mapMostRestrictiveFlagsForColumnBenchmark();
  void mapColumnsPartOfForeignKeyTest();
  void mapGetForeignKeyListTest();
  void addGetTest();
  void setModelTest();
  void flagsTest();
  void recordTest();
  void columnsPartOfForeignKeyTest();
  void getForeignKeyListTest();
  void qtModelTest();
  void modelGetFlagsBenchmark();
  void modelGetFlagsBenchmark_data();
  void foreignKeyModelGetFlagsBenchmark();
  void foreignKeyModelGetFlagsBenchmark_data();
  void modelGetDataBenchmark();
  void modelGetDataBenchmark_data();
  void foreignKeyModelGetDataBenchmark();
  void foreignKeyModelGetDataBenchmark_data();

 private:

  // QTest contains a nice qCompare() for QList (not for QVector)
  static QList<int> toQList(const Mdt::ItemModel::ColumnList & list);
};

#endif // #ifndef MDT_ITEM_MODEL_FOREIGN_KEY_PROXY_MODEL_TEST_H

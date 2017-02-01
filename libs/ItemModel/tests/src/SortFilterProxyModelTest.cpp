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
#include "SortFilterProxyModelTest.h"
#include "Mdt/ItemModel/SortFilterProxyModel.h"
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <QRegExp>
#include <QStringList>
#include <QVariant>

using namespace Mdt::ItemModel;

void SortFilterProxyModelTest::initTestCase()
{
}

void SortFilterProxyModelTest::cleanupTestCase()
{
}

/*
 * Tests
 */

// void SortFilterProxyModelTest::QSortFilterProxyModelFilterInsertRowTest()
// {
//   QStringListModel model;
//   QSortFilterProxyModel proxyModel;
//   /*
//    * Setup proxy model
//    */
//   proxyModel.setSourceModel(&model);
//   proxyModel.setDynamicSortFilter(true);
//   proxyModel.setFilterRegExp("^A");
//   /*
//    * Populate model with initial data
//    * Source model    Proxy model
//    * ------          ------
//    * | A5 |          | A5 |
//    * ------          ------
//    * | B5 |
//    * ------
//    */
//   model.setStringList(QStringList({"A5","B5"}));
//   QCOMPARE(proxyModel.rowCount(), 1);
//   QCOMPARE(getModelData(proxyModel, 0, 0), QVariant("A5"));
//   /*
//    * Insert a row at the begining
//    * Source model    Proxy model
//    * ------          ------
//    * |    |          | A5 |
//    * ------          ------
//    * | A5 |
//    * ------
//    * | B5 |
//    * ------
//    */
//   QVERIFY(proxyModel.insertRow(0));
//   // Check source model
//   QCOMPARE(model.rowCount(), 3);
//   QVERIFY(getModelData(model, 0, 0).isNull());
//   QCOMPARE(getModelData(model, 1, 0), QVariant("A5"));
//   QCOMPARE(getModelData(model, 2, 0), QVariant("B5"));
//   // Check proxy model
//   QCOMPARE(proxyModel.rowCount(), 1);
//   QCOMPARE(getModelData(proxyModel, 0, 0), QVariant("A5"));
//   /*
//    * Edit inserted row
//    * Source model    Proxy model
//    * ------          ------
//    * | A4 |          | A4 |
//    * ------          ------
//    * | A5 |          | A5 |
//    * ------          ------
//    * | B5 |
//    * ------
//    */
//   QVERIFY(setModelData(model, 0, 0, "A4"));
//   // Check source model
//   QCOMPARE(model.rowCount(), 3);
//   QCOMPARE(getModelData(model, 0, 0), QVariant("A4"));
//   QCOMPARE(getModelData(model, 1, 0), QVariant("A5"));
//   QCOMPARE(getModelData(model, 2, 0), QVariant("B5"));
//   // Check proxy model
//   QCOMPARE(proxyModel.rowCount(), 2);
//   QCOMPARE(getModelData(proxyModel, 0, 0), QVariant("A4"));
//   QCOMPARE(getModelData(proxyModel, 1, 0), QVariant("A5"));
//   /*
//    * Insert a row at the end
//    *
//    * Expected:
//    * Source model    Proxy model
//    * ------          ------
//    * | A4 |          | A4 |
//    * ------          ------
//    * | A5 |          | A5 |
//    * ------          ------
//    * | B5 |
//    * ------
//    * |    |
//    * ------
//    *
//    * What QSortFilterProxyModel does:
//    * Source model    Proxy model
//    * ------          ------
//    * | A4 |          | A4 |
//    * ------          ------
//    * | A5 |          | A5 |
//    * ------          ------
//    * |    |
//    * ------
//    * | B5 |
//    * ------
//    */
//   QVERIFY(proxyModel.insertRow(2));
//   // Check source model
//   QCOMPARE(model.rowCount(), 4);
//   QCOMPARE(getModelData(model, 0, 0), QVariant("A4"));
//   QCOMPARE(getModelData(model, 1, 0), QVariant("A5"));
//   QCOMPARE(getModelData(model, 2, 0), QVariant("B5"));
//   QVERIFY(getModelData(model, 3, 0).isNull());
//   // Check proxy model
//   QCOMPARE(proxyModel.rowCount(), 2);
//   QCOMPARE(getModelData(proxyModel, 0, 0), QVariant("A4"));
//   QCOMPARE(getModelData(proxyModel, 1, 0), QVariant("A5"));
//   /*
//    * Edit inserted row
//    * Source model    Proxy model
//    * ------          ------
//    * | A4 |          | A4 |
//    * ------          ------
//    * | A5 |          | A5 |
//    * ------          ------
//    * | B5 |          | A6 |
//    * ------          ------
//    * | A6 |
//    * ------
//    */
//   QVERIFY(setModelData(model, 3, 0, "A6"));
//   // Check source model
//   QCOMPARE(model.rowCount(), 4);
//   QCOMPARE(getModelData(model, 0, 0), QVariant("A4"));
//   QCOMPARE(getModelData(model, 1, 0), QVariant("A5"));
//   QCOMPARE(getModelData(model, 2, 0), QVariant("B5"));
//   QCOMPARE(getModelData(model, 3, 0), QVariant("A6"));
//   QVERIFY(getModelData(model, 3, 0).isNull());
//   // Check proxy model
//   QCOMPARE(proxyModel.rowCount(), 3);
//   QCOMPARE(getModelData(proxyModel, 0, 0), QVariant("A4"));
//   QCOMPARE(getModelData(proxyModel, 1, 0), QVariant("A5"));
//   QCOMPARE(getModelData(proxyModel, 2, 0), QVariant("A6"));
// }

void SortFilterProxyModelTest::QSortFilterProxyModelFilterInsertRowTest()
{
  QFETCH(QStringList, initialModelList);
  QFETCH(int, row);
  QFETCH(QStringList, expectedModelList);
  QFETCH(QStringList, expectedProxyModelList);
  QStringListModel model;
  QSortFilterProxyModel proxyModel;
  /*
   * Setup models
   */
  model.setStringList(initialModelList);
  proxyModel.setSourceModel(&model);
  proxyModel.setDynamicSortFilter(true);
  proxyModel.setFilterRegExp("^A");
  /*
   * Insert and check
   */
  QVERIFY(proxyModel.insertRow(row));
  // Check source model
  QCOMPARE(model.stringList(), expectedModelList);
  // Check proxy model
  QCOMPARE(proxyModel.rowCount(), expectedProxyModelList.count());
  for(int r = 0; r < proxyModel.rowCount(); ++r){
    QModelIndex index = proxyModel.index(r, 0);
    QVERIFY(index.isValid());
    QCOMPARE(proxyModel.data(index).toString(), expectedProxyModelList.at(r));
  }
}

void SortFilterProxyModelTest::QSortFilterProxyModelFilterInsertRowTest_data()
{
  setupInsertRowTestData();
}

// void SortFilterProxyModelTest::filterInsertRowTest()
// {
//   QStringListModel model;
//   SortFilterProxyModel proxyModel;
//   /*
//    * Setup proxy model
//    */
//   proxyModel.setSourceModel(&model);
//   proxyModel.setDynamicSortFilter(true);
//   proxyModel.setFilterRegExp("^A");
//   /*
//    * Populate model with initial data
//    * Source model    Proxy model
//    * ------          ------
//    * | A5 |          | A5 |
//    * ------          ------
//    * | B5 |
//    * ------
//    */
//   model.setStringList(QStringList({"A5","B5"}));
//   QCOMPARE(proxyModel.rowCount(), 1);
//   QCOMPARE(getModelData(proxyModel, 0, 0), QVariant("A5"));
//   /*
//    * Insert a row at the begining
//    * Source model    Proxy model
//    * ------          ------
//    * |    |          | A5 |
//    * ------          ------
//    * | A5 |
//    * ------
//    * | B5 |
//    * ------
//    */
//   QVERIFY(proxyModel.insertRow(0));
//   // Check source model
//   QCOMPARE(model.rowCount(), 3);
//   QVERIFY(getModelData(model, 0, 0).isNull());
//   QCOMPARE(getModelData(model, 1, 0), QVariant("A5"));
//   QCOMPARE(getModelData(model, 2, 0), QVariant("B5"));
//   // Check proxy model
//   QCOMPARE(proxyModel.rowCount(), 1);
//   QCOMPARE(getModelData(proxyModel, 0, 0), QVariant("A5"));
//   /*
//    * Edit inserted row
//    * Source model    Proxy model
//    * ------          ------
//    * | A4 |          | A4 |
//    * ------          ------
//    * | A5 |          | A5 |
//    * ------          ------
//    * | B5 |
//    * ------
//    */
//   QVERIFY(setModelData(model, 0, 0, "A4"));
//   // Check source model
//   QCOMPARE(model.rowCount(), 3);
//   QCOMPARE(getModelData(model, 0, 0), QVariant("A4"));
//   QCOMPARE(getModelData(model, 1, 0), QVariant("A5"));
//   QCOMPARE(getModelData(model, 2, 0), QVariant("B5"));
//   // Check proxy model
//   QCOMPARE(proxyModel.rowCount(), 2);
//   QCOMPARE(getModelData(proxyModel, 0, 0), QVariant("A4"));
//   QCOMPARE(getModelData(proxyModel, 1, 0), QVariant("A5"));
//   /*
//    * Insert a row at the end
//    * Source model    Proxy model
//    * ------          ------
//    * | A4 |          | A4 |
//    * ------          ------
//    * | A5 |          | A5 |
//    * ------          ------
//    * | B5 |
//    * ------
//    * |    |
//    * ------
//    */
//   QVERIFY(proxyModel.insertRow(2));
//   // Check source model
//   QCOMPARE(model.rowCount(), 4);
//   QCOMPARE(getModelData(model, 0, 0), QVariant("A4"));
//   QCOMPARE(getModelData(model, 1, 0), QVariant("A5"));
//   QCOMPARE(getModelData(model, 2, 0), QVariant("B5"));
//   QVERIFY(getModelData(model, 3, 0).isNull());
//   // Check proxy model
//   QCOMPARE(proxyModel.rowCount(), 2);
//   QCOMPARE(getModelData(proxyModel, 0, 0), QVariant("A4"));
//   QCOMPARE(getModelData(proxyModel, 1, 0), QVariant("A5"));
//   /*
//    * Edit inserted row
//    * Source model    Proxy model
//    * ------          ------
//    * | A4 |          | A4 |
//    * ------          ------
//    * | A5 |          | A5 |
//    * ------          ------
//    * | B5 |          | A6 |
//    * ------          ------
//    * | A6 |
//    * ------
//    */
//   QVERIFY(setModelData(model, 3, 0, "A6"));
//   // Check source model
//   QCOMPARE(model.rowCount(), 4);
//   QCOMPARE(getModelData(model, 0, 0), QVariant("A4"));
//   QCOMPARE(getModelData(model, 1, 0), QVariant("A5"));
//   QCOMPARE(getModelData(model, 2, 0), QVariant("B5"));
//   QCOMPARE(getModelData(model, 3, 0), QVariant("A6"));
//   QVERIFY(getModelData(model, 3, 0).isNull());
//   // Check proxy model
//   QCOMPARE(proxyModel.rowCount(), 3);
//   QCOMPARE(getModelData(proxyModel, 0, 0), QVariant("A4"));
//   QCOMPARE(getModelData(proxyModel, 1, 0), QVariant("A5"));
//   QCOMPARE(getModelData(proxyModel, 2, 0), QVariant("A6"));
// }

void SortFilterProxyModelTest::filterInsertRowTest()
{
  QFETCH(QStringList, initialModelList);
  QFETCH(int, row);
  QFETCH(QStringList, expectedModelList);
  QFETCH(QStringList, expectedProxyModelList);
  QStringListModel model;
  SortFilterProxyModel proxyModel;
  /*
   * Setup models
   */
  model.setStringList(initialModelList);
  proxyModel.setSourceModel(&model);
  proxyModel.setDynamicSortFilter(true);
  proxyModel.setFilterRegExp("^A");
  /*
   * Insert and check
   */
  QVERIFY(proxyModel.insertRow(row));
  // Check source model
  QCOMPARE(model.stringList(), expectedModelList);
  // Check proxy model
  QCOMPARE(proxyModel.rowCount(), expectedProxyModelList.count());
  for(int r = 0; r < proxyModel.rowCount(); ++r){
    QModelIndex index = proxyModel.index(r, 0);
    QVERIFY(index.isValid());
    QCOMPARE(proxyModel.data(index).toString(), expectedProxyModelList.at(r));
  }
}

void SortFilterProxyModelTest::filterInsertRowTest_data()
{
  setupInsertRowTestData();
}

/*
 * Helper functions
 */

void SortFilterProxyModelTest::setupInsertRowTestData()
{
  QTest::addColumn<QStringList>("initialModelList");
  QTest::addColumn<int>("row");
  QTest::addColumn<QStringList>("expectedModelList");
  QTest::addColumn<QStringList>("expectedProxyModelList");
  /*
   * Insert a row at the begining
   * Source model    Proxy model
   * ------          ------
   * |    |          | A5 |
   * ------          ------
   * | A5 |
   * ------
   * | B5 |
   * ------
   */
  QTest::newRow("at begining") << (QStringList() << "A5" << "B5")
                    << 0
                    << (QStringList() << "" << "A5" << "B5")
                    << (QStringList() << "A5");
  /*
   * Insert a row at end
   * Source model    Proxy model
   * ------          ------
   * | B4 |          | A5 |
   * ------          ------
   * | A5 |
   * ------
   * |    |
   * ------
   *
   * Note on context:
   * Immagine we want to append a row into proxy model
   * This new row will contain data "A6".
   * We can expect this:
   * Source model    Proxy model
   * ------          ------
   * | B4 |          | A5 |
   * ------          ------
   * | A5 |          | A6 |
   * ------          ------
   * | A6 |
   * ------
   * But not this:
   * Source model    Proxy model
   * ------          ------
   * | B4 |          | A6 |
   * ------          ------
   * | A6 |          | A5 |
   * ------          ------
   * | A5 |
   * ------
   */
  QTest::newRow("at end") << (QStringList() << "B4" << "A5")
                    << 1
                    << (QStringList() << "B4" << "A5" << "")
                    << (QStringList() << "A5");
}


/*
 * Main
 */
MDT_ITEM_MODEL_TEST_MAIN(SortFilterProxyModelTest)
// MDT_ITEM_MODEL_TEST_MAIN_DEBUG_ENV(SortFilterProxyModelTest)

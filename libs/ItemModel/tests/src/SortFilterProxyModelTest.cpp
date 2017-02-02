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

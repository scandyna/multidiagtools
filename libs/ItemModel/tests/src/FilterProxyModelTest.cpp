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
#include "FilterProxyModelTest.h"
#include "Mdt/Application.h"
#include "Mdt/ItemModel/FilterProxyModel.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include <QModelIndex>
#include <QTableView>
#include <QListView>
#include <QTreeView>
#include <QComboBox>

namespace ItemModel = Mdt::ItemModel;
using ItemModel::VariantTableModel;
using ItemModel::FilterProxyModel;
using ItemModel::FilterColumn;
using Like = ItemModel::LikeExpression;

void FilterProxyModelTest::initTestCase()
{
}

void FilterProxyModelTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void FilterProxyModelTest::filterTest()
{
  QModelIndex index;
  VariantTableModel model;
  FilterProxyModel proxyModel;
  QTableView tableView;
  QTreeView treeView;
  QListView listView;
  QComboBox combobox;
  /*
   * Setup models
   */
  proxyModel.setSourceModel(&model);
  /*
   * Setup views
   */
  tableView.setModel(&proxyModel);
  treeView.setModel(&proxyModel);
  listView.setModel(&proxyModel);
  combobox.setModel(&proxyModel);
  tableView.show();
  treeView.show();
  listView.show();
  combobox.show();
  /*
   * Tests
   */
  FilterColumn id(0);
  FilterColumn name(1);
  // Initial state
  QCOMPARE(proxyModel.rowCount(), 0);
  QCOMPARE(proxyModel.columnCount(), 0);
  // Populate
  populateModel(model);
  QCOMPARE(proxyModel.rowCount(), 3);
  QCOMPARE(proxyModel.columnCount(), 2);
  /*
   * Simple filter
   */
  proxyModel.setFilter(id == 1);
  QCOMPARE(proxyModel.rowCount(), 1);
  QCOMPARE(proxyModel.columnCount(), 2);
  // Check row 0 , id
  index = proxyModel.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(proxyModel.data(index), QVariant(1));
  // Check row 0 , name
  index = proxyModel.index(0, 1);
  QVERIFY(index.isValid());
  QCOMPARE(proxyModel.data(index), QVariant("ABC"));
  /*
   * Case sensitivity
   */
  proxyModel.setFilterCaseSensitivity(Qt::CaseSensitive);
  proxyModel.setFilter(name == "abc");
  QCOMPARE(proxyModel.rowCount(), 0);
  proxyModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
  proxyModel.setFilter(name == "abc");
  QCOMPARE(proxyModel.rowCount(), 1);
  index = proxyModel.index(0, 1);
  QVERIFY(index.isValid());
  QCOMPARE(proxyModel.data(index), QVariant("ABC"));
  /*
   * 2 columns filter
   */
  proxyModel.setFilter(id == 1 || name == "DEF");
  QCOMPARE(proxyModel.rowCount(), 2);
  QCOMPARE(proxyModel.columnCount(), 2);
  // Check row 0 , id
  index = proxyModel.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(proxyModel.data(index), QVariant(1));
  // Check row 0 , name
  index = proxyModel.index(0, 1);
  QVERIFY(index.isValid());
  QCOMPARE(proxyModel.data(index), QVariant("ABC"));
  // Check row 1 , id
  index = proxyModel.index(1, 0);
  QVERIFY(index.isValid());
  QCOMPARE(proxyModel.data(index), QVariant(2));
  // Check row 1 , name
  index = proxyModel.index(1, 1);
  QVERIFY(index.isValid());
  QCOMPARE(proxyModel.data(index), QVariant("DEF"));

  /*
   * Play
   */
//   while(tableView.isVisible()){
//     QTest::qWait(500);
//   }
}

void FilterProxyModelTest::filterBenchmark()
{
  QFETCH(int, N);
  QModelIndex index;
  VariantTableModel model;
  FilterProxyModel proxyModel;
  FilterColumn A(0);
  FilterColumn B(1);
  /*
   * Setup models
   */
  QVERIFY(N >= 10);
  model.populate(N, 3);
  proxyModel.setSourceModel(&model);
  proxyModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
  QCOMPARE(proxyModel.rowCount(), N);
  QCOMPARE(proxyModel.columnCount(), 3);
  QBENCHMARK{
    proxyModel.setFilter(A != "8A" && B == "10B");
  }
  QCOMPARE(proxyModel.rowCount(), 1);
  index = proxyModel.index(0, 1);
  QVERIFY(index.isValid());
  QCOMPARE(proxyModel.data(index), QVariant("10B"));
}

void FilterProxyModelTest::filterBenchmark_data()
{
  QTest::addColumn<int>("N");

  QTest::newRow("20 el.") << 20;
  QTest::newRow("100 el.") << 100;
  QTest::newRow("1'000 el.") << 1000;
  QTest::newRow("10'000 el.") << 10000;
}

void FilterProxyModelTest::filterLikeBenchmark()
{
  QFETCH(int, N);
  QModelIndex index;
  VariantTableModel model;
  FilterProxyModel proxyModel;
  FilterColumn A(0);
  FilterColumn B(1);
  /*
   * Setup models
   */
  QVERIFY(N >= 20);
  model.populate(N, 3);
  proxyModel.setSourceModel(&model);
  proxyModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
  QCOMPARE(proxyModel.rowCount(), N);
  QCOMPARE(proxyModel.columnCount(), 3);
  QBENCHMARK{
    proxyModel.setFilter(A == Like("1?A") && B == Like("1?B"));
  }
  QCOMPARE(proxyModel.rowCount(), 10);
  index = proxyModel.index(0, 0);
  QVERIFY(index.isValid());
  QCOMPARE(proxyModel.data(index), QVariant("10A"));
  index = proxyModel.index(9, 0);
  QVERIFY(index.isValid());
  QCOMPARE(proxyModel.data(index), QVariant("19A"));
}

void FilterProxyModelTest::filterLikeBenchmark_data()
{
  QTest::addColumn<int>("N");

  QTest::newRow("20 el.") << 20;
  QTest::newRow("100 el.") << 100;
  QTest::newRow("1'000 el.") << 1000;
  QTest::newRow("10'000 el.") << 10000;
}

void FilterProxyModelTest::populateModel(VariantTableModel & model)
{
  QModelIndex index;
  /*
   * Populate table model:
   * ----------------------------
   * | Id | Name |
   * ----------------------------
   * | 1  | ABC  |
   * ----------------------------
   * | 2  | DEF  |
   * ----------------------------
   * | 3  | GHI  |
   * ----------------------------
   */
  model.populate(3,2);
  /*
   * Row 0
   */
  // Set Id
  index = model.index(0, 0);
  QVERIFY(index.isValid());
  model.setData(index, 1);
  // Set Name
  index = model.index(0, 1);
  QVERIFY(index.isValid());
  model.setData(index, "ABC");
  /*
   * Row 1
   */
  // Set Id
  index = model.index(1, 0);
  QVERIFY(index.isValid());
  model.setData(index, 2);
  // Set Name
  index = model.index(1, 1);
  QVERIFY(index.isValid());
  model.setData(index, "DEF");
  /*
   * Row 2
   */
  // Set Id
  index = model.index(2, 0);
  QVERIFY(index.isValid());
  model.setData(index, 3);
  // Set Name
  index = model.index(2, 1);
  QVERIFY(index.isValid());
  model.setData(index, "GHI");
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  FilterProxyModelTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}

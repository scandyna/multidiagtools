/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
 ** along with Mdt. If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "HeaderProxyModelTest.h"
#include "qtmodeltest.h"
#include "Mdt/ItemModel/HeaderProxyModelItem.h"
#include "Mdt/ItemModel/HeaderProxyModelItemList.h"
#include "Mdt/ItemModel/HeaderProxyModel.h"
#include "Mdt/ItemModel/VariantTableModel.h"

using namespace Mdt::ItemModel;

void HeaderProxyModelTest::initTestCase()
{
}

void HeaderProxyModelTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void HeaderProxyModelTest::itemTest()
{
  HeaderProxyModelItem item;
  item.setLabel("A");
  QCOMPARE(item.label(), QString("A"));
}

void HeaderProxyModelTest::itemListTest()
{
  /*
   * Initial state
   */
  HeaderProxyModelItemList list;
  QCOMPARE(list.count(), 0);
  /*
   * Set list of labels
   */
  list.setLabels({"A","B"});
  QCOMPARE(list.count(), 2);
  QCOMPARE(list.labelAt(0), QString("A"));
  QCOMPARE(list.labelAt(1), QString("B"));
  list.setLabels({"1"});
  QCOMPARE(list.count(), 1);
  QCOMPARE(list.labelAt(0), QString("1"));
  list.setLabels({"A","B","C"});
  QCOMPARE(list.count(), 3);
  QCOMPARE(list.labelAt(0), QString("A"));
  QCOMPARE(list.labelAt(1), QString("B"));
  QCOMPARE(list.labelAt(2), QString("C"));
  /*
   * Clear
   */
  list.clear();
  QCOMPARE(list.count(), 0);
  /*
   * Set labels individually
   */
  list.setLabelAt(0, "A");
  QCOMPARE(list.count(), 1);
  QCOMPARE(list.labelAt(0), QString("A"));
  list.setLabelAt(2, "C");
  QCOMPARE(list.count(), 3);
  QCOMPARE(list.labelAt(0), QString("A"));
  QVERIFY(list.labelAt(1).isEmpty());
  QCOMPARE(list.labelAt(2), QString("C"));
}

void HeaderProxyModelTest::dimensionTest()
{
  VariantTableModel model;
  HeaderProxyModel headerModel;
  headerModel.setSourceModel(&model);

  QCOMPARE(headerModel.rowCount(), 0);
  QCOMPARE(headerModel.columnCount(), 0);
  model.resize(3, 2);
  QCOMPARE(headerModel.rowCount(), 3);
  QCOMPARE(headerModel.columnCount(), 2);
}

void HeaderProxyModelTest::horizontalLabelsTest()
{
  /*
   * Initial state
   */
  HeaderProxyModel headerModel;
  QCOMPARE(headerModel.rowCount(), 0);
  QCOMPARE(headerModel.columnCount(), 0);
  QVERIFY(headerModel.headerData(-1, Qt::Horizontal, Qt::DisplayRole).isNull());
  QVERIFY(headerModel.headerData(0, Qt::Horizontal, Qt::DisplayRole).isNull());
  /*
   * Set header labels
   */
  headerModel.setHorizontalHeaderLabels({"A","B"});
  QCOMPARE(headerModel.headerData(0, Qt::Horizontal, Qt::DisplayRole), QVariant("A"));
  QCOMPARE(headerModel.headerData(1, Qt::Horizontal, Qt::DisplayRole), QVariant("B"));
  /*
   * Set source model
   */
  VariantTableModel model;
  headerModel.setSourceModel(&model);
  model.resize(2, 3);
  QCOMPARE(headerModel.headerData(0, Qt::Horizontal, Qt::DisplayRole), QVariant("A"));
  QCOMPARE(headerModel.headerData(1, Qt::Horizontal, Qt::DisplayRole), QVariant("B"));
  QCOMPARE(headerModel.headerData(2, Qt::Horizontal, Qt::DisplayRole), model.headerData(2, Qt::Horizontal, Qt::DisplayRole));
  // Check also that vertical headers are not affected
  QCOMPARE(headerModel.headerData(0, Qt::Vertical, Qt::DisplayRole), model.headerData(0, Qt::Vertical, Qt::DisplayRole));
  QCOMPARE(headerModel.headerData(1, Qt::Vertical, Qt::DisplayRole), model.headerData(1, Qt::Vertical, Qt::DisplayRole));
}

void HeaderProxyModelTest::qtModelTest()
{
  /*
   * Setup model
   */
  VariantTableModel model;
  model.resize(3, 2);
  model.populateColumn(0, {1,2,3});
  model.populateColumn(1, {"A","B","C"});
  /*
   * Setup proxy model
   */
  HeaderProxyModel headerModel;
  headerModel.setSourceModel(&model);
  /*
   * Test
   */
  QCOMPARE(headerModel.rowCount(), 3);
  QtModelTest mt(&headerModel);
  QtModelTest smt(&model);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  HeaderProxyModelTest test;

  return QTest::qExec(&test, argc, argv);
}

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
#include "ForeignKeyProxyModelTest.h"
#include "qtmodeltest.h"
#include "Mdt/ItemModel/ForeignKeyProxyModelMapItem.h"
#include "Mdt/ItemModel/ForeignKeyProxyModelMap.h"
#include "Mdt/ItemModel/ForeignKeyProxyModel.h"
#include "Mdt/ItemModel/VariantTableModel.h"

using namespace Mdt::ItemModel;

void ForeignKeyProxyModelTest::initTestCase()
{
}

void ForeignKeyProxyModelTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void ForeignKeyProxyModelTest::mapItemFlagsTest()
{
  /*
   * Initial state
   */
  ForeignKeyProxyModelMapItemFlags flags;
  QVERIFY(flags.isForeignKeyEditable());
  QVERIFY(flags.isForeignKeyItemsEnabled());
  /*
   * Set
   */
  flags.setForeignKeyEditable(false);
  QVERIFY(!flags.isForeignKeyEditable());
  QVERIFY(flags.isForeignKeyItemsEnabled());
  flags.setForeignKeyItemsEnabled(false);
  QVERIFY(!flags.isForeignKeyEditable());
  QVERIFY(!flags.isForeignKeyItemsEnabled());
}

void ForeignKeyProxyModelTest::mapItemTest()
{
  /*
   * Default constructed
   */
  ForeignKeyProxyModelMapItem item0;
  QVERIFY(item0.foreignKey().isNull());
  QVERIFY(item0.isForeignKeyEditable());
  QVERIFY(item0.isForeignKeyItemsEnabled());
  /*
   * Contruct
   */
  ForeignKey fk{1,2};
  ForeignKeyProxyModelMapItem item1(fk);
  QVERIFY(!item1.foreignKey().isNull());
  QVERIFY(item1.isForeignKeyEditable());
  QVERIFY(item1.isForeignKeyItemsEnabled());
  /*
   * Set
   */
  item1.setForeignKeyEditable(false);
  QVERIFY(!item1.isForeignKeyEditable());
  QVERIFY(item1.isForeignKeyItemsEnabled());
  item1.setForeignKeyItemsEnabled(false);
  QVERIFY(!item1.isForeignKeyEditable());
  QVERIFY(!item1.isForeignKeyItemsEnabled());
}

void ForeignKeyProxyModelTest::mapAddGetTest()
{
  /*
   * Initial state
   */
  ForeignKeyProxyModelMap map;
  QVERIFY(!map.hasForeignKeyReferencing("FK1"));
  /*
   * Add foreign key
   */
  ForeignKey fk1{1};
  map.addForeignKey("FK1", fk1);
  QVERIFY(map.hasForeignKeyReferencing("FK1"));
  QCOMPARE(map.getForeignKeyReferencing("FK1").columnCount(), 1);
  ForeignKey fk12{1,2};
  map.addForeignKey("FK12", fk12);
  QCOMPARE(map.getForeignKeyReferencing("FK12").columnCount(), 2);
  /*
   * Change flags
   */
  // Check default flags
  QVERIFY(map.isForeignKeyEditable("FK1"));
  QVERIFY(map.isForeignKeyItemsEnabled("FK1"));
  QVERIFY(map.isForeignKeyEditable("FK12"));
  QVERIFY(map.isForeignKeyItemsEnabled("FK12"));
  // Change editable flags for FK1
  map.setForeignKeyEditable("FK1", false);
  QVERIFY(!map.isForeignKeyEditable("FK1"));
  QVERIFY(map.isForeignKeyItemsEnabled("FK1"));
  QVERIFY(map.isForeignKeyEditable("FK12"));
  QVERIFY(map.isForeignKeyItemsEnabled("FK12"));
  // Change items enabled flag for FK12
  map.setForeignKeyItemsEnabled("FK12", false);
  QVERIFY(!map.isForeignKeyEditable("FK1"));
  QVERIFY(map.isForeignKeyItemsEnabled("FK1"));
  QVERIFY(map.isForeignKeyEditable("FK12"));
  QVERIFY(!map.isForeignKeyItemsEnabled("FK12"));
  // Change editable flag for all FK
  map.setAllForeignKeysEditable(false);
  QVERIFY(!map.isForeignKeyEditable("FK1"));
  QVERIFY(map.isForeignKeyItemsEnabled("FK1"));
  QVERIFY(!map.isForeignKeyEditable("FK12"));
  QVERIFY(!map.isForeignKeyItemsEnabled("FK12"));
  // Change items enabled flag for all FK
  map.setAllForeignKeysItemsEnabled(false);
  QVERIFY(!map.isForeignKeyEditable("FK1"));
  QVERIFY(!map.isForeignKeyItemsEnabled("FK1"));
  QVERIFY(!map.isForeignKeyEditable("FK12"));
  QVERIFY(!map.isForeignKeyItemsEnabled("FK12"));
  /*
   * Remove a foreign key
   */
  QVERIFY(!map.getForeignKeyReferencing("FK1").isNull());
  QVERIFY(!map.getForeignKeyReferencing("FK12").isNull());
  map.removeForeignKey("FK1");
  QVERIFY(map.getForeignKeyReferencing("FK1").isNull());
  QVERIFY(!map.getForeignKeyReferencing("FK12").isNull());
  map.removeForeignKey("FK12");
  QVERIFY(map.getForeignKeyReferencing("FK1").isNull());
  QVERIFY(map.getForeignKeyReferencing("FK12").isNull());
}

void ForeignKeyProxyModelTest::mapMostRestrictiveFlagsForColumnTest()
{
  ForeignKeyProxyModelMapItemFlags flags;
  ForeignKey fk1{1};
  ForeignKey fk12{1,2};
  ForeignKey fk2{2};
  /*
   * Setup A:
   * Non overlapping foreign keys
   */
  ForeignKeyProxyModelMap mapA;
  mapA.addForeignKey("FE1", fk1);
  mapA.addForeignKey("FE2", fk2);
  // Default
  flags = mapA.getMostRestrictiveFlagsForColumn(1);
  QVERIFY(flags.isForeignKeyItemsEnabled());
  QVERIFY(flags.isForeignKeyEditable());
  flags = mapA.getMostRestrictiveFlagsForColumn(2);
  QVERIFY(flags.isForeignKeyItemsEnabled());
  QVERIFY(flags.isForeignKeyEditable());
  // Diable editable for FE1
  mapA.setForeignKeyEditable("FE1", false);
  flags = mapA.getMostRestrictiveFlagsForColumn(1);
  QVERIFY(flags.isForeignKeyItemsEnabled());
  QVERIFY(!flags.isForeignKeyEditable());
  flags = mapA.getMostRestrictiveFlagsForColumn(2);
  QVERIFY(flags.isForeignKeyItemsEnabled());
  QVERIFY(flags.isForeignKeyEditable());
  // Disable items enabled for FE1
  mapA.setForeignKeyItemsEnabled("FE1", false);
  flags = mapA.getMostRestrictiveFlagsForColumn(1);
  QVERIFY(!flags.isForeignKeyItemsEnabled());
  QVERIFY(!flags.isForeignKeyEditable());
  flags = mapA.getMostRestrictiveFlagsForColumn(2);
  QVERIFY(flags.isForeignKeyItemsEnabled());
  QVERIFY(flags.isForeignKeyEditable());
  /*
   * Setup B:
   * ------------------------------
   * | Column             | 1 | 2 |
   * ------------------------------
   * | FK1 items enabled  | y | - |
   * ------------------------------
   * | FK1 editable       | y | - |
   * ------------------------------
   * | FK12 items enabled | y | y |
   * ------------------------------
   * | FK12 editable      | n | n |
   * ------------------------------
   * | Res. items enabled | y | y |
   * ------------------------------
   * | Res. editable      | n | n |
   * ------------------------------
   */
  ForeignKeyProxyModelMap mapB;
  mapB.addForeignKey("FK1", fk1);
  mapB.addForeignKey("FK12", fk12);
  mapB.setForeignKeyEditable("FK12", false);
  // Check
  flags = mapB.getMostRestrictiveFlagsForColumn(1);
  QVERIFY(flags.isForeignKeyItemsEnabled());
  QVERIFY(!flags.isForeignKeyEditable());
  flags = mapB.getMostRestrictiveFlagsForColumn(2);
  QVERIFY(flags.isForeignKeyItemsEnabled());
  QVERIFY(!flags.isForeignKeyEditable());
}

void ForeignKeyProxyModelTest::mapMostRestrictiveFlagsForColumnBenchmark()
{
  ForeignKeyProxyModelMapItemFlags flags0, flags1;
  ForeignKey fk1{1};
  ForeignKey fk12{1,2};
  ForeignKeyProxyModelMap map;
  map.addForeignKey("FK1", fk1);
  map.addForeignKey("FK12", fk12);
  map.setForeignKeyEditable("FK12", false);
  QBENCHMARK{
    flags0 = map.getMostRestrictiveFlagsForColumn(1);
    flags1 = map.getMostRestrictiveFlagsForColumn(2);
  }
  QVERIFY(flags0.isForeignKeyItemsEnabled());
  QVERIFY(!flags0.isForeignKeyEditable());
  QVERIFY(flags1.isForeignKeyItemsEnabled());
  QVERIFY(!flags1.isForeignKeyEditable());
}

void ForeignKeyProxyModelTest::addGetTest()
{
  ForeignKeyProxyModel proxyModel;
  proxyModel.addForeignKey("A", {2,3});
  QCOMPARE(proxyModel.getForeignKeyReferencing("A").columnCount(), 2);
}

void ForeignKeyProxyModelTest::setModelTest()
{
  /*
   * Check that we can do setup before setting source model
   */
  ForeignKeyProxyModel proxyModel;
  proxyModel.addForeignKey("A", {0,1});
  QCOMPARE(proxyModel.rowCount(), 0);
  /*
   * Set source model
   */
  VariantTableModel model;
  model.resize(2, 3);
  proxyModel.setSourceModel(&model);
  QCOMPARE(proxyModel.rowCount(), 2);
}

void ForeignKeyProxyModelTest::flagsTest()
{
  Qt::ItemFlags expectedFlags;
  /*
   * Setup source model
   */
  VariantTableModel model;
  model.resize(3, 5);
  model.populateColumn(0, {1,2});
  model.populateColumn(1, {"A","B"});
  /*
   * Default state
   */
  ForeignKeyProxyModel proxyModel;
  proxyModel.setSourceModel(&model);
  proxyModel.addForeignKey("FE1", {1});
  proxyModel.addForeignKey("FE23", {2,3});
  // Check flags for FE1
  QVERIFY(proxyModel.isForeignKeyEditable("FE1"));
  QVERIFY(proxyModel.isForeignKeyItemsEnabled("FE1"));
  QCOMPARE(getModelFlags(proxyModel, 0, 1), getModelFlags(model, 0, 1));
  // Check flags for FE23
  QVERIFY(proxyModel.isForeignKeyEditable("FE23"));
  QVERIFY(proxyModel.isForeignKeyItemsEnabled("FE23"));
  QCOMPARE(getModelFlags(proxyModel, 0, 2), getModelFlags(model, 0, 2));
  QCOMPARE(getModelFlags(proxyModel, 0, 3), getModelFlags(model, 0, 3));
  /*
   * Disable editable flags for FE1
   */
  proxyModel.setForeignKeyEditable("FE1", false);
  proxyModel.setForeignKeyItemsEnabled("FE1", true);
  // Check flags for FE1
  QVERIFY(!proxyModel.isForeignKeyEditable("FE1"));
  QVERIFY( proxyModel.isForeignKeyItemsEnabled("FE1"));
  expectedFlags = getModelFlags(model, 0, 1) & Qt::ItemFlags(~Qt::ItemIsEditable);
  QCOMPARE(getModelFlags(proxyModel, 0, 1), expectedFlags);
  // Check flags for FE23
  QVERIFY(proxyModel.isForeignKeyEditable("FE23"));
  QVERIFY(proxyModel.isForeignKeyItemsEnabled("FE23"));
  QCOMPARE(getModelFlags(proxyModel, 0, 2), getModelFlags(model, 0, 2));
  QCOMPARE(getModelFlags(proxyModel, 0, 3), getModelFlags(model, 0, 3));
  /*
   * Disable items enable flags for FE1
   */
  proxyModel.setForeignKeyEditable("FE1", true);
  proxyModel.setForeignKeyItemsEnabled("FE1", false);
  // Check flags for FE1
  QVERIFY( proxyModel.isForeignKeyEditable("FE1"));
  QVERIFY(!proxyModel.isForeignKeyItemsEnabled("FE1"));
  expectedFlags = getModelFlags(model, 0, 1) & Qt::ItemFlags(~Qt::ItemIsEnabled);
  QCOMPARE(getModelFlags(proxyModel, 0, 1), expectedFlags);
  // Check flags for FE23
  QVERIFY(proxyModel.isForeignKeyEditable("FE23"));
  QVERIFY(proxyModel.isForeignKeyItemsEnabled("FE23"));
  QCOMPARE(getModelFlags(proxyModel, 0, 2), getModelFlags(model, 0, 2));
  QCOMPARE(getModelFlags(proxyModel, 0, 3), getModelFlags(model, 0, 3));
  /*
   * Disable editable + enabled flags for FE1
   */
  proxyModel.setForeignKeyEditable("FE1", false);
  proxyModel.setForeignKeyItemsEnabled("FE1", false);
  // Check flags for FE1
  QVERIFY(!proxyModel.isForeignKeyEditable("FE1"));
  QVERIFY(!proxyModel.isForeignKeyItemsEnabled("FE1"));
  expectedFlags = getModelFlags(model, 0, 1) & Qt::ItemFlags(~Qt::ItemIsEditable) & Qt::ItemFlags(~Qt::ItemIsEnabled);
  QCOMPARE(getModelFlags(proxyModel, 0, 1), expectedFlags);
  // Check flags for FE23
  QVERIFY(proxyModel.isForeignKeyEditable("FE23"));
  QVERIFY(proxyModel.isForeignKeyItemsEnabled("FE23"));
  QCOMPARE(getModelFlags(proxyModel, 0, 2), getModelFlags(model, 0, 2));
  QCOMPARE(getModelFlags(proxyModel, 0, 3), getModelFlags(model, 0, 3));
  /*
   * Disable editable + enabled flags for all foreign keys
   */
  proxyModel.setAllForeignKeysEditable(false);
  proxyModel.setAllForeignKeysItemsEnabled(false);
  QVERIFY(!proxyModel.isForeignKeyEditable("FE1"));
  QVERIFY(!proxyModel.isForeignKeyItemsEnabled("FE1"));
  QVERIFY(!proxyModel.isForeignKeyEditable("FE23"));
  QVERIFY(!proxyModel.isForeignKeyItemsEnabled("FE23"));
  /*
   * Enable editable flag for all foreign keys
   */
  proxyModel.setAllForeignKeysEditable(true);
  proxyModel.setAllForeignKeysItemsEnabled(false);
  QVERIFY( proxyModel.isForeignKeyEditable("FE1"));
  QVERIFY(!proxyModel.isForeignKeyItemsEnabled("FE1"));
  QVERIFY( proxyModel.isForeignKeyEditable("FE23"));
  QVERIFY(!proxyModel.isForeignKeyItemsEnabled("FE23"));
}

void ForeignKeyProxyModelTest::recordTest()
{
  ForeignKeyRecord record;
  /*
   * Setup source model
   */
  VariantTableModel model;
  model.resize(2, 3);
  model.populateColumn(0, {1,2});
  model.populateColumn(1, {"A","B"});
  model.populateColumn(2, {10,20});
  /*
   * Setup proxy model
   */
  ForeignKeyProxyModel proxyModel;
  proxyModel.setSourceModel(&model);
  proxyModel.addForeignKey("FE1", {0,2});
  proxyModel.addForeignKey("FE2", {1});
  /*
   * Get record for row 0 refering entity FE1
   */
  record = proxyModel.getForeignKeyRecord("FE1", 0);
  QCOMPARE(record.columnCount(), 2);
  QCOMPARE(record.columnAt(0), 0);
  QCOMPARE(record.dataAt(0), QVariant(1));
  QCOMPARE(record.columnAt(1), 2);
  QCOMPARE(record.dataAt(1), QVariant(10));
  /*
   * Get record for row 0 refering entity FE2
   */
  record = proxyModel.getForeignKeyRecord("FE2", 0);
  QCOMPARE(record.columnCount(), 1);
  QCOMPARE(record.columnAt(0), 1);
  QCOMPARE(record.dataAt(0), QVariant("A"));
}

void ForeignKeyProxyModelTest::qtModelTest()
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
  ForeignKeyProxyModel proxyModel;
  proxyModel.setSourceModel(&model);
  /*
   * Test
   */
  QCOMPARE(proxyModel.rowCount(), 3);
  QtModelTest mt(&proxyModel);
  QtModelTest smt(&model);
}

void ForeignKeyProxyModelTest::modelGetFlagsBenchmark()
{
  QFETCH(int, n);
  /*
   * Setup model
   */
  VariantTableModel model;
  model.resize(n, 2);
  model.populateColumnWithInt(0, 1);
  model.populateColumnWithInt(1, -1);
  auto column0flags = model.flags( model.index(0, 0) );
  auto column1flags = model.flags( model.index(0, 1) );
  QBENCHMARK{
    for(int row = 0; row < n; ++row){
      QCOMPARE(model.flags( model.index(row, 0) ), column0flags);
      QCOMPARE(model.flags( model.index(row, 1) ), column1flags);
    }
  }
}

void ForeignKeyProxyModelTest::modelGetFlagsBenchmark_data()
{
  QTest::addColumn<int>("n");

  QTest::newRow("10") << 10;
  QTest::newRow("10'000") << 10000;
}

void ForeignKeyProxyModelTest::foreignKeyModelGetFlagsBenchmark()
{
  QFETCH(int, n);
  /*
   * Setup model
   */
  VariantTableModel model;
  model.resize(n, 2);
  model.populateColumnWithInt(0, 1);
  model.populateColumnWithInt(1, -1);
  /*
   * Setup proxy model
   */
  ForeignKeyProxyModel proxyModel;
  proxyModel.setSourceModel(&model);
  proxyModel.addForeignKey("A", {0});
  proxyModel.addForeignKey("B", {0,1});
  proxyModel.setForeignKeyEditable("A", false);
  auto column0flags = proxyModel.flags( model.index(0, 0) );
  auto column1flags = proxyModel.flags( model.index(0, 1) );
  QBENCHMARK{
    for(int row = 0; row < n; ++row){
      QCOMPARE(proxyModel.flags( model.index(row, 0) ), column0flags);
      QCOMPARE(proxyModel.flags( model.index(row, 1) ), column1flags);
    }
  }
}

void ForeignKeyProxyModelTest::foreignKeyModelGetFlagsBenchmark_data()
{
  QTest::addColumn<int>("n");

  QTest::newRow("10") << 10;
  QTest::newRow("10'000") << 10000;
}

void ForeignKeyProxyModelTest::modelGetDataBenchmark()
{
  QFETCH(int, n);
  /*
   * Setup model
   */
  VariantTableModel model;
  model.resize(n, 2);
  model.populateColumnWithInt(0, 1);
  model.populateColumnWithInt(1, -1);
  QBENCHMARK{
    for(int row = 0; row < n; ++row){
      QCOMPARE(model.data( model.index(row, 0) ).toInt(), row+1);
      QVERIFY(!model.data( model.index(row, 1) ).isNull());
    }
  }
}

void ForeignKeyProxyModelTest::modelGetDataBenchmark_data()
{
  QTest::addColumn<int>("n");

  QTest::newRow("10") << 10;
  QTest::newRow("10'000") << 10000;
}

void ForeignKeyProxyModelTest::foreignKeyModelGetDataBenchmark()
{
  QFETCH(int, n);
  /*
   * Setup model
   */
  VariantTableModel model;
  model.resize(n, 2);
  model.populateColumnWithInt(0, 1);
  model.populateColumnWithInt(1, -1);
  /*
   * Setup proxy model
   */
  ForeignKeyProxyModel proxyModel;
  proxyModel.setSourceModel(&model);
  proxyModel.addForeignKey("A", {0});
  proxyModel.addForeignKey("B", {0,1});
  proxyModel.setForeignKeyEditable("A", false);
  QBENCHMARK{
    for(int row = 0; row < n; ++row){
      QCOMPARE(proxyModel.data( model.index(row, 0) ).toInt(), row+1);
      QVERIFY(!proxyModel.data( model.index(row, 1) ).isNull());
    }
  }
}

void ForeignKeyProxyModelTest::foreignKeyModelGetDataBenchmark_data()
{
  QTest::addColumn<int>("n");

  QTest::newRow("10") << 10;
  QTest::newRow("10'000") << 10000;
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  ForeignKeyProxyModelTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}

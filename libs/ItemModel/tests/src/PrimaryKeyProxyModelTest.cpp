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
#include "PrimaryKeyProxyModelTest.h"
#include "qtmodeltest.h"
#include "Mdt/ItemModel/PrimaryKeyProxyModel.h"
#include "Mdt/ItemModel/VariantTableModel.h"

using namespace Mdt::ItemModel;

void PrimaryKeyProxyModelTest::initTestCase()
{
}

void PrimaryKeyProxyModelTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void PrimaryKeyProxyModelTest::setGetTest()
{
  PrimaryKeyProxyModel proxyModel;
  proxyModel.setPrimaryKey({1,2});
  QCOMPARE(proxyModel.primaryKey().columnCount(), 2);
  proxyModel.clearPrimaryKey();
  QCOMPARE(proxyModel.primaryKey().columnCount(), 0);
}

void PrimaryKeyProxyModelTest::setModelTest()
{
  /*
   * Check that we can do setup before setting source model
   */
  PrimaryKeyProxyModel proxyModel;
  proxyModel.setPrimaryKey({0,1});
  QCOMPARE(proxyModel.rowCount(), 0);
  /*
   * Set source model
   */
  VariantTableModel model;
  model.resize(2, 3);
  proxyModel.setSourceModel(&model);
  QCOMPARE(proxyModel.rowCount(), 2);
}

void PrimaryKeyProxyModelTest::flagsTest()
{
  Qt::ItemFlags expectedFlags;
  /*
   * Setup source model
   */
  VariantTableModel model;
  model.resize(2, 3);
  model.populateColumn(0, {1,2});
  model.populateColumn(1, {"A","B"});
  /*
   * Initial state
   */
  PrimaryKeyProxyModel proxyModel;
  QVERIFY(proxyModel.isPrimaryKeyEditable());
  QVERIFY(proxyModel.isPrimaryKeyItemsEnabled());
  /*
   * Flags by default
   */
  proxyModel.setSourceModel(&model);
  proxyModel.setPrimaryKey({0});
  QCOMPARE(getModelFlags(proxyModel, 0, 0), getModelFlags(model, 0, 0));
  QCOMPARE(getModelFlags(proxyModel, 0, 1), getModelFlags(model, 0, 1));
  /*
   * Disable edition on primary key
   */
  proxyModel.setPrimaryKeyEditable(false);
  QVERIFY(!proxyModel.isPrimaryKeyEditable());
  expectedFlags = getModelFlags(model, 0, 0) & Qt::ItemFlags(~Qt::ItemIsEditable);
  QCOMPARE(getModelFlags(proxyModel, 0, 0), expectedFlags);
  QCOMPARE(getModelFlags(proxyModel, 0, 1), getModelFlags(model, 0, 1));
  /*
   * Enable edition on primary key
   */
  proxyModel.setPrimaryKeyEditable(true);
  QCOMPARE(getModelFlags(proxyModel, 0, 0), getModelFlags(model, 0, 0));
  QCOMPARE(getModelFlags(proxyModel, 0, 1), getModelFlags(model, 0, 1));
  /*
   * Disable item for primary key
   */
  proxyModel.setPrimaryKeyItemsEnabled(false);
  expectedFlags = getModelFlags(model, 0, 0) & Qt::ItemFlags(~Qt::ItemIsEnabled);
  QCOMPARE(getModelFlags(proxyModel, 0, 0), expectedFlags);
  QCOMPARE(getModelFlags(proxyModel, 0, 1), getModelFlags(model, 0, 1));
  /*
   * Disable edition + items on primary key
   */
  proxyModel.setPrimaryKeyEditable(false);
  proxyModel.setPrimaryKeyItemsEnabled(false);
  expectedFlags = getModelFlags(model, 0, 0) & Qt::ItemFlags(~Qt::ItemIsEditable) & Qt::ItemFlags(~Qt::ItemIsEnabled);
  QCOMPARE(getModelFlags(proxyModel, 0, 0), expectedFlags);
  QCOMPARE(getModelFlags(proxyModel, 0, 1), getModelFlags(model, 0, 1));
}

void PrimaryKeyProxyModelTest::recordTest()
{
  PrimaryKeyRecord record;
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
  PrimaryKeyProxyModel proxyModel;
  proxyModel.setSourceModel(&model);
  proxyModel.setPrimaryKey({0,2});
  /*
   * Get record for row 0
   */
  record = proxyModel.getPrimaryKeyRecord(0);
  QCOMPARE(record.columnCount(), 2);
  QCOMPARE(record.columnAt(0), 0);
  QCOMPARE(record.dataAt(0), QVariant(1));
  QCOMPARE(record.columnAt(1), 2);
  QCOMPARE(record.dataAt(1), QVariant(10));
  /*
   * Find record
   */
  QCOMPARE(proxyModel.findRowForPrimaryKeyRecord(record), 0);
  // Match row 1
  record.clear();
  record.append(0, 2);
  record.append(2, 20);
  QCOMPARE(proxyModel.findRowForPrimaryKeyRecord(record), 1);
  // Match non existing row
  record.clear();
  record.append(0, 2);
  record.append(2, 21);
  QCOMPARE(proxyModel.findRowForPrimaryKeyRecord(record), -1);
}

void PrimaryKeyProxyModelTest::qtModelTest()
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
  PrimaryKeyProxyModel proxyModel;
  proxyModel.setSourceModel(&model);
  /*
   * Test
   */
  QCOMPARE(proxyModel.rowCount(), 3);
  QtModelTest mt(&proxyModel);
  QtModelTest smt(&model);
}

void PrimaryKeyProxyModelTest::modelGetFlagsBenchmark()
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

void PrimaryKeyProxyModelTest::modelGetFlagsBenchmark_data()
{
  QTest::addColumn<int>("n");

  QTest::newRow("10") << 10;
  QTest::newRow("10'000") << 10000;
}

void PrimaryKeyProxyModelTest::primaryKeyModelGetFlagsBenchmark()
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
  PrimaryKeyProxyModel proxyModel;
  proxyModel.setSourceModel(&model);
  proxyModel.setPrimaryKey({0});
  proxyModel.setPrimaryKeyEditable(false);
  auto column0flags = proxyModel.flags( model.index(0, 0) );
  auto column1flags = proxyModel.flags( model.index(0, 1) );
  QBENCHMARK{
    for(int row = 0; row < n; ++row){
      QCOMPARE(proxyModel.flags( model.index(row, 0) ), column0flags);
      QCOMPARE(proxyModel.flags( model.index(row, 1) ), column1flags);
    }
  }
}

void PrimaryKeyProxyModelTest::primaryKeyModelGetFlagsBenchmark_data()
{
  QTest::addColumn<int>("n");

  QTest::newRow("10") << 10;
  QTest::newRow("10'000") << 10000;
}

void PrimaryKeyProxyModelTest::modelGetDataBenchmark()
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

void PrimaryKeyProxyModelTest::modelGetDataBenchmark_data()
{
  QTest::addColumn<int>("n");

  QTest::newRow("10") << 10;
  QTest::newRow("10'000") << 10000;
}

void PrimaryKeyProxyModelTest::primaryKeyModelGetDataBenchmark()
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
  PrimaryKeyProxyModel proxyModel;
  proxyModel.setSourceModel(&model);
  proxyModel.setPrimaryKey({0});
  proxyModel.setPrimaryKeyEditable(false);
  QBENCHMARK{
    for(int row = 0; row < n; ++row){
      QCOMPARE(proxyModel.data( model.index(row, 0) ).toInt(), row+1);
      QVERIFY(!proxyModel.data( model.index(row, 1) ).isNull());
    }
  }
}

void PrimaryKeyProxyModelTest::primaryKeyModelGetDataBenchmark_data()
{
  QTest::addColumn<int>("n");

  QTest::newRow("10") << 10;
  QTest::newRow("10'000") << 10000;
}

void PrimaryKeyProxyModelTest::findPrimaryKeyRecordBenchmark()
{
  QFETCH(int, n);
  /*
   * Setup model
   */
  VariantTableModel model;
  model.resize(n, 2);
  model.populateColumnWithInt(0, 1);
  model.populateColumnWithInt(1, 2);
  /*
   * Setup proxy model
   */
  PrimaryKeyProxyModel proxyModel;
  proxyModel.setSourceModel(&model);
  proxyModel.setPrimaryKey({0,1});
  /*
   * Setup PK record that must match
   */
  PrimaryKeyRecord record;
  record.append(0, n);
  record.append(1, n+1);
  QBENCHMARK{
    QCOMPARE( proxyModel.findRowForPrimaryKeyRecord(record), n-1 );
  }
}

void PrimaryKeyProxyModelTest::findPrimaryKeyRecordBenchmark_data()
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
  PrimaryKeyProxyModelTest test;

  return QTest::qExec(&test, argc, argv);
}

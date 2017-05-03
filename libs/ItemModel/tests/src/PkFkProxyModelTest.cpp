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
#include "PkFkProxyModelTest.h"
#include "qtmodeltest.h"
#include "Mdt/ItemModel/PrimaryKeyProxyModel.h"
#include "Mdt/ItemModel/ForeignKeyProxyModelMapItem.h"
#include "Mdt/ItemModel/ForeignKeyProxyModel.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include <QSignalSpy>
#include <QVariantList>
#include <QVector>

using namespace Mdt::ItemModel;

/*
 * Init/cleanup
 */

void PkFkProxyModelTest::initTestCase()
{
}

void PkFkProxyModelTest::cleanupTestCase()
{
}

/*
 * Tests
 */

// void PkFkProxyModelTest::pkSetGetKeyTest()
// {
//   PrimaryKeyProxyModel proxyModel;
//   proxyModel.setPrimaryKey({1,2});
//   QCOMPARE(proxyModel.primaryKey().columnCount(), 2);
// }

// void PkFkProxyModelTest::fkMapItemTest()
// {
//   ForeignKey fk{1,2};
//   ForeignKeyProxyModelMapItem item(fk);
//   QCOMPARE(item.foreignKey().columnCount(), 2);
//   QVERIFY(item.isForeignKeyEditable());
//   QVERIFY(item.isForeignKeyItemsEnabled());
//   item.setForeignKeyEditable(false);
//   QVERIFY(!item.isForeignKeyEditable());
//   QVERIFY(item.isForeignKeyItemsEnabled());
//   item.setForeignKeyItemsEnabled(false);
//   QVERIFY(!item.isForeignKeyEditable());
//   QVERIFY(!item.isForeignKeyItemsEnabled());
// }

// void PkFkProxyModelTest::fkAddGetKeyTest()
// {
//   ForeignKeyProxyModel proxyModel;
//   proxyModel.addForeignKey("A", {2,3});
//   QCOMPARE(proxyModel.getForeignKeyReferencing("A").columnCount(), 2);
// 
// //   proxyModel.setForeignKey({2,3});
// //   QCOMPARE(proxyModel.foreignKey().columnCount(), 2);
// }

// void PkFkProxyModelTest::pkSetModelTest()
// {
//   /*
//    * Check that we can do setup before setting source model
//    */
//   PrimaryKeyProxyModel proxyModel;
//   proxyModel.setPrimaryKey({0,1});
//   
//   QCOMPARE(proxyModel.rowCount(), 0);
// 
//   /*
//    * Set source model
//    */
//   VariantTableModel model;
//   model.resize(2, 3);
//   proxyModel.setSourceModel(&model);
//   
//   QCOMPARE(proxyModel.rowCount(), 2);
//   
//   
// 
//   QFAIL("Not complete");
// }

// void PkFkProxyModelTest::fkSetModelTest()
// {
//   QFAIL("Not complete");
// }

// void PkFkProxyModelTest::pkFlagsTest()
// {
//   Qt::ItemFlags expectedFlags;
//   /*
//    * Setup source model
//    */
//   VariantTableModel model;
//   model.resize(2, 3);
//   model.populateColumn(0, {1,2});
//   model.populateColumn(1, {"A","B"});
//   /*
//    * Initial state
//    */
//   PrimaryKeyProxyModel proxyModel;
//   QVERIFY(proxyModel.isPrimaryKeyEditable());
//   QVERIFY(proxyModel.isPrimaryKeyItemsEnabled());
//   /*
//    * Flags by default
//    */
//   proxyModel.setSourceModel(&model);
//   proxyModel.setPrimaryKey({0});
//   QCOMPARE(getModelFlags(proxyModel, 0, 0), getModelFlags(model, 0, 0));
//   QCOMPARE(getModelFlags(proxyModel, 0, 1), getModelFlags(model, 0, 1));
//   /*
//    * Disable edition on primary key
//    */
//   proxyModel.setPrimaryKeyEditable(false);
//   QVERIFY(!proxyModel.isPrimaryKeyEditable());
//   expectedFlags = getModelFlags(model, 0, 0) & Qt::ItemFlags(~Qt::ItemIsEditable);
//   QCOMPARE(getModelFlags(proxyModel, 0, 0), expectedFlags);
//   QCOMPARE(getModelFlags(proxyModel, 0, 1), getModelFlags(model, 0, 1));
//   /*
//    * Enable edition on primary key
//    */
//   proxyModel.setPrimaryKeyEditable(true);
//   QCOMPARE(getModelFlags(proxyModel, 0, 0), getModelFlags(model, 0, 0));
//   QCOMPARE(getModelFlags(proxyModel, 0, 1), getModelFlags(model, 0, 1));
//   /*
//    * Disable item for primary key
//    */
//   proxyModel.setPrimaryKeyItemsEnabled(false);
//   expectedFlags = getModelFlags(model, 0, 0) & Qt::ItemFlags(~Qt::ItemIsEnabled);
//   QCOMPARE(getModelFlags(proxyModel, 0, 0), expectedFlags);
//   QCOMPARE(getModelFlags(proxyModel, 0, 1), getModelFlags(model, 0, 1));
//   /*
//    * Disable edition + items on primary key
//    */
//   proxyModel.setPrimaryKeyEditable(false);
//   proxyModel.setPrimaryKeyItemsEnabled(false);
//   expectedFlags = getModelFlags(model, 0, 0) & Qt::ItemFlags(~Qt::ItemIsEditable) & Qt::ItemFlags(~Qt::ItemIsEnabled);
//   QCOMPARE(getModelFlags(proxyModel, 0, 0), expectedFlags);
//   QCOMPARE(getModelFlags(proxyModel, 0, 1), getModelFlags(model, 0, 1));
// }

// void PkFkProxyModelTest::fkFlagsTest()
// {
//   Qt::ItemFlags expectedFlags;
//   /*
//    * Setup source model
//    */
//   VariantTableModel model;
//   model.resize(3, 5);
//   model.populateColumn(0, {1,2});
//   model.populateColumn(1, {"A","B"});
//   /*
//    * Default state
//    */
//   ForeignKeyProxyModel proxyModel;
//   proxyModel.setSourceModel(&model);
//   proxyModel.addForeignKey("A", {1});
//   proxyModel.addForeignKey("B", {2,3});
//   // Check flags for FK -> A
//   QVERIFY(proxyModel.isForeignKeyEditable("A"));
//   QVERIFY(proxyModel.isForeignKeyItemsEnabled("A"));
//   QCOMPARE(getModelFlags(proxyModel, 0, 1), getModelFlags(model, 0, 1));
//   // Check flags for FK -> B
//   QVERIFY(proxyModel.isForeignKeyEditable("B"));
//   QVERIFY(proxyModel.isForeignKeyItemsEnabled("B"));
//   QCOMPARE(getModelFlags(proxyModel, 0, 2), getModelFlags(model, 0, 2));
//   QCOMPARE(getModelFlags(proxyModel, 0, 3), getModelFlags(model, 0, 3));
//   /*
//    * Disable editable flags for A
//    */
//   proxyModel.setForeignKeyEditable("A", false);
//   proxyModel.setForeignKeyItemsEnabled("A", true);
//   // Check flags for FK -> A
//   QVERIFY(!proxyModel.isForeignKeyEditable("A"));
//   QVERIFY( proxyModel.isForeignKeyItemsEnabled("A"));
//   expectedFlags = getModelFlags(model, 0, 1) & Qt::ItemFlags(~Qt::ItemIsEditable);
//   QCOMPARE(getModelFlags(proxyModel, 0, 1), expectedFlags);
//   // Check flags for FK -> B
//   QVERIFY(proxyModel.isForeignKeyEditable("B"));
//   QVERIFY(proxyModel.isForeignKeyItemsEnabled("B"));
//   QCOMPARE(getModelFlags(proxyModel, 0, 2), getModelFlags(model, 0, 2));
//   QCOMPARE(getModelFlags(proxyModel, 0, 3), getModelFlags(model, 0, 3));
//   /*
//    * Disable items enable flags for A
//    */
//   proxyModel.setForeignKeyEditable("A", true);
//   proxyModel.setForeignKeyItemsEnabled("A", false);
//   // Check flags for FK -> A
//   QVERIFY( proxyModel.isForeignKeyEditable("A"));
//   QVERIFY(!proxyModel.isForeignKeyItemsEnabled("A"));
//   expectedFlags = getModelFlags(model, 0, 1) & Qt::ItemFlags(~Qt::ItemIsEnabled);
//   QCOMPARE(getModelFlags(proxyModel, 0, 1), expectedFlags);
//   // Check flags for FK -> B
//   QVERIFY(proxyModel.isForeignKeyEditable("B"));
//   QVERIFY(proxyModel.isForeignKeyItemsEnabled("B"));
//   QCOMPARE(getModelFlags(proxyModel, 0, 2), getModelFlags(model, 0, 2));
//   QCOMPARE(getModelFlags(proxyModel, 0, 3), getModelFlags(model, 0, 3));
//   /*
//    * Disable editable + enabled flags for A
//    */
//   proxyModel.setForeignKeyEditable("A", false);
//   proxyModel.setForeignKeyItemsEnabled("A", false);
//   // Check flags for FK -> A
//   QVERIFY( proxyModel.isForeignKeyEditable("A"));
//   QVERIFY( proxyModel.isForeignKeyItemsEnabled("A"));
//   expectedFlags = getModelFlags(model, 0, 1) & Qt::ItemFlags(~Qt::ItemIsEditable) & Qt::ItemFlags(~Qt::ItemIsEnabled);
//   QCOMPARE(getModelFlags(proxyModel, 0, 1), expectedFlags);
//   // Check flags for FK -> B
//   QVERIFY(proxyModel.isForeignKeyEditable("B"));
//   QVERIFY(proxyModel.isForeignKeyItemsEnabled("B"));
//   QCOMPARE(getModelFlags(proxyModel, 0, 2), getModelFlags(model, 0, 2));
//   QCOMPARE(getModelFlags(proxyModel, 0, 3), getModelFlags(model, 0, 3));
//   /*
//    * Disable editable + enabled flags for all foreign keys
//    */
//   proxyModel.setAllForeignKeysEditable(false);
//   proxyModel.setAllForeignKeysItemsEnabled(false);
//   QVERIFY(!proxyModel.isForeignKeyEditable("A"));
//   QVERIFY(!proxyModel.isForeignKeyItemsEnabled("A"));
//   QVERIFY(!proxyModel.isForeignKeyEditable("B"));
//   QVERIFY(!proxyModel.isForeignKeyItemsEnabled("B"));
//   /*
//    * Enable editable flag for all foreign keys
//    */
//   proxyModel.setAllForeignKeysEditable(true);
//   proxyModel.setAllForeignKeysItemsEnabled(false);
//   QVERIFY( proxyModel.isForeignKeyEditable("A"));
//   QVERIFY(!proxyModel.isForeignKeyItemsEnabled("A"));
//   QVERIFY( proxyModel.isForeignKeyEditable("B"));
//   QVERIFY(!proxyModel.isForeignKeyItemsEnabled("B"));
// 
// //   QVERIFY(proxyModel.isForeignKeyEditable());
// //   QVERIFY(proxyModel.isForeignKeyItemsEnabled());
// //   proxyModel.setForeignKeyEditable(false);
// //   proxyModel.setForeignKeyItemsEnabled(false);
// //   QVERIFY(!proxyModel.isForeignKeyEditable());
// //   QVERIFY(!proxyModel.isForeignKeyItemsEnabled());
// }

// void PkFkProxyModelTest::modelGetFlagsBenchmark()
// {
//   QFETCH(int, n);
//   /*
//    * Setup model
//    */
//   VariantTableModel model;
//   model.resize(n, 2);
//   model.populateColumnWithInt(0, 1);
//   model.populateColumnWithInt(1, -1);
//   auto column0flags = model.flags( model.index(0, 0) );
//   auto column1flags = model.flags( model.index(0, 1) );
//   QBENCHMARK{
//     for(int row = 0; row < n; ++row){
//       QCOMPARE(model.flags( model.index(row, 0) ), column0flags);
//       QCOMPARE(model.flags( model.index(row, 1) ), column1flags);
//     }
//   }
// }

// void PkFkProxyModelTest::modelGetFlagsBenchmark_data()
// {
//   QTest::addColumn<int>("n");
// 
//   QTest::newRow("10") << 10;
//   QTest::newRow("10'000") << 10000;
// }

// void PkFkProxyModelTest::foreignKeyModelGetFlagsBenchmark()
// {
//   QFETCH(int, n);
//   /*
//    * Setup model
//    */
//   VariantTableModel model;
//   model.resize(n, 2);
//   model.populateColumnWithInt(0, 1);
//   model.populateColumnWithInt(1, -1);
//   /*
//    * Setup proxy model
//    */
//   ForeignKeyProxyModel proxyModel;
//   proxyModel.setSourceModel(&model);
//   proxyModel.addForeignKey("A", {0});
//   proxyModel.addForeignKey("B", {0,1});
//   proxyModel.setForeignKeyEditable("A", false);
//   auto column0flags = proxyModel.flags( model.index(0, 0) );
//   auto column1flags = proxyModel.flags( model.index(0, 1) );
//   QBENCHMARK{
//     for(int row = 0; row < n; ++row){
//       QCOMPARE(proxyModel.flags( model.index(row, 0) ), column0flags);
//       QCOMPARE(proxyModel.flags( model.index(row, 1) ), column1flags);
//     }
//   }
// }

// void PkFkProxyModelTest::foreignKeyModelGetFlagsBenchmark_data()
// {
//   QTest::addColumn<int>("n");
// 
//   QTest::newRow("10") << 10;
//   QTest::newRow("10'000") << 10000;
// }

// void PkFkProxyModelTest::primaryKeyRecordTest()
// {
//   PrimaryKeyRecord record;
//   /*
//    * Setup source model
//    */
//   VariantTableModel model;
//   model.resize(2, 3);
//   model.populateColumn(0, {1,2});
//   model.populateColumn(1, {"A","B"});
//   model.populateColumn(2, {10,20});
//   /*
//    * Setup proxy model
//    */
//   PrimaryKeyProxyModel proxyModel;
//   proxyModel.setSourceModel(&model);
//   proxyModel.setPrimaryKey({0,2});
//   /*
//    * Get record for row 0
//    */
//   record = proxyModel.primaryKeyRecord(0);
//   QCOMPARE(record.columnCount(), 2);
//   QCOMPARE(record.columnAt(0), 0);
//   QCOMPARE(record.dataAt(0), QVariant(1));
//   QCOMPARE(record.columnAt(1), 2);
//   QCOMPARE(record.dataAt(1), QVariant(10));
//   /*
//    * Find record
//    */
//   QCOMPARE(proxyModel.findRowForPrimaryKeyRecord(record), 0);
//   // Match row 1
//   record.clear();
//   record.append(0, 2);
//   record.append(2, 20);
//   QCOMPARE(proxyModel.findRowForPrimaryKeyRecord(record), 1);
//   // Match non existing row
//   record.clear();
//   record.append(0, 2);
//   record.append(2, 21);
//   QCOMPARE(proxyModel.findRowForPrimaryKeyRecord(record), -1);
// }

// void PkFkProxyModelTest::foreignKeyRecordTest()
// {
//   ForeignKeyRecord record;
//   /*
//    * Setup source model
//    */
//   VariantTableModel model;
//   model.resize(2, 3);
//   model.populateColumn(0, {1,2});
//   model.populateColumn(1, {"A","B"});
//   model.populateColumn(2, {10,20});
//   /*
//    * Setup proxy model
//    */
//   ForeignKeyProxyModel proxyModel;
//   proxyModel.setSourceModel(&model);
//   proxyModel.addForeignKey("FE1", {0,2});
//   proxyModel.addForeignKey("FE2", {1});
//   /*
//    * Get record for row 0 refering entity FE1
//    */
//   record = proxyModel.getForeignKeyRecord("FE1", 0);
//   QCOMPARE(record.columnCount(), 2);
//   QCOMPARE(record.columnAt(0), 0);
//   QCOMPARE(record.dataAt(0), QVariant(1));
//   QCOMPARE(record.columnAt(1), 2);
//   QCOMPARE(record.dataAt(1), QVariant(10));
//   /*
//    * Get record for row 0 refering entity FE2
//    */
//   record = proxyModel.getForeignKeyRecord("FE2", 0);
//   QCOMPARE(record.columnCount(), 1);
//   QCOMPARE(record.columnAt(0), 1);
//   QCOMPARE(record.dataAt(0), QVariant("A"));
// 
// //   /*
// //    * Setup proxy model
// //    */
// //   ForeignKeyProxyModel proxyModel;
// //   proxyModel.setSourceModel(&model);
// //   proxyModel.setForeignKey({0,2});
// //   /*
// //    * Get record for row 0
// //    */
// //   record = proxyModel.foreignKeyRecord(0);
// //   QCOMPARE(record.columnCount(), 2);
// //   QCOMPARE(record.columnAt(0), 0);
// //   QCOMPARE(record.dataAt(0), QVariant(1));
// //   QCOMPARE(record.columnAt(1), 2);
// //   QCOMPARE(record.dataAt(1), QVariant(10));
// }

// void PkFkProxyModelTest::pkQtModelTest()
// {
//   /*
//    * Setup model
//    */
//   VariantTableModel model;
//   model.resize(3, 2);
//   model.populateColumn(0, {1,2,3});
//   model.populateColumn(1, {"A","B","C"});
//   /*
//    * Setup proxy model
//    */
//   PrimaryKeyProxyModel proxyModel;
//   proxyModel.setSourceModel(&model);
//   /*
//    * Test
//    */
//   QCOMPARE(proxyModel.rowCount(), 3);
//   QtModelTest mt(&proxyModel);
//   QtModelTest smt(&model);
// }

// void PkFkProxyModelTest::fkQtModelTest()
// {
//   /*
//    * Setup model
//    */
//   VariantTableModel model;
//   model.resize(3, 2);
//   model.populateColumn(0, {1,2,3});
//   model.populateColumn(1, {"A","B","C"});
//   /*
//    * Setup proxy model
//    */
//   ForeignKeyProxyModel proxyModel;
//   proxyModel.setSourceModel(&model);
//   /*
//    * Test
//    */
//   QCOMPARE(proxyModel.rowCount(), 3);
//   QtModelTest mt(&proxyModel);
//   QtModelTest smt(&model);
// }

// void PkFkProxyModelTest::modelGetDataBenchmark()
// {
//   QFETCH(int, n);
//   /*
//    * Setup model
//    */
//   VariantTableModel model;
//   model.resize(n, 2);
//   model.populateColumnWithInt(0, 1);
//   model.populateColumnWithInt(1, -1);
//   auto column0flags = model.flags( model.index(0, 0) );
//   auto column1flags = model.flags( model.index(0, 1) );
//   QBENCHMARK{
//     for(int row = 0; row < n; ++row){
//       QCOMPARE(model.data( model.index(row, 0) ).toInt(), row+1);
//       QVERIFY(!model.data( model.index(row, 1) ).isNull());
//       QCOMPARE(model.flags( model.index(row, 0) ), column0flags);
//       QCOMPARE(model.flags( model.index(row, 1) ), column1flags);
//     }
//   }
// }

// void PkFkProxyModelTest::modelGetDataBenchmark_data()
// {
//   QTest::addColumn<int>("n");
// 
//   QTest::newRow("10") << 10;
//   QTest::newRow("10'000") << 10000;
// }

// void PkFkProxyModelTest::proxyModelGetDataBenchmark()
// {
//   QFETCH(int, n);
//   /*
//    * Setup model
//    */
//   VariantTableModel model;
//   model.resize(n, 2);
//   model.populateColumnWithInt(0, 1);
//   model.populateColumnWithInt(1, -1);
//   /*
//    * Setup proxy model
//    */
//   PrimaryKeyProxyModel proxyModel;
//   proxyModel.setSourceModel(&model);
//   proxyModel.setPrimaryKey({0});
//   proxyModel.setPrimaryKeyEditable(false);
//   auto column0flags = proxyModel.flags( model.index(0, 0) );
//   auto column1flags = proxyModel.flags( model.index(0, 1) );
//   QBENCHMARK{
//     for(int row = 0; row < n; ++row){
//       QCOMPARE(proxyModel.data( proxyModel.index(row, 0) ).toInt(), row+1);
//       QVERIFY(!proxyModel.data( proxyModel.index(row, 1) ).isNull());
//       QCOMPARE(proxyModel.flags( model.index(row, 0) ), column0flags);
//       QCOMPARE(proxyModel.flags( model.index(row, 1) ), column1flags);
//     }
//   }
// }

// void PkFkProxyModelTest::proxyModelGetDataBenchmark_data()
// {
//   QTest::addColumn<int>("n");
// 
//   QTest::newRow("10") << 10;
//   QTest::newRow("10'000") << 10000;
// }

// void PkFkProxyModelTest::findPrimaryKeyRecordBenchmark()
// {
//   QFETCH(int, n);
//   /*
//    * Setup model
//    */
//   VariantTableModel model;
//   model.resize(n, 2);
//   model.populateColumnWithInt(0, 1);
//   model.populateColumnWithInt(1, 2);
//   /*
//    * Setup proxy model
//    */
//   PrimaryKeyProxyModel proxyModel;
//   proxyModel.setSourceModel(&model);
//   proxyModel.setPrimaryKey({0,1});
//   /*
//    * Setup PK record that must match
//    */
//   PrimaryKeyRecord record;
//   record.append(0, n);
//   record.append(1, n+1);
//   QBENCHMARK{
//     QCOMPARE( proxyModel.findRowForPrimaryKeyRecord(record), n-1 );
//   }
// }

// void PkFkProxyModelTest::findPrimaryKeyRecordBenchmark_data()
// {
//   QTest::addColumn<int>("n");
// 
//   QTest::newRow("10") << 10;
//   QTest::newRow("10'000") << 10000;
// }

/*
 * Main
 */
MDT_ITEM_MODEL_TEST_MAIN(PkFkProxyModelTest)
// MDT_ITEM_MODEL_TEST_MAIN_DEBUG_ENV(PkFkProxyModelTest)

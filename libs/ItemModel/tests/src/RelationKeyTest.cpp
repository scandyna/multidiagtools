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
#include "RelationKeyTest.h"
#include "Mdt/Application.h"
#include "Mdt/ItemModel/PrimaryKey.h"
#include "Mdt/ItemModel/ForeignKey.h"
#include "Mdt/ItemModel/RelationKey.h"
#include "Mdt/ItemModel/RelationKeyCopier.h"
#include "Mdt/ItemModel/VariantTableModel.h"

using namespace Mdt::ItemModel;

void RelationKeyTest::initTestCase()
{
}

void RelationKeyTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void RelationKeyTest::columnPairTest()
{
  RelationColumnPair p(1, 2);
  QCOMPARE(p.parentModelColumn(), 1);
  QCOMPARE(p.childModelColumn(), 2);
}

void RelationKeyTest::relationKeyTest()
{
  /*
   * Initial state
   */
  RelationKey key;
  QCOMPARE(key.columnPairCount(), 0);
  QVERIFY(key.isNull());
  QVERIFY(!key.containsColumnPair(1, 2));
  QCOMPARE(key.greatestParentModelColumn(), -1);
  QCOMPARE(key.greatestChildModelColumn(), -1);
  QVERIFY(key.begin() == key.end());
  /*
   * Add column pair
   */
  key.addColumnPair(1, 2);
  QCOMPARE(key.columnPairCount(), 1);
  QVERIFY(!key.isNull());
  QCOMPARE(key.columnPairAt(0).parentModelColumn(), 1);
  QCOMPARE(key.columnPairAt(0).childModelColumn(), 2);
  QVERIFY(key.containsColumnPair(1, 2));
  QVERIFY(!key.containsColumnPair(2, 1));
  QVERIFY(!key.containsColumnPair(1, 1));
  QCOMPARE(key.greatestParentModelColumn(), 1);
  QCOMPARE(key.greatestChildModelColumn(), 2);
  /*
   * Iterate
   */
  for(const auto cp : key){
    QCOMPARE(cp.parentModelColumn(), 1);
    QCOMPARE(cp.childModelColumn(), 2);
  }
  /*
   * Set key
   */
  PrimaryKey pk({3,4});
  ForeignKey fk({5,6});
  key.setKey(pk, fk);
  // Check first pair
  auto it = key.begin();
  QVERIFY(it != key.end());
  QCOMPARE(it->parentModelColumn(), 3);
  QCOMPARE(it->childModelColumn(), 5);
  // Check second pair
  ++it;
  QVERIFY(it != key.end());
  QCOMPARE(it->parentModelColumn(), 4);
  QCOMPARE(it->childModelColumn(), 6);
  // Check greatest columns
  QCOMPARE(key.greatestParentModelColumn(), 4);
  QCOMPARE(key.greatestChildModelColumn(), 6);
  /*
   * Clear
   */
  key.clear();
  QVERIFY(key.isNull());
  QVERIFY(key.begin() == key.end());
}

void RelationKeyTest::keyCopierSetupTest()
{
  /*
   * Initial state
   */
  RelationKeyCopier kc;
//   QVERIFY(!(kc.copyTriggers() & RelationKeyCopier::ChildModelRowsInserted));
//   QVERIFY(!(kc.copyTriggers() & RelationKeyCopier::ParentModelDataChanged));
  QVERIFY(kc.key().isNull());
  QCOMPARE(kc.parentModelCurrentRow(), -1);
  // Set current row without parent model
  kc.setParentModelCurrentRow(2);
  QCOMPARE(kc.parentModelCurrentRow(), -1);
  /*
   * Set a model as parent
   */
  VariantTableModel model1;
  model1.resize(2, 1);
  kc.setParentModel(&model1);
  // Set current row
  kc.setParentModelCurrentRow(1);
  QCOMPARE(kc.parentModelCurrentRow(), 1);
  /*
   * Set a model as child
   */
  VariantTableModel model2;
  model2.resize(1, 1);
  kc.setChildModel(&model2);
  /*
   * Set key using addColumnPair()
   */
  QVERIFY(kc.key().isNull());
  kc.addColumnPair(0, 0);
  QVERIFY(!kc.key().isNull());
  /*
   * Change parent model
   */
  VariantTableModel parentModel;
  parentModel.resize(2, 3);
  kc.setParentModel(&parentModel);
  QVERIFY(!kc.key().isNull());
  QCOMPARE(kc.parentModelCurrentRow(), -1);
  /*
   * Set key using setKey(PK,FK)
   */
  PrimaryKey pk({0});
  ForeignKey fk({0});
  kc.setKey(pk, fk);
  QVERIFY(!kc.key().isNull());
  /*
   * Change child model
   */
  VariantTableModel childModel;
  childModel.resize(1, 5);
  kc.setChildModel(&childModel);
  QVERIFY(!kc.key().isNull());
  /*
   * Set key using setKey(RelationKey)
   */
  RelationKey key;
  key.addColumnPair(1, 2);
  kc.setKey(key);
  QVERIFY(!kc.key().isNull());
}

// void RelationKeyTest::keyCopierTriggersTest()
// {
//   /*
//    * Initial state
//    */
//   RelationKeyCopier kc;
//   QCOMPARE(kc.copyTriggers(), RelationKeyCopier::NoTrigger);
//   QVERIFY(!kc.isChildModelRowsInsertedTriggerEnabled());
//   /*
//    * Set ChildModelRowsInserted trigger
//    */
//   kc.setChildModelRowsInsertedTriggerEnabled(true);
//   QCOMPARE(kc.copyTriggers(), RelationKeyCopier::ChildModelRowsInserted);
//   QVERIFY(kc.isChildModelRowsInsertedTriggerEnabled());
//   kc.setChildModelRowsInsertedTriggerEnabled(false);
//   QCOMPARE(kc.copyTriggers(), RelationKeyCopier::NoTrigger);
//   QVERIFY(!kc.isChildModelRowsInsertedTriggerEnabled());
//   /*
//    * Set ParentModelDataChanged trigger
//    */
//   kc.setCopyTriggers(RelationKeyCopier::ParentModelDataChanged);
//   QCOMPARE(kc.copyTriggers(), RelationKeyCopier::ParentModelDataChanged);
//   /*
//    * Set both
//    */
//   kc.setCopyTriggers(RelationKeyCopier::ChildModelRowsInserted | RelationKeyCopier::ParentModelDataChanged);
//   QVERIFY( (kc.copyTriggers() & RelationKeyCopier::ChildModelRowsInserted) );
//   QVERIFY( (kc.copyTriggers() & RelationKeyCopier::ParentModelDataChanged) );
//   kc.setChildModelRowsInsertedTriggerEnabled(false);
//   QVERIFY(!(kc.copyTriggers() & RelationKeyCopier::ChildModelRowsInserted) );
//   QVERIFY( (kc.copyTriggers() & RelationKeyCopier::ParentModelDataChanged) );
//   kc.setChildModelRowsInsertedTriggerEnabled(true);
//   QVERIFY( (kc.copyTriggers() & RelationKeyCopier::ChildModelRowsInserted) );
//   QVERIFY( (kc.copyTriggers() & RelationKeyCopier::ParentModelDataChanged) );
// }

// void RelationKeyTest::keyCopierTriggerBenchmark()
// {
//   RelationKeyCopier kc;
//   VariantTableModel parentModel;
//   parentModel.resize(1,1);
//   VariantTableModel childModel;
//   childModel.resize(1,1);
//   kc.setParentModel(&parentModel);
//   kc.setChildModel(&childModel);
//   QBENCHMARK{
//     kc.setChildModelRowsInsertedTriggerEnabled(true);
//     kc.setChildModelRowsInsertedTriggerEnabled(false);
//     kc.setChildModelRowsInsertedTriggerEnabled(true);
//   }
//   QVERIFY(kc.isChildModelRowsInsertedTriggerEnabled());
// }

void RelationKeyTest::keyCopierParentModelCurrentRowTest()
{
  /*
   * Initial state
   */
  RelationKeyCopier kc;
  QCOMPARE(kc.parentModelCurrentRow(), -1);
  kc.setParentModelCurrentRow(-1);
  QCOMPARE(kc.parentModelCurrentRow(), -1);
  /*
   * Set parent model
   */
  VariantTableModel parentModel;
  parentModel.resize(2, 1);
  kc.setParentModel(&parentModel);
  QCOMPARE(kc.parentModelCurrentRow(), -1);
  kc.setParentModelCurrentRow(1);
  QCOMPARE(kc.parentModelCurrentRow(), 1);
  /*
   * Set child model
   */
  VariantTableModel childModel;
  childModel.resize(3, 1);
  kc.setChildModel(&childModel);
  QCOMPARE(kc.parentModelCurrentRow(), 1);
  /*
   * Change parent model current row
   */
  kc.setParentModelCurrentRow(1);
  QCOMPARE(kc.parentModelCurrentRow(), 1);
  kc.setParentModelCurrentRow(0);
  QCOMPARE(kc.parentModelCurrentRow(), 0);
  /*
   * Change parent model
   */
  VariantTableModel parentModel2;
  parentModel2.resize(3, 1);
  kc.setParentModel(&parentModel2);
  QCOMPARE(kc.parentModelCurrentRow(), -1);
}

void RelationKeyTest::keyCopierInsertIntoChildModelTest()
{
  RelationKeyCopier kc;
  RowRange r;
  /*
   * Setup parent model
   * ---------
   * |IdA|IdB|
   * ---------
   * | 1 | A |
   * ---------
   * | 2 | B |
   * ---------
   */
  VariantTableModel parentModel;
  parentModel.resize(2, 2);
  parentModel.populateColumn(0, {1 ,2});
  parentModel.populateColumn(1, {"A","B"});
  /*
   * Setup child model
   * ------------
   * |Id|IdA|IdB|
   * ------------
   */
  VariantTableModel childModel;
  childModel.resize(0, 3);
  /*
   * Setup relation copier
   */
  kc.setParentModel(&parentModel);
  kc.setChildModel(&childModel);
  PrimaryKey pk({0,1});
  ForeignKey fk({1,2});
  kc.setKey(pk, fk);
  kc.setParentModelCurrentRow(0);
//   kc.setChildModelRowsInsertedTriggerEnabled(true);
  /*
   * Insert a row into child model
   * ------------
   * |Id|IdA|IdB|
   * ------------
   * |  | 1 | A |
   * ------------
   */
  childModel.insertRow(0);
  r.setFirstRow(0);
  r.setRowCount(1);
  QVERIFY(kc.copyAllKeyData(r));
  QCOMPARE(childModel.rowCount(), 1);
  QCOMPARE(childModel.data(0, 1), QVariant(1));
  QCOMPARE(childModel.data(0, 2), QVariant("A"));
  /*
   * Insert 2 rows at end
   * ------------
   * |Id|IdA|IdB|
   * ------------
   * |  | 1 | A |
   * ------------
   * |  | 2 | B |
   * ------------
   * |  | 2 | B |
   * ------------
   */
  kc.setParentModelCurrentRow(1);
  childModel.insertRows(1, 2);
  r.setFirstRow(1);
  r.setRowCount(2);
  QVERIFY(kc.copyAllKeyData(r));
  QCOMPARE(childModel.rowCount(), 3);
  QCOMPARE(childModel.data(0, 1), QVariant(1));
  QCOMPARE(childModel.data(0, 2), QVariant("A"));
  QCOMPARE(childModel.data(1, 1), QVariant(2));
  QCOMPARE(childModel.data(1, 2), QVariant("B"));
  QCOMPARE(childModel.data(2, 1), QVariant(2));
  QCOMPARE(childModel.data(2, 2), QVariant("B"));
  /*
   * Insert 2 rows at the beginning
   * ------------
   * |Id|IdA|IdB|
   * ------------
   * |  | 2 | B |
   * ------------
   * |  | 2 | B |
   * ------------
   * |  | 1 | A |
   * ------------
   * |  | 2 | B |
   * ------------
   * |  | 2 | B |
   * ------------
   */
  childModel.insertRows(0, 2);
  r.setFirstRow(0);
  r.setRowCount(2);
  QVERIFY(kc.copyAllKeyData(r));
  QCOMPARE(childModel.rowCount(), 5);
  QCOMPARE(childModel.data(0, 1), QVariant(2));
  QCOMPARE(childModel.data(0, 2), QVariant("B"));
  QCOMPARE(childModel.data(1, 1), QVariant(2));
  QCOMPARE(childModel.data(1, 2), QVariant("B"));
  QCOMPARE(childModel.data(2, 1), QVariant(1));
  QCOMPARE(childModel.data(2, 2), QVariant("A"));
  QCOMPARE(childModel.data(3, 1), QVariant(2));
  QCOMPARE(childModel.data(3, 2), QVariant("B"));
  QCOMPARE(childModel.data(4, 1), QVariant(2));
  QCOMPARE(childModel.data(4, 2), QVariant("B"));
  /*
   * Disable copy on insertion into child model
   * Insert a row at end
   * ------------
   * |Id|IdA|IdB|
   * ------------
   * |  | 2 | B |
   * ------------
   * |  | 2 | B |
   * ------------
   * |  | 1 | A |
   * ------------
   * |  | 2 | B |
   * ------------
   * |  | 2 | B |
   * ------------
   * |  |   |   |
   * ------------
   */
//   kc.setChildModelRowsInsertedTriggerEnabled(false);
//   childModel.insertRow(5);
//   QCOMPARE(childModel.rowCount(), 6);
//   QCOMPARE(childModel.data(0, 1), QVariant(2));
//   QCOMPARE(childModel.data(0, 2), QVariant("B"));
//   QCOMPARE(childModel.data(1, 1), QVariant(2));
//   QCOMPARE(childModel.data(1, 2), QVariant("B"));
//   QCOMPARE(childModel.data(2, 1), QVariant(1));
//   QCOMPARE(childModel.data(2, 2), QVariant("A"));
//   QCOMPARE(childModel.data(3, 1), QVariant(2));
//   QCOMPARE(childModel.data(3, 2), QVariant("B"));
//   QCOMPARE(childModel.data(4, 1), QVariant(2));
//   QCOMPARE(childModel.data(4, 2), QVariant("B"));
//   QVERIFY(childModel.data(5, 1).isNull());
//   QVERIFY(childModel.data(5, 2).isNull());
  /*
   * Enable copy on insertion into child model
   * Insert a row at end
   * ------------
   * |Id|IdA|IdB|
   * ------------
   * |  | 2 | B |
   * ------------
   * |  | 2 | B |
   * ------------
   * |  | 1 | A |
   * ------------
   * |  | 2 | B |
   * ------------
   * |  | 2 | B |
   * ------------
   * |  |   |   |
   * ------------
   * |  | 2 | B |
   * ------------
   */
//   kc.setChildModelRowsInsertedTriggerEnabled(true);
//   childModel.insertRow(6);
//   QCOMPARE(childModel.rowCount(), 7);
//   QCOMPARE(childModel.data(0, 1), QVariant(2));
//   QCOMPARE(childModel.data(0, 2), QVariant("B"));
//   QCOMPARE(childModel.data(1, 1), QVariant(2));
//   QCOMPARE(childModel.data(1, 2), QVariant("B"));
//   QCOMPARE(childModel.data(2, 1), QVariant(1));
//   QCOMPARE(childModel.data(2, 2), QVariant("A"));
//   QCOMPARE(childModel.data(3, 1), QVariant(2));
//   QCOMPARE(childModel.data(3, 2), QVariant("B"));
//   QCOMPARE(childModel.data(4, 1), QVariant(2));
//   QCOMPARE(childModel.data(4, 2), QVariant("B"));
//   QVERIFY(childModel.data(5, 1).isNull());
//   QVERIFY(childModel.data(5, 2).isNull());
//   QCOMPARE(childModel.data(6, 1), QVariant(2));
//   QCOMPARE(childModel.data(6, 2), QVariant("B"));
}

void RelationKeyTest::keyCopierEditParentModelTest()
{
  QFAIL("Not implemented");
}

/// \todo Add some test that removes rows (something goes wrong with current row..)

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  RelationKeyTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}

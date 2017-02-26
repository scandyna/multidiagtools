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
#include "RelationFilterProxyModelTest.h"
#include "qtmodeltest.h"
#include "Mdt/ItemModel/RelationFilterProxyModel.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include "Mdt/ItemModel/FormatProxyModel.h"
#include <QStringList>
#include <QModelIndex>
#include <QTableView>
#include <QListView>
#include <QTreeView>
#include <QComboBox>
#include <QSignalSpy>
#include <QVariantList>
#include <QDebug>
#include <iterator>
#include <vector>
#include <array>
#include <algorithm>

using namespace Mdt::ItemModel;
// namespace ItemModel = Mdt::ItemModel;
// using ItemModel::VariantTableModel;
// using ItemModel::RelationFilterProxyModel;
// using ItemModel::FilterColumn;
// using ItemModel::ParentModelColumn;
// using Like = ItemModel::LikeExpression;

void RelationFilterProxyModelTest::initTestCase()
{
}

void RelationFilterProxyModelTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void RelationFilterProxyModelTest::setModelTest()
{
  /*
   * Initial state
   */
  RelationFilterProxyModel proxyModel;
  QCOMPARE(proxyModel.rowCount(), 0);
  /*
   * Check that we can do setup without any model set
   */
  ParentModelColumn pmc(0);
  FilterColumn fc(1);
  proxyModel.setFilter(fc == pmc);
  proxyModel.setFilterRole(Qt::DisplayRole);
  proxyModel.setDynamicSortFilter(true);
  /*
   * Set source model
   */
  VariantTableModel model;
  model.resize(6, 2);
  model.populateColumn(1, {1,2,2,3,3,3});
  proxyModel.setSourceModel(&model);
  QCOMPARE(proxyModel.rowCount(), 0);
  /*
   * Set parent model
   */
  VariantTableModel parentModel;
  parentModel.resize(3, 1);
  parentModel.populateColumn(0, {1,2,3});
  proxyModel.setParentModel(&parentModel);
  QCOMPARE(proxyModel.rowCount(), 0);
  /*
   * Apply filter
   */
  proxyModel.setParentModelMatchRow(0);
  QCOMPARE(proxyModel.rowCount(), 1);
  /*
   * Change source model with lower column count
   * Goal: check that setup order does not matter
   *       ( setSourceModel() , setFilter() )
   */
  VariantTableModel model2;
  model2.resize(4, 1);
  model2.populateColumn(0, {1,2,2,3});
  proxyModel.setSourceModel(&model2);
  QCOMPARE(proxyModel.rowCount(), 0);
}

void RelationFilterProxyModelTest::setFilterTest()
{
  /*
   * Initial state
   */
  RelationFilterProxyModel proxyModel;
  QVERIFY(proxyModel.relationKeyForEquality().isNull());
  /*
   * Set filter using expression
   */
  proxyModel.setFilter(ChildModelColumn(2) == ParentModelColumn(1));
  QCOMPARE(proxyModel.relationKeyForEquality().columnPairCount(), 1);
  QCOMPARE(proxyModel.relationKeyForEquality().columnPairAt(0).parentModelColumn(), 1);
  QCOMPARE(proxyModel.relationKeyForEquality().columnPairAt(0).childModelColumn(), 2);
  /*
   * Set filter with a relation key
   */
  RelationKey rk;
  rk.addColumnPair(3, 2);
  proxyModel.setFilter(rk);
  QCOMPARE(proxyModel.relationKeyForEquality().columnPairCount(), 1);
  QCOMPARE(proxyModel.relationKeyForEquality().columnPairAt(0).parentModelColumn(), 3);
  QCOMPARE(proxyModel.relationKeyForEquality().columnPairAt(0).childModelColumn(), 2);
  /*
   * Set filter sepcifying parent model PK and child model FK
   */
  PrimaryKey pk({1});
  ForeignKey fk({2});
  proxyModel.setFilter(pk, fk);
  QCOMPARE(proxyModel.relationKeyForEquality().columnPairCount(), 1);
  QCOMPARE(proxyModel.relationKeyForEquality().columnPairAt(0).parentModelColumn(), 1);
  QCOMPARE(proxyModel.relationKeyForEquality().columnPairAt(0).childModelColumn(), 2);
}

void RelationFilterProxyModelTest::parentModelMatchRowTest()
{
  /*
   * Initial state
   */
  RelationFilterProxyModel proxyModel;
  QCOMPARE(proxyModel.parentModelMatchRow(), -1);
  // Set current row must be accepted without parent model set
  proxyModel.setParentModelMatchRow(-1);
  QCOMPARE(proxyModel.parentModelMatchRow(), -1);
  // Verify bound checking
  proxyModel.setParentModelMatchRow(0);
  QCOMPARE(proxyModel.parentModelMatchRow(), -1);
  /*
   * Set parent model
   */
  VariantTableModel parentModel;
  parentModel.resize(3, 1);
  proxyModel.setParentModel(&parentModel);
  QCOMPARE(proxyModel.parentModelMatchRow(), -1);
  // Verify bound checking
  proxyModel.setParentModelMatchRow(5);
  QCOMPARE(proxyModel.parentModelMatchRow(), -1);
  // Set a valid row
  proxyModel.setParentModelMatchRow(1);
  QCOMPARE(proxyModel.parentModelMatchRow(), 1);
  /*
   * Set source model
   */
  VariantTableModel model;
  model.resize(2, 1);
  proxyModel.setSourceModel(&model);
  // Check that match row was keeped
  QCOMPARE(proxyModel.parentModelMatchRow(), 1);
  // Verify bound checking
  proxyModel.setParentModelMatchRow(5);
  QCOMPARE(proxyModel.parentModelMatchRow(), -1);
  /*
   * Change parent model match row
   */
  proxyModel.setParentModelMatchRow(2);
  QCOMPARE(proxyModel.parentModelMatchRow(), 2);
  proxyModel.setParentModelMatchRow(0);
  QCOMPARE(proxyModel.parentModelMatchRow(), 0);
  /*
   * Change parent model
   */
  VariantTableModel parentModel2;
  parentModel2.resize(3, 1);
  proxyModel.setParentModel(&parentModel2);
  QCOMPARE(proxyModel.parentModelMatchRow(), 0);
}

void RelationFilterProxyModelTest::filterTest()
{
  /*
   * Setup parent table model
   * ------
   * | Id |
   * ------
   * | 1  |
   * ------
   * | 2  |
   * ------
   */
  VariantTableModel parentModel;
  parentModel.resize(2, 1);
  parentModel.populateColumn(0, {1,2});
  /*
   * Setup (child) table model
   * -----------------
   * | Id | ParentId |
   * -----------------
   * | 11 |   1      |
   * -----------------
   * | 12 |   1      |
   * -----------------
   * | 21 |   2      |
   * -----------------
   */
  VariantTableModel model;
  model.resize(3, 2);
  model.populateColumn(0, {11,12,21});
  model.populateColumn(1, {1 ,1 ,2});
  /*
   * Setup proxy model
   */
  RelationFilterProxyModel proxyModel;
  proxyModel.setParentModel(&parentModel);
  proxyModel.setSourceModel(&model);
  proxyModel.setFilter( ChildModelColumn(1) == ParentModelColumn(0) );
  /*
   * Tests
   */
  // No row was set
  QCOMPARE(proxyModel.rowCount(), 0);
  // Match row 0 of parent model
  proxyModel.setParentModelMatchRow(0);
  QCOMPARE(proxyModel.rowCount(), 2);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(11));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant(12));
  // Match row 1 of parent model
  proxyModel.setParentModelMatchRow(1);
  QCOMPARE(proxyModel.rowCount(), 1);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(21));
  // Match row -1 of parent model
  proxyModel.setParentModelMatchRow(-1);
  QCOMPARE(proxyModel.rowCount(), 0);
}

void RelationFilterProxyModelTest::filterMultiColumnTest()
{
  /*
   * Setup parent table model
   * -------------
   * | Id1 | Id2 |
   * -------------
   * |  1  |  A  |
   * -------------
   * |  2  |  B  |
   * -------------
   */
  VariantTableModel parentModel;
  parentModel.resize(2, 2);
  parentModel.populateColumn(0, {1,2});
  parentModel.populateColumn(1, {"A","B"});
  /*
   * Setup (child) table model
   * ------------------------------
   * | Id | ParentId1 | ParentId2 |
   * ------------------------------
   * | 11 |     1     |     A     |
   * ------------------------------
   * | 12 |     1     |     A     |
   * ------------------------------
   * | 21 |     2     |     B     |
   * ------------------------------
   * | 99 |     2     |     A     |
   * ------------------------------
   */
  VariantTableModel model;
  model.resize(4, 3);
  model.populateColumn(0, {11,12,21,99});
  model.populateColumn(1, {1 ,1 ,2 ,2});
  model.populateColumn(2, {"A","A","B","A"});
  /*
   * Setup proxy model
   */
  RelationFilterProxyModel proxyModel;
  proxyModel.setParentModel(&parentModel);
  proxyModel.setSourceModel(&model);
  proxyModel.setFilter( ChildModelColumn(1) == ParentModelColumn(0) && ChildModelColumn(2) == ParentModelColumn(1) );
  /*
   * Tests
   */
  // No row was set
  QCOMPARE(proxyModel.rowCount(), 0);
  // Match row 0 of parent model
  proxyModel.setParentModelMatchRow(0);
  QCOMPARE(proxyModel.rowCount(), 2);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(11));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant(12));
  // Match row 1 of parent model
  proxyModel.setParentModelMatchRow(1);
  QCOMPARE(proxyModel.rowCount(), 1);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(21));
}

void RelationFilterProxyModelTest::filterBenchmark()
{
  QFETCH(int, n);
  /*
   * Setup parent table model
   * ------
   * | Id |
   * ------
   * | 1  |
   * ------
   * | 2  |
   * ------
   */
  VariantTableModel parentModel;
  parentModel.resize(2, 1);
  parentModel.populateColumn(0, {1,2});
  /*
   * Setup (child) table model
   * -----------------
   * | Id | ParentId |
   * -----------------
   * | 1  |   -1     |
   * -----------------
   * | 2  |    0     |
   * -----------------
   * | 3  |    1     |
   * -----------------
   * | 4  |    2     |
   * -----------------
   * | .. |    ..    |
   * -----------------
   */
  VariantTableModel model;
  model.resize(n, 2);
  model.populateColumnWithInt(0, 1);
  model.populateColumnWithInt(1, -1);
  /*
   * Setup proxy model
   */
  ParentModelColumn parentModelId(0);
  FilterColumn modelParentId(1);
  RelationFilterProxyModel proxyModel;
  proxyModel.setDynamicSortFilter(false);
  proxyModel.setParentModel(&parentModel);
  proxyModel.setSourceModel(&model);
  QBENCHMARK{
    proxyModel.setFilter(modelParentId == parentModelId);
    proxyModel.setParentModelMatchRow(0);
    proxyModel.setParentModelMatchRow(1);
  }
  QCOMPARE(proxyModel.rowCount(), 1);
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(2));
}

void RelationFilterProxyModelTest::filterBenchmark_data()
{
  QTest::addColumn<int>("n");

  QTest::newRow("10") << 10;
  QTest::newRow("100") << 100;
  QTest::newRow("1'000") << 1000;
  QTest::newRow("10'000") << 10000;
}

void RelationFilterProxyModelTest::filterGetCurrentSourceRowListTest()
{
  RowList list;
  /*
   * Setup parent table model
   * ------
   * | Id |
   * ------
   * | 1  |
   * ------
   * | 2  |
   * ------
   */
  VariantTableModel parentModel;
  parentModel.resize(2, 1);
  parentModel.populateColumn(0, {1,2});
  /*
   * Setup (child) table model
   * -----------------
   * | Id | ParentId |
   * -----------------
   * | 11 |   1      |
   * -----------------
   * | 12 |   1      |
   * -----------------
   * | 21 |   2      |
   * -----------------
   * | 13 |   1      |
   * -----------------
   */
  VariantTableModel model;
  model.resize(4, 2);
  model.populateColumn(0, {11,12,21,13});
  model.populateColumn(1, {1 ,1 ,2 ,1 });
  /*
   * Setup proxy model
   */
  ParentModelColumn parentModelId(0);
  FilterColumn modelParentId(1);
  RelationFilterProxyModel proxyModel;
  proxyModel.setParentModel(&parentModel);
  proxyModel.setSourceModel(&model);
  proxyModel.setFilter(modelParentId == parentModelId);
  /*
   * Check for parent Id 1
   */
  proxyModel.setParentModelMatchRow(0);
  QCOMPARE(proxyModel.rowCount(), 3);
  list = proxyModel.getCurrentSourceModelRowList();
  QCOMPARE(list.size(), 3);
  QCOMPARE(list.at(0), 0);
  QCOMPARE(list.at(1), 1);
  QCOMPARE(list.at(2), 3);
  /*
   * Check for parent Id 2
   */
  proxyModel.setParentModelMatchRow(1);
  QCOMPARE(proxyModel.rowCount(), 1);
  list = proxyModel.getCurrentSourceModelRowList();
  QCOMPARE(list.size(), 1);
  QCOMPARE(list.at(0), 2);
}

void RelationFilterProxyModelTest::filterRoleTest()
{
  QFAIL("Not complete");
}

void RelationFilterProxyModelTest::setterEventTest()
{
  QFAIL("Not complete");
}

void RelationFilterProxyModelTest::dynamicFilterTest()
{
  /*
   * Setup parent model
   * -------------
   * | Id | Name |
   * -------------
   * | 1  | C1   |
   * -------------
   * | 2  | C2   |
   * -------------
   */
  VariantTableModel clientModel;
  clientModel.resize(2, 2);
  clientModel.populateColumn(0, {1,2});
  clientModel.populateColumn(1, {"C1","C2"});
  /*
   * Setup source model
   * ------------------------
   * | Id | Cli_Id | Street |
   * ------------------------
   * | 11 |   1    |  S11   |
   * ------------------------
   * | 12 |   1    |  S12   |
   * ------------------------
   * | 25 |   2    |  S25   |
   * ------------------------
   */
  VariantTableModel addressModel;
  addressModel.resize(3, 3);
  addressModel.populateColumn(0, {11,12,25});
  addressModel.populateColumn(1, {1 ,1 ,2 });
  addressModel.populateColumn(2, {"S11","S12","S25"});
  /*
   * Setup proxy model
   */
  ParentModelColumn clientId(0);
  FilterColumn addressClientId(1);
  RelationFilterProxyModel proxyModel;
  proxyModel.setParentModel(&clientModel);
  proxyModel.setSourceModel(&addressModel);
  proxyModel.setFilter(addressClientId == clientId);
  proxyModel.setDynamicSortFilter(true);
  QCOMPARE(proxyModel.rowCount(), 0);
  /*
   * Filter on client 1:
   *  Address model              Proxy model
   * ------------------------   ------------------------
   * | Id | Cli_Id | Street |   | Id | Cli_Id | Street |
   * ------------------------   ------------------------
   * | 11 |   1    |  S11   |   | 11 |   1    |  S11   |
   * ------------------------   ------------------------
   * | 12 |   1    |  S12   |   | 12 |   1    |  S12   |
   * ------------------------   ------------------------
   * | 25 |   2    |  S25   |
   * ------------------------
   */
  proxyModel.setParentModelMatchRow(0);
  QCOMPARE(proxyModel.rowCount(), 2);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(11));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 0, 2), QVariant("S11"));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant(12));
  QCOMPARE(getModelData(proxyModel, 1, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 1, 2), QVariant("S12"));
  /*
   * Filter on client 2:
   *  Address model              Proxy model
   * ------------------------   ------------------------
   * | Id | Cli_Id | Street |   | Id | Cli_Id | Street |
   * ------------------------   ------------------------
   * | 11 |   1    |  S11   |   | 25 |   2    |  S25   |
   * ------------------------   ------------------------
   * | 12 |   1    |  S12   |
   * ------------------------
   * | 25 |   2    |  S25   |
   * ------------------------
   */
  proxyModel.setParentModelMatchRow(1);
  QCOMPARE(proxyModel.rowCount(), 1);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(25));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 0, 2), QVariant("S25"));
  /*
   * Set client row to -1
   */
  proxyModel.setParentModelMatchRow(-1);
  QCOMPARE(proxyModel.rowCount(), 0);
}

void RelationFilterProxyModelTest::dynamicFilterInsertTest()
{
  /*
   * Setup parent model
   * -------------
   * | Id | Name |
   * -------------
   * | 1  | C1   |
   * -------------
   * | 2  | C2   |
   * -------------
   */
  VariantTableModel clientModel;
  clientModel.resize(2, 2);
  clientModel.populateColumn(0, {1,2});
  clientModel.populateColumn(1, {"C1","C2"});
  /*
   * Setup source model
   * ------------------------
   * | Id | Cli_Id | Street |
   * ------------------------
   * | 11 |   1    |  S11   |
   * ------------------------
   * | 12 |   1    |  S12   |
   * ------------------------
   * | 25 |   2    |  S25   |
   * ------------------------
   */
  VariantTableModel addressModel;
  addressModel.resize(3, 3);
  addressModel.populateColumn(0, {11,12,25});
  addressModel.populateColumn(1, {1 ,1 ,2 });
  addressModel.populateColumn(2, {"S11","S12","S25"});
  /*
   * Setup proxy model
   */
  ParentModelColumn clientId(0);
  FilterColumn addressClientId(1);
  RelationFilterProxyModel proxyModel;
  proxyModel.setParentModel(&clientModel);
  proxyModel.setSourceModel(&addressModel);
  proxyModel.setFilter(addressClientId == clientId);
  proxyModel.setDynamicSortFilter(true);
  /*
   * Filter on client 2:
   *  Address model              Proxy model
   * ------------------------   ------------------------
   * | Id | Cli_Id | Street |   | Id | Cli_Id | Street |
   * ------------------------   ------------------------
   * | 11 |   1    |  S11   |   | 25 |   2    |  S25   |
   * ------------------------   ------------------------
   * | 12 |   1    |  S12   |
   * ------------------------
   * | 25 |   2    |  S25   |
   * ------------------------
   */
  proxyModel.setParentModelMatchRow(1);
  QCOMPARE(proxyModel.rowCount(), 1);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(25));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 0, 2), QVariant("S25"));
  /*
   * Insert into source model, at beginning
   */
  addressModel.prependRow();
  QCOMPARE(proxyModel.rowCount(), 1);
  /*
   * Edit inserted row
   * Proxy model must return this after edition was done:
   *  Address model              Proxy model
   * ------------------------   ------------------------
   * | Id | Cli_Id | Street |   | Id | Cli_Id | Street |
   * ------------------------   ------------------------
   * | 24 |   2    |  S24   |   | 24 |   2    |  S24   |
   * ------------------------   ------------------------
   * | 11 |   1    |  S11   |   | 25 |   2    |  S25   |
   * ------------------------   ------------------------
   * | 12 |   1    |  S12   |
   * ------------------------
   * | 25 |   2    |  S25   |
   * ------------------------
   */
  setModelData(addressModel, 0, 0, 24);
  setModelData(addressModel, 0, 1, 2);
  setModelData(addressModel, 0, 2, "S24");
  QCOMPARE(proxyModel.rowCount(), 2);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(24));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 0, 2), QVariant("S24"));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant(25));
  QCOMPARE(getModelData(proxyModel, 1, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 1, 2), QVariant("S25"));
  /*
   * Insert into source model, at end
   */
  addressModel.appendRow();
  QCOMPARE(proxyModel.rowCount(), 2);
  /*
   * Edit inserted row
   * Proxy model must return this after edition was done:
   *  Address model              Proxy model
   * ------------------------   ------------------------
   * | Id | Cli_Id | Street |   | Id | Cli_Id | Street |
   * ------------------------   ------------------------
   * | 24 |   2    |  S24   |   | 24 |   2    |  S24   |
   * ------------------------   ------------------------
   * | 11 |   1    |  S11   |   | 25 |   2    |  S25   |
   * ------------------------   ------------------------
   * | 12 |   1    |  S12   |   | 26 |   2    |  S26   |
   * ------------------------   ------------------------
   * | 25 |   2    |  S25   |
   * ------------------------
   * | 26 |   2    |  S26   |
   * ------------------------
   */
  setModelData(addressModel, 4, 0, 26);
  setModelData(addressModel, 4, 1, 2);
  setModelData(addressModel, 4, 2, "S26");
  QCOMPARE(proxyModel.rowCount(), 3);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(24));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 0, 2), QVariant("S24"));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant(25));
  QCOMPARE(getModelData(proxyModel, 1, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 1, 2), QVariant("S25"));
  QCOMPARE(getModelData(proxyModel, 2, 0), QVariant(26));
  QCOMPARE(getModelData(proxyModel, 2, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 2, 2), QVariant("S26"));
  /*
   * Insert into proxy model, at the beginning
   * Now, proxy model must copy client ID:
   *  Address model              Proxy model
   * ------------------------   ------------------------
   * | Id | Cli_Id | Street |   | Id | Cli_Id | Street |
   * ------------------------   ------------------------
   * |    |   2    |        |   |    |   2    |        |
   * ------------------------   ------------------------
   * | 24 |   2    |  S24   |   | 24 |   2    |  S24   |
   * ------------------------   ------------------------
   * | 11 |   1    |  S11   |   | 25 |   2    |  S25   |
   * ------------------------   ------------------------
   * | 12 |   1    |  S12   |   | 26 |   2    |  S26   |
   * ------------------------   ------------------------
   * | 25 |   2    |  S25   |
   * ------------------------
   * | 26 |   2    |  S26   |
   * ------------------------
   */
  QVERIFY(proxyModel.insertRow(0));
  QCOMPARE(proxyModel.rowCount(), 4);
  QVERIFY(getModelData(proxyModel, 0, 0).isNull());
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(2));
  QVERIFY(getModelData(proxyModel, 0, 2).isNull());
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant(24));
  QCOMPARE(getModelData(proxyModel, 1, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 1, 2), QVariant("S24"));
  QCOMPARE(getModelData(proxyModel, 2, 0), QVariant(25));
  QCOMPARE(getModelData(proxyModel, 2, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 2, 2), QVariant("S25"));
  QCOMPARE(getModelData(proxyModel, 3, 0), QVariant(26));
  QCOMPARE(getModelData(proxyModel, 3, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 3, 2), QVariant("S26"));
  /*
   * Edit inserted row
   * Proxy model must return this after edition was done:
   *  Address model              Proxy model
   * ------------------------   ------------------------
   * | Id | Cli_Id | Street |   | Id | Cli_Id | Street |
   * ------------------------   ------------------------
   * | 23 |   2    |  S23   |   | 23 |   2    |  S23   |
   * ------------------------   ------------------------
   * | 24 |   2    |  S24   |   | 24 |   2    |  S24   |
   * ------------------------   ------------------------
   * | 11 |   1    |  S11   |   | 25 |   2    |  S25   |
   * ------------------------   ------------------------
   * | 12 |   1    |  S12   |   | 26 |   2    |  S26   |
   * ------------------------   ------------------------
   * | 25 |   2    |  S25   |
   * ------------------------
   * | 26 |   2    |  S26   |
   * ------------------------
   */
  setModelData(proxyModel, 0, 0, 23);
  setModelData(proxyModel, 0, 2, "S23");
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(23));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 0, 2), QVariant("S23"));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant(24));
  QCOMPARE(getModelData(proxyModel, 1, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 1, 2), QVariant("S24"));
  QCOMPARE(getModelData(proxyModel, 2, 0), QVariant(25));
  QCOMPARE(getModelData(proxyModel, 2, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 2, 2), QVariant("S25"));
  QCOMPARE(getModelData(proxyModel, 3, 0), QVariant(26));
  QCOMPARE(getModelData(proxyModel, 3, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 3, 2), QVariant("S26"));
  /*
   * Insert into proxy model, at the end.
   * Now, proxy model must copy client ID:
   *  Address model              Proxy model
   * ------------------------   ------------------------
   * | Id | Cli_Id | Street |   | Id | Cli_Id | Street |
   * ------------------------   ------------------------
   * | 23 |   2    |  S23   |   | 23 |   2    |  S23   |
   * ------------------------   ------------------------
   * | 24 |   2    |  S24   |   | 24 |   2    |  S24   |
   * ------------------------   ------------------------
   * | 11 |   1    |  S11   |   | 25 |   2    |  S25   |
   * ------------------------   ------------------------
   * | 12 |   1    |  S12   |   | 26 |   2    |  S26   |
   * ------------------------   ------------------------
   * | 25 |   2    |  S25   |   |    |   2    |        |
   * ------------------------   ------------------------
   * | 26 |   2    |  S26   |
   * ------------------------
   * |    |   2    |        |
   * ------------------------
   */
  QVERIFY(proxyModel.insertRow(4));
  QCOMPARE(proxyModel.rowCount(), 5);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(23));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 0, 2), QVariant("S23"));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant(24));
  QCOMPARE(getModelData(proxyModel, 1, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 1, 2), QVariant("S24"));
  QCOMPARE(getModelData(proxyModel, 2, 0), QVariant(25));
  QCOMPARE(getModelData(proxyModel, 2, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 2, 2), QVariant("S25"));
  QCOMPARE(getModelData(proxyModel, 3, 0), QVariant(26));
  QCOMPARE(getModelData(proxyModel, 3, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 3, 2), QVariant("S26"));
  QVERIFY(getModelData(proxyModel, 4, 0).isNull());
  QCOMPARE(getModelData(proxyModel, 4, 1), QVariant(2));
  QVERIFY(getModelData(proxyModel, 4, 2).isNull());
  /*
   * Filter on client 1
   *  Address model              Proxy model
   * ------------------------   ------------------------
   * | Id | Cli_Id | Street |   | Id | Cli_Id | Street |
   * ------------------------   ------------------------
   * | 23 |   2    |  S23   |   | 11 |   1    |  S11   |
   * ------------------------   ------------------------
   * | 24 |   2    |  S24   |   | 12 |   1    |  S12   |
   * ------------------------   ------------------------
   * | 11 |   1    |  S11   |
   * ------------------------
   * | 12 |   1    |  S12   |
   * ------------------------
   * | 25 |   2    |  S25   |
   * ------------------------
   * | 26 |   2    |  S26   |
   * ------------------------
   * |    |   2    |        |
   * ------------------------
   */
  proxyModel.setParentModelMatchRow(0);
  QCOMPARE(proxyModel.rowCount(), 2);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(11));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 0, 2), QVariant("S11"));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant(12));
  QCOMPARE(getModelData(proxyModel, 1, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 1, 2), QVariant("S12"));
}

void RelationFilterProxyModelTest::dynamicFilterInsertMultiColumnTest()
{
  /*
   * Setup parent model
   * --------------------
   * | Id1 | Id2 | Name |
   * --------------------
   * |  1  |  A  |  C1  |
   * --------------------
   * |  2  |  B  |  C2  |
   * --------------------
   */
  VariantTableModel clientModel;
  clientModel.resize(2, 3);
  clientModel.populateColumn(0, {1,2});
  clientModel.populateColumn(1, {"A","B"});
  clientModel.populateColumn(2, {"C1","C2"});
  /*
   * Setup source model
   * -----------------------------------
   * | Id | Cli_Id1 | Cli_Id2 | Street |
   * -----------------------------------
   * | 11 |    1    |    A    |  S11   |
   * -----------------------------------
   * | 12 |    1    |    A    |  S12   |
   * -----------------------------------
   * | 25 |    2    |    B    |  S25   |
   * -----------------------------------
   */
  VariantTableModel addressModel;
  addressModel.resize(3, 4);
  addressModel.populateColumn(0, {11,12,25});
  addressModel.populateColumn(1, {1 ,1 ,2 });
  addressModel.populateColumn(2, {"A","A","B"});
  addressModel.populateColumn(3, {"S11","S12","S25"});
  /*
   * Setup proxy model
   */
  RelationFilterProxyModel proxyModel;
  proxyModel.setParentModel(&clientModel);
  proxyModel.setSourceModel(&addressModel);
  proxyModel.setFilter(ChildModelColumn(1) == ParentModelColumn(0) && ChildModelColumn(2) == ParentModelColumn(1));
  proxyModel.setDynamicSortFilter(true);
  /*
   * Filter on client 2:
   *  Address model                         Proxy model
   * -----------------------------------   -----------------------------------
   * | Id | Cli_Id1 | Cli_Id2 | Street |   | Id | Cli_Id1 | Cli_Id2 | Street |
   * -----------------------------------   -----------------------------------
   * | 11 |    1    |    A    |  S11   |   | 25 |    2    |    B    |  S25   |
   * -----------------------------------   -----------------------------------
   * | 12 |    1    |    A    |  S12   |
   * -----------------------------------
   * | 25 |    2    |    B    |  S25   |
   * -----------------------------------
   */
  proxyModel.setParentModelMatchRow(1);
  QCOMPARE(proxyModel.rowCount(), 1);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(25));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 0, 2), QVariant("B"));
  QCOMPARE(getModelData(proxyModel, 0, 3), QVariant("S25"));
  /*
   * Insert into source model, at beginning
   */
  addressModel.prependRow();
  QCOMPARE(proxyModel.rowCount(), 1);
  /*
   * Edit inserted row
   * Proxy model must return this after edition was done:
   *  Address model                         Proxy model
   * -----------------------------------   -----------------------------------
   * | Id | Cli_Id1 | Cli_Id2 | Street |   | Id | Cli_Id1 | Cli_Id2 | Street |
   * -----------------------------------   -----------------------------------
   * | 24 |    2    |    B    |  S24   |   | 24 |    2    |    B    |  S24   |
   * -----------------------------------   -----------------------------------
   * | 11 |    1    |    A    |  S11   |   | 25 |    2    |    B    |  S25   |
   * -----------------------------------   -----------------------------------
   * | 12 |    1    |    A    |  S12   |
   * -----------------------------------
   * | 25 |    2    |    B    |  S25   |
   * -----------------------------------
   */
  setModelData(addressModel, 0, 0, 24);
  setModelData(addressModel, 0, 1, 2);
  setModelData(addressModel, 0, 2, "B");
  setModelData(addressModel, 0, 3, "S24");
  QCOMPARE(proxyModel.rowCount(), 2);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(24));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 0, 2), QVariant("B"));
  QCOMPARE(getModelData(proxyModel, 0, 3), QVariant("S24"));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant(25));
  QCOMPARE(getModelData(proxyModel, 1, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 1, 2), QVariant("B"));
  QCOMPARE(getModelData(proxyModel, 1, 3), QVariant("S25"));
  /*
   * Insert into source model, at end
   */
  addressModel.appendRow();
  QCOMPARE(proxyModel.rowCount(), 2);
  /*
   * Edit inserted row
   * Proxy model must return this after edition was done:
   *  Address model                         Proxy model
   * -----------------------------------   -----------------------------------
   * | Id | Cli_Id1 | Cli_Id2 | Street |   | Id | Cli_Id1 | Cli_Id2 | Street |
   * -----------------------------------   -----------------------------------
   * | 24 |    2    |    B    |  S24   |   | 24 |    2    |    B    |  S24   |
   * -----------------------------------   -----------------------------------
   * | 11 |    1    |    A    |  S11   |   | 25 |    2    |    B    |  S25   |
   * -----------------------------------   -----------------------------------
   * | 12 |    1    |    A    |  S12   |   | 26 |    2    |    B    |  S26   |
   * -----------------------------------   -----------------------------------
   * | 25 |    2    |    B    |  S25   |
   * -----------------------------------
   * | 26 |    2    |    B    |  S26   |
   * -----------------------------------
   */
  setModelData(addressModel, 4, 0, 26);
  setModelData(addressModel, 4, 1, 2);
  setModelData(addressModel, 4, 2, "B");
  setModelData(addressModel, 4, 3, "S26");
  QCOMPARE(proxyModel.rowCount(), 3);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(24));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 0, 2), QVariant("B"));
  QCOMPARE(getModelData(proxyModel, 0, 3), QVariant("S24"));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant(25));
  QCOMPARE(getModelData(proxyModel, 1, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 1, 2), QVariant("B"));
  QCOMPARE(getModelData(proxyModel, 1, 3), QVariant("S25"));
  QCOMPARE(getModelData(proxyModel, 2, 0), QVariant(26));
  QCOMPARE(getModelData(proxyModel, 2, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 2, 2), QVariant("B"));
  QCOMPARE(getModelData(proxyModel, 2, 3), QVariant("S26"));
  /*
   * Insert into proxy model, at the beginning
   * Now, proxy model must copy client IDs:
   *  Address model                         Proxy model
   * -----------------------------------   -----------------------------------
   * | Id | Cli_Id1 | Cli_Id2 | Street |   | Id | Cli_Id1 | Cli_Id2 | Street |
   * -----------------------------------   -----------------------------------
   * |    |    2    |    B    |        |   |    |    2    |    B    |        |
   * -----------------------------------   -----------------------------------
   * | 24 |    2    |    B    |  S24   |   | 24 |    2    |    B    |  S24   |
   * -----------------------------------   -----------------------------------
   * | 11 |    1    |    A    |  S11   |   | 25 |    2    |    B    |  S25   |
   * -----------------------------------   -----------------------------------
   * | 12 |    1    |    A    |  S12   |   | 26 |    2    |    B    |  S26   |
   * -----------------------------------   -----------------------------------
   * | 25 |    2    |    B    |  S25   |
   * -----------------------------------
   * | 26 |    2    |    B    |  S26   |
   * -----------------------------------
   */
  QVERIFY(proxyModel.insertRow(0));
  QCOMPARE(proxyModel.rowCount(), 4);
  QVERIFY(getModelData(proxyModel, 0, 0).isNull());
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 0, 2), QVariant("B"));
  QVERIFY(getModelData(proxyModel, 0, 3).isNull());
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant(24));
  QCOMPARE(getModelData(proxyModel, 1, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 1, 2), QVariant("B"));
  QCOMPARE(getModelData(proxyModel, 1, 3), QVariant("S24"));
  QCOMPARE(getModelData(proxyModel, 2, 0), QVariant(25));
  QCOMPARE(getModelData(proxyModel, 2, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 2, 2), QVariant("B"));
  QCOMPARE(getModelData(proxyModel, 2, 3), QVariant("S25"));
  QCOMPARE(getModelData(proxyModel, 3, 0), QVariant(26));
  QCOMPARE(getModelData(proxyModel, 3, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 3, 2), QVariant("B"));
  QCOMPARE(getModelData(proxyModel, 3, 3), QVariant("S26"));
  /*
   * Edit inserted row
   * Proxy model must return this after edition was done:
   *  Address model                         Proxy model
   * -----------------------------------   -----------------------------------
   * | Id | Cli_Id1 | Cli_Id2 | Street |   | Id | Cli_Id1 | Cli_Id2 | Street |
   * -----------------------------------   -----------------------------------
   * | 23 |    2    |    B    |  S23   |   | 23 |    2    |    B    |  S23   |
   * -----------------------------------   -----------------------------------
   * | 24 |    2    |    B    |  S24   |   | 24 |    2    |    B    |  S24   |
   * -----------------------------------   -----------------------------------
   * | 11 |    1    |    A    |  S11   |   | 25 |    2    |    B    |  S25   |
   * -----------------------------------   -----------------------------------
   * | 12 |    1    |    A    |  S12   |   | 26 |    2    |    B    |  S26   |
   * -----------------------------------   -----------------------------------
   * | 25 |    2    |    B    |  S25   |
   * -----------------------------------
   * | 26 |    2    |    B    |  S26   |
   * -----------------------------------
   */
  setModelData(proxyModel, 0, 0, 23);
  setModelData(proxyModel, 0, 3, "S23");
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(23));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 0, 2), QVariant("B"));
  QCOMPARE(getModelData(proxyModel, 0, 3), QVariant("S23"));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant(24));
  QCOMPARE(getModelData(proxyModel, 1, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 1, 2), QVariant("B"));
  QCOMPARE(getModelData(proxyModel, 1, 3), QVariant("S24"));
  QCOMPARE(getModelData(proxyModel, 2, 0), QVariant(25));
  QCOMPARE(getModelData(proxyModel, 2, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 2, 2), QVariant("B"));
  QCOMPARE(getModelData(proxyModel, 2, 3), QVariant("S25"));
  QCOMPARE(getModelData(proxyModel, 3, 0), QVariant(26));
  QCOMPARE(getModelData(proxyModel, 3, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 3, 2), QVariant("B"));
  QCOMPARE(getModelData(proxyModel, 3, 3), QVariant("S26"));
  /*
   * Insert into proxy model, at the end.
   * Now, proxy model must copy client IDs:
   *  Address model                         Proxy model
   * -----------------------------------   -----------------------------------
   * | Id | Cli_Id1 | Cli_Id2 | Street |   | Id | Cli_Id1 | Cli_Id2 | Street |
   * -----------------------------------   -----------------------------------
   * | 23 |    2    |    B    |  S23   |   | 23 |    2    |    B    |  S23   |
   * -----------------------------------   -----------------------------------
   * | 24 |    2    |    B    |  S24   |   | 24 |    2    |    B    |  S24   |
   * -----------------------------------   -----------------------------------
   * | 11 |    1    |    A    |  S11   |   | 25 |    2    |    B    |  S25   |
   * -----------------------------------   -----------------------------------
   * | 12 |    1    |    A    |  S12   |   | 26 |    2    |    B    |  S26   |
   * -----------------------------------   -----------------------------------
   * | 25 |    2    |    B    |  S25   |   |    |    2    |    B    |        |
   * -----------------------------------   -----------------------------------
   * | 26 |    2    |    B    |  S26   |
   * -----------------------------------
   * |    |    2    |    B    |        |
   * -----------------------------------
   */
  QVERIFY(proxyModel.insertRow(4));
  QCOMPARE(proxyModel.rowCount(), 5);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(23));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 0, 2), QVariant("B"));
  QCOMPARE(getModelData(proxyModel, 0, 3), QVariant("S23"));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant(24));
  QCOMPARE(getModelData(proxyModel, 1, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 1, 2), QVariant("B"));
  QCOMPARE(getModelData(proxyModel, 1, 3), QVariant("S24"));
  QCOMPARE(getModelData(proxyModel, 2, 0), QVariant(25));
  QCOMPARE(getModelData(proxyModel, 2, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 2, 2), QVariant("B"));
  QCOMPARE(getModelData(proxyModel, 2, 3), QVariant("S25"));
  QCOMPARE(getModelData(proxyModel, 3, 0), QVariant(26));
  QCOMPARE(getModelData(proxyModel, 3, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 3, 2), QVariant("B"));
  QCOMPARE(getModelData(proxyModel, 3, 3), QVariant("S26"));
  QVERIFY(getModelData(proxyModel, 4, 0).isNull());
  QCOMPARE(getModelData(proxyModel, 4, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 4, 2), QVariant("B"));
  QVERIFY(getModelData(proxyModel, 4, 3).isNull());
  /*
   * Filter on client 1
   *  Address model                         Proxy model
   * -----------------------------------   -----------------------------------
   * | Id | Cli_Id1 | Cli_Id2 | Street |   | Id | Cli_Id1 | Cli_Id2 | Street |
   * -----------------------------------   -----------------------------------
   * | 23 |    2    |    B    |  S23   |   | 11 |    1    |    A    |  S11   |
   * -----------------------------------   -----------------------------------
   * | 24 |    2    |    B    |  S24   |   | 12 |    1    |    A    |  S12   |
   * -----------------------------------   -----------------------------------
   * | 11 |    1    |    A    |  S11   |
   * -----------------------------------
   * | 12 |    1    |    A    |  S12   |
   * -----------------------------------
   * | 25 |    2    |    B    |  S25   |
   * -----------------------------------
   * | 26 |    2    |    B    |  S26   |
   * -----------------------------------
   * |    |    2    |    B    |        |
   * -----------------------------------
   */
  proxyModel.setParentModelMatchRow(0);
  QCOMPARE(proxyModel.rowCount(), 2);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(11));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 0, 2), QVariant("A"));
  QCOMPARE(getModelData(proxyModel, 0, 3), QVariant("S11"));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant(12));
  QCOMPARE(getModelData(proxyModel, 1, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 1, 2), QVariant("A"));
  QCOMPARE(getModelData(proxyModel, 1, 3), QVariant("S12"));
}

void RelationFilterProxyModelTest::parentModelKeyChangeTest()
{
  /*
   * Setup parent model
   * -------------
   * | Id | Name |
   * -------------
   * | 1  | C1   |
   * -------------
   * | 2  | C2   |
   * -------------
   */
  VariantTableModel clientModel;
  clientModel.resize(2, 2);
  clientModel.populateColumn(0, {1,2});
  clientModel.populateColumn(1, {"C1","C2"});
  /*
   * Setup source model
   * ------------------------
   * | Id | Cli_Id | Street |
   * ------------------------
   * | 11 |   1    |  S11   |
   * ------------------------
   * | 12 |   1    |  S12   |
   * ------------------------
   * | 25 |   2    |  S25   |
   * ------------------------
   */
  VariantTableModel addressModel;
  addressModel.resize(3, 3);
  addressModel.populateColumn(0, {11,12,25});
  addressModel.populateColumn(1, {1 ,1 ,2 });
  addressModel.populateColumn(2, {"S11","S12","S25"});
  /*
   * Setup proxy model
   */
  ParentModelColumn clientId(0);
  FilterColumn addressClientId(1);
  RelationFilterProxyModel proxyModel;
  proxyModel.setParentModel(&clientModel);
  proxyModel.setSourceModel(&addressModel);
  proxyModel.setFilter(addressClientId == clientId);
  proxyModel.setDynamicSortFilter(true);
  /*
   * Filter on client 2:
   *  Address model              Proxy model
   * ------------------------   ------------------------
   * | Id | Cli_Id | Street |   | Id | Cli_Id | Street |
   * ------------------------   ------------------------
   * | 11 |   1    |  S11   |   | 25 |   2    |  S25   |
   * ------------------------   ------------------------
   * | 12 |   1    |  S12   |
   * ------------------------
   * | 25 |   2    |  S25   |
   * ------------------------
   */
  proxyModel.setParentModelMatchRow(1);
  QCOMPARE(proxyModel.rowCount(), 1);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(25));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 0, 2), QVariant("S25"));
  /*
   * Edit ID of client 2 -> set to 3
   * Proxy model must update Cli_Id in address model:
   *  Address model              Proxy model
   * ------------------------   ------------------------
   * | Id | Cli_Id | Street |   | Id | Cli_Id | Street |
   * ------------------------   ------------------------
   * | 11 |   1    |  S11   |   | 25 |   3    |  S25   |
   * ------------------------   ------------------------
   * | 12 |   1    |  S12   |
   * ------------------------
   * | 25 |   3    |  S25   |
   * ------------------------
   */
  // Edit client model
  QVERIFY(setModelData(clientModel,1, 0, 3));
  // Check address model
  QCOMPARE(addressModel.rowCount(), 3);
  QCOMPARE(getModelData(addressModel, 0, 0), QVariant(11));
  QCOMPARE(getModelData(addressModel, 0, 1), QVariant(1));
  QCOMPARE(getModelData(addressModel, 0, 2), QVariant("S11"));
  QCOMPARE(getModelData(addressModel, 1, 0), QVariant(12));
  QCOMPARE(getModelData(addressModel, 1, 1), QVariant(1));
  QCOMPARE(getModelData(addressModel, 1, 2), QVariant("S12"));
  QCOMPARE(getModelData(addressModel, 2, 0), QVariant(25));
  QCOMPARE(getModelData(addressModel, 2, 1), QVariant(3));
  QCOMPARE(getModelData(addressModel, 2, 2), QVariant("S25"));
  // Check proxy model
  QCOMPARE(proxyModel.rowCount(), 1);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(25));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(3));
  QCOMPARE(getModelData(proxyModel, 0, 2), QVariant("S25"));
  /*
   * Edit ID of client 2 -> set to 1
   * Proxy model must update Cli_Id in address model:
   *  Address model              Proxy model
   * ------------------------   ------------------------
   * | Id | Cli_Id | Street |   | Id | Cli_Id | Street |
   * ------------------------   ------------------------
   * | 11 |   1    |  S11   |   | 11 |   1    |  S11   |
   * ------------------------   ------------------------
   * | 12 |   1    |  S12   |   | 12 |   1    |  S12   |
   * ------------------------   ------------------------
   * | 25 |   1    |  S25   |   | 25 |   1    |  S25   |
   * ------------------------   ------------------------
   */
  // Edit client model
  QVERIFY(setModelData(clientModel,1, 0, 1));
  // Check address model
  QCOMPARE(addressModel.rowCount(), 3);
  QCOMPARE(getModelData(addressModel, 0, 0), QVariant(11));
  QCOMPARE(getModelData(addressModel, 0, 1), QVariant(1));
  QCOMPARE(getModelData(addressModel, 0, 2), QVariant("S11"));
  QCOMPARE(getModelData(addressModel, 1, 0), QVariant(12));
  QCOMPARE(getModelData(addressModel, 1, 1), QVariant(1));
  QCOMPARE(getModelData(addressModel, 1, 2), QVariant("S12"));
  QCOMPARE(getModelData(addressModel, 2, 0), QVariant(25));
  QCOMPARE(getModelData(addressModel, 2, 1), QVariant(1));
  QCOMPARE(getModelData(addressModel, 2, 2), QVariant("S25"));
  // Check proxy model
  QCOMPARE(proxyModel.rowCount(), 3);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(11));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 0, 2), QVariant("S11"));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant(12));
  QCOMPARE(getModelData(proxyModel, 1, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 1, 2), QVariant("S12"));
  QCOMPARE(getModelData(proxyModel, 2, 0), QVariant(25));
  QCOMPARE(getModelData(proxyModel, 2, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 2, 2), QVariant("S25"));
}

void RelationFilterProxyModelTest::parentModelKeyMultiColumnKeyChangeTest()
{
  /*
   * Setup parent model
   * --------------------
   * | Id1 | Id2 | Name |
   * --------------------
   * |  1  |  A  |  C1  |
   * --------------------
   * |  2  |  B  |  C2  |
   * --------------------
   */
  VariantTableModel clientModel;
  clientModel.resize(2, 3);
  clientModel.populateColumn(0, {1,2});
  clientModel.populateColumn(1, {"A","B"});
  clientModel.populateColumn(2, {"C1","C2"});
  /*
   * Setup source model
   * -----------------------------------
   * | Id | Cli_Id1 | Cli_Id2 | Street |
   * -----------------------------------
   * | 11 |    1    |    A    |  S11   |
   * -----------------------------------
   * | 12 |    1    |    A    |  S12   |
   * -----------------------------------
   * | 25 |    2    |    B    |  S25   |
   * -----------------------------------
   */
  VariantTableModel addressModel;
  addressModel.resize(3, 4);
  addressModel.populateColumn(0, {11,12,25});
  addressModel.populateColumn(1, {1 ,1 ,2 });
  addressModel.populateColumn(2, {"A","A","B"});
  addressModel.populateColumn(3, {"S11","S12","S25"});
  /*
   * Setup proxy model
   */
  RelationFilterProxyModel proxyModel;
  proxyModel.setParentModel(&clientModel);
  proxyModel.setSourceModel(&addressModel);
  proxyModel.setFilter(ChildModelColumn(1) == ParentModelColumn(0) && ChildModelColumn(2) == ParentModelColumn(1));
  proxyModel.setDynamicSortFilter(true);
  /*
   * Filter on client 2:
   *  Address model                         Proxy model
   * -----------------------------------   -----------------------------------
   * | Id | Cli_Id1 | Cli_Id2 | Street |   | Id | Cli_Id1 | Cli_Id2 | Street |
   * -----------------------------------   -----------------------------------
   * | 11 |    1    |    A    |  S11   |   | 25 |    2    |    B    |  S25   |
   * -----------------------------------   -----------------------------------
   * | 12 |    1    |    A    |  S12   |
   * -----------------------------------
   * | 25 |    2    |    B    |  S25   |
   * -----------------------------------
   */
  proxyModel.setParentModelMatchRow(1);
  QCOMPARE(proxyModel.rowCount(), 1);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(25));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 0, 2), QVariant("B"));
  QCOMPARE(getModelData(proxyModel, 0, 3), QVariant("S25"));
  /*
   * Edit ID of client 2 -> set to 4,D
   * Proxy model must update Cli_Id1 and Cli_Id2 in address model:
   *  Address model                         Proxy model
   * -----------------------------------   -----------------------------------
   * | Id | Cli_Id1 | Cli_Id2 | Street |   | Id | Cli_Id1 | Cli_Id2 | Street |
   * -----------------------------------   -----------------------------------
   * | 11 |    1    |    A    |  S11   |   | 25 |    4    |    D    |  S25   |
   * -----------------------------------   -----------------------------------
   * | 12 |    1    |    A    |  S12   |
   * -----------------------------------
   * | 25 |    4    |    D    |  S25   |
   * -----------------------------------
   */
  // Edit client model
  QVERIFY(setModelData(clientModel, 1, 0, 4));
  QVERIFY(setModelData(clientModel, 1, 1, "D"));
  // Check address model
  QCOMPARE(addressModel.rowCount(), 3);
  QCOMPARE(getModelData(addressModel, 0, 0), QVariant(11));
  QCOMPARE(getModelData(addressModel, 0, 1), QVariant(1));
  QCOMPARE(getModelData(addressModel, 0, 2), QVariant("A"));
  QCOMPARE(getModelData(addressModel, 0, 3), QVariant("S11"));
  QCOMPARE(getModelData(addressModel, 1, 0), QVariant(12));
  QCOMPARE(getModelData(addressModel, 1, 1), QVariant(1));
  QCOMPARE(getModelData(addressModel, 1, 2), QVariant("A"));
  QCOMPARE(getModelData(addressModel, 1, 3), QVariant("S12"));
  QCOMPARE(getModelData(addressModel, 2, 0), QVariant(25));
  QCOMPARE(getModelData(addressModel, 2, 1), QVariant(4));
  QCOMPARE(getModelData(addressModel, 2, 2), QVariant("D"));
  QCOMPARE(getModelData(addressModel, 2, 3), QVariant("S25"));
  // Check proxy model
  QCOMPARE(proxyModel.rowCount(), 1);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(25));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(4));
  QCOMPARE(getModelData(proxyModel, 0, 2), QVariant("D"));
  QCOMPARE(getModelData(proxyModel, 0, 3), QVariant("S25"));
  /*
   * Edit ID of client 2 -> set to 1,A
   * Proxy model must update Cli_Id1 and Cli_Id2 in address model:
   *  Address model                         Proxy model
   * -----------------------------------   -----------------------------------
   * | Id | Cli_Id1 | Cli_Id2 | Street |   | Id | Cli_Id1 | Cli_Id2 | Street |
   * -----------------------------------   -----------------------------------
   * | 11 |    1    |    A    |  S11   |   | 11 |    1    |    A    |  S11   |
   * -----------------------------------   -----------------------------------
   * | 12 |    1    |    A    |  S12   |   | 12 |    1    |    A    |  S12   |
   * -----------------------------------   -----------------------------------
   * | 25 |    1    |    A    |  S25   |   | 25 |    1    |    A    |  S25   |
   * -----------------------------------   -----------------------------------
   */
  // Edit client model
  QVERIFY(setModelData(clientModel, 1, 0, 1));
  QVERIFY(setModelData(clientModel, 1, 1, "A"));
  // Check address model
  QCOMPARE(addressModel.rowCount(), 3);
  QCOMPARE(getModelData(addressModel, 0, 0), QVariant(11));
  QCOMPARE(getModelData(addressModel, 0, 1), QVariant(1));
  QCOMPARE(getModelData(addressModel, 0, 2), QVariant("A"));
  QCOMPARE(getModelData(addressModel, 0, 3), QVariant("S11"));
  QCOMPARE(getModelData(addressModel, 1, 0), QVariant(12));
  QCOMPARE(getModelData(addressModel, 1, 1), QVariant(1));
  QCOMPARE(getModelData(addressModel, 1, 2), QVariant("A"));
  QCOMPARE(getModelData(addressModel, 1, 3), QVariant("S12"));
  QCOMPARE(getModelData(addressModel, 2, 0), QVariant(25));
  QCOMPARE(getModelData(addressModel, 2, 1), QVariant(1));
  QCOMPARE(getModelData(addressModel, 2, 2), QVariant("A"));
  QCOMPARE(getModelData(addressModel, 2, 3), QVariant("S25"));
  // Check proxy model
  QCOMPARE(proxyModel.rowCount(), 3);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(11));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 0, 2), QVariant("A"));
  QCOMPARE(getModelData(proxyModel, 0, 3), QVariant("S11"));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant(12));
  QCOMPARE(getModelData(proxyModel, 1, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 1, 2), QVariant("A"));
  QCOMPARE(getModelData(proxyModel, 1, 3), QVariant("S12"));
  QCOMPARE(getModelData(proxyModel, 2, 0), QVariant(25));
  QCOMPARE(getModelData(proxyModel, 2, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 2, 2), QVariant("A"));
  QCOMPARE(getModelData(proxyModel, 2, 3), QVariant("S25"));
}

void RelationFilterProxyModelTest::parentModelKeyChangeRolesTest()
{
  /*
   * Setup parent model
   * -------------
   * | Id | Name |
   * -------------
   * | 1  | C1   |
   * -------------
   */
  VariantTableModel clientModel;
  clientModel.resize(1, 2);
  clientModel.populateColumn(0, {1});
  clientModel.populateColumn(1, {"C1"});
  FormatProxyModel clientFormatModel;
  clientFormatModel.setSourceModel(&clientModel);
  /*
   * Setup source model
   * ------------------------
   * | Id | Cli_Id | Street |
   * ------------------------
   * | 11 |   1    |   A    |
   * ------------------------
   */
  VariantTableModel addressModel;
  addressModel.resize(1, 3);
  addressModel.populateColumn(0, {11});
  addressModel.populateColumn(1, {1});
  addressModel.populateColumn(2, {"A"});
  /*
   * Setup proxy model
   */
  ParentModelColumn clientId(0);
  FilterColumn addressClientId(1);
  RelationFilterProxyModel proxyModel;
  proxyModel.setParentModel(&clientFormatModel);
  proxyModel.setSourceModel(&addressModel);
  proxyModel.setFilter(addressClientId == clientId);
  proxyModel.setDynamicSortFilter(true);
  /*
   * Setup signal spy
   */
  QSignalSpy dataChangedSpy(&proxyModel, &RelationFilterProxyModel::dataChanged);
  QVERIFY(dataChangedSpy.isValid());
  /*
   * Filter on client 1:
   *  Address model              Proxy model
   * ------------------------   ------------------------
   * | Id | Cli_Id | Street |   | Id | Cli_Id | Street |
   * ------------------------   ------------------------
   * | 11 |   1    |   A    |   | 11 |   1    |   A    |
   * ------------------------   ------------------------
   */
  proxyModel.setParentModelMatchRow(0);
  QCOMPARE(proxyModel.rowCount(), 1);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(11));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 0, 2), QVariant("A"));
  dataChangedSpy.clear();
  /*
   * Set a format for client ID (must do nothing)
   *  Address model              Proxy model
   * ------------------------   ------------------------
   * | Id | Cli_Id | Street |   | Id | Cli_Id | Street |
   * ------------------------   ------------------------
   * | 11 |   1    |   A    |   | 11 |   1    |   A    |
   * ------------------------   ------------------------
   */
  clientFormatModel.setTextAlignmentForColumn(0, Qt::AlignCenter);
  QCOMPARE(proxyModel.rowCount(), 1);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(11));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 0, 2), QVariant("A"));
  QCOMPARE(dataChangedSpy.count(), 0);
  /*
   * Check changing Edit role - model pass it in dataChanged
   *  Address model              Proxy model
   * ------------------------   ------------------------
   * | Id | Cli_Id | Street |   | Id | Cli_Id | Street |
   * ------------------------   ------------------------
   * | 11 |   2    |   A    |   | 11 |   2    |   A    |
   * ------------------------   ------------------------
   */
  clientModel.setData(0, 0, 2);
  QCOMPARE(proxyModel.rowCount(), 1);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(11));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 0, 2), QVariant("A"));
  QCOMPARE(dataChangedSpy.count(), 1);
  /*
   * Check changing edit role - model not pass in dataChanged
   *  Address model              Proxy model
   * ------------------------   ------------------------
   * | Id | Cli_Id | Street |   | Id | Cli_Id | Street |
   * ------------------------   ------------------------
   * | 11 |   3    |   A    |   | 11 |   3    |   A    |
   * ------------------------   ------------------------
   */
  clientModel.setPassRolesInDataChaged(false);
  clientModel.setData(0, 0, 3);
  QCOMPARE(proxyModel.rowCount(), 1);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(11));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(3));
  QCOMPARE(getModelData(proxyModel, 0, 2), QVariant("A"));
}

void RelationFilterProxyModelTest::qtModelTest()
{
  /*
   * Setup parent model
   * -------------
   * | Id | Name |
   * -------------
   * | 1  | C1   |
   * -------------
   * | 2  | C2   |
   * -------------
   */
  VariantTableModel clientModel;
  clientModel.resize(2, 2);
  clientModel.populateColumn(0, {1,2});
  clientModel.populateColumn(1, {"C1","C2"});
  /*
   * Setup source model
   * ------------------------
   * | Id | Cli_Id | Street |
   * ------------------------
   * | 11 |   1    |  S11   |
   * ------------------------
   * | 12 |   1    |  S12   |
   * ------------------------
   * | 25 |   2    |  S25   |
   * ------------------------
   */
  VariantTableModel addressModel;
  addressModel.resize(3, 3);
  addressModel.populateColumn(0, {11,12,25});
  addressModel.populateColumn(1, {1 ,1 ,2 });
  addressModel.populateColumn(2, {"S11","S12","S25"});
  /*
   * Setup proxy model
   */
  ParentModelColumn clientId(0);
  FilterColumn addressClientId(1);
  RelationFilterProxyModel proxyModel;
  proxyModel.setParentModel(&clientModel);
  proxyModel.setSourceModel(&addressModel);
  proxyModel.setFilter(addressClientId == clientId);
  proxyModel.setDynamicSortFilter(true);
  proxyModel.setParentModelMatchRow(0);
  /*
   * Test
   */
  QCOMPARE(proxyModel.rowCount(), 2);
  QtModelTest mt(&proxyModel);
  QtModelTest smt(&addressModel);
  QtModelTest pmt(&clientModel);
}

/*
 * Main
 */
MDT_ITEM_MODEL_TEST_MAIN(RelationFilterProxyModelTest)
// MDT_ITEM_MODEL_TEST_MAIN_DEBUG_ENV(RelationFilterProxyModelTest)

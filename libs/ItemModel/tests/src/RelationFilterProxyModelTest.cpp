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
#include "Mdt/ItemModel/RelationFilterProxyModel.h"
#include "Mdt/ItemModel/VariantTableModel.h"
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

namespace ItemModel = Mdt::ItemModel;
using ItemModel::VariantTableModel;
using ItemModel::RelationFilterProxyModel;
using ItemModel::FilterColumn;
using ItemModel::ParentModelColumn;
using Like = ItemModel::LikeExpression;

void RelationFilterProxyModelTest::initTestCase()
{
}

void RelationFilterProxyModelTest::cleanupTestCase()
{
}

/*
 * Tests
 */

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
  /*
   * Set parent model
   */
  VariantTableModel parentModel;
  parentModel.resize(3, 1);
  proxyModel.setParentModel(&parentModel);
  QCOMPARE(proxyModel.parentModelMatchRow(), -1);
  proxyModel.setParentModelMatchRow(1);
  QCOMPARE(proxyModel.parentModelMatchRow(), 1);
  /*
   * Set source model
   */
  VariantTableModel model;
  model.resize(2, 1);
  proxyModel.setSourceModel(&model);
  QCOMPARE(proxyModel.parentModelMatchRow(), 1);
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
  QCOMPARE(proxyModel.parentModelMatchRow(), -1);
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
  ParentModelColumn parentModelId(0);
  FilterColumn modelParentId(1);
  RelationFilterProxyModel proxyModel;
  proxyModel.setParentModel(&parentModel);
  proxyModel.setSourceModel(&model);
  proxyModel.setFilter(modelParentId == parentModelId);
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

void RelationFilterProxyModelTest::filterRoleTest()
{
  QFAIL("Not complete");
}

void RelationFilterProxyModelTest::setModelTest()
{
  QFAIL("Not complete");
}

void RelationFilterProxyModelTest::setterEventTest()
{
  QFAIL("Not complete");
}

void RelationFilterProxyModelTest::dynamicFilterTest()
{
  QFAIL("Not complete");
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
  
  displayModels(addressModel, proxyModel);
  
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
   * Note: when calling proxyModel.insertRow(4),
   *       QSortFilterProxyModel will insert before row 4 in source model
   *       (Experienced in Qt 5.5.1 Linux).
   * Now, proxy model must copy client ID:
   *  Address model              Proxy model
   * ------------------------   ------------------------
   * | Id | Cli_Id | Street |   | Id | Cli_Id | Street |
   * ------------------------   ------------------------
   * | 23 |   2    |  S23   |   | 23 |   2    |  S23   |
   * ------------------------   ------------------------
   * | 24 |   2    |  S24   |   | 24 |   2    |  S24   |
   * ------------------------   ------------------------
   * | 11 |   1    |  S11   |   |    |   2    |        |
   * ------------------------   ------------------------
   * | 12 |   1    |  S12   |   | 25 |   2    |  S25   |
   * ------------------------   ------------------------
   * |    |   2    |        |   | 26 |   2    |  S26   |
   * ------------------------   ------------------------
   * | 25 |   2    |  S25   |
   * ------------------------
   * | 26 |   2    |  S26   |
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
  QVERIFY(getModelData(proxyModel, 2, 0).isNull());
  QCOMPARE(getModelData(proxyModel, 2, 1), QVariant(2));
  QVERIFY(getModelData(proxyModel, 2, 2).isNull());
  QCOMPARE(getModelData(proxyModel, 3, 0), QVariant(25));
  QCOMPARE(getModelData(proxyModel, 3, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 3, 2), QVariant("S25"));
  QCOMPARE(getModelData(proxyModel, 4, 0), QVariant(26));
  QCOMPARE(getModelData(proxyModel, 4, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 4, 2), QVariant("S26"));
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
   * |    |   2    |        |
   * ------------------------
   * | 25 |   2    |  S25   |
   * ------------------------
   * | 26 |   2    |  S26   |
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

/*
 * Main
 */
MDT_ITEM_MODEL_TEST_MAIN(RelationFilterProxyModelTest)
// MDT_ITEM_MODEL_TEST_MAIN_DEBUG_ENV(RelationFilterProxyModelTest)

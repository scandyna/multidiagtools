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
#include "Mdt/Application.h"
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
#include <iterator>
#include <vector>
#include <array>
#include <algorithm>

#include <QDebug>

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
   * | 1  |   1      |
   * -----------------
   * | 2  |   1      |
   * -----------------
   * | .. |   2      |
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

/*
 * Helper
 */

QVariant RelationFilterProxyModelTest::getModelData(const QAbstractItemModel& model, int row, int column)
{
  auto index = model.index(row, column);
  Q_ASSERT(index.isValid());
  return model.data(index);
}

void RelationFilterProxyModelTest::displayModels(QAbstractItemModel* sourceModel, QSortFilterProxyModel* proxyModel)
{
  Q_ASSERT(sourceModel != nullptr);
  Q_ASSERT(proxyModel != nullptr);

  QTableView sourceView;
  QTableView proxyView;

  sourceView.setModel(sourceModel);
  sourceView.setWindowTitle("Source");
  proxyView.setModel(proxyModel);
  proxyView.setWindowTitle("Proxy");
  sourceView.show();
  proxyView.show();
  while(sourceView.isVisible()){
    QTest::qWait(500);
  }
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  RelationFilterProxyModelTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}

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
#include "SortProxyModelTest.h"
#include "Mdt/Application.h"
#include "Mdt/ItemModel/SortProxyModel.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include "Mdt/ItemModel/ColumnSortOrderList.h"
#include "Mdt/ItemModel/ColumnSortStringAttributesList.h"
#include <QStringList>
#include <QModelIndex>
#include <QTableView>
#include <QTreeView>
#include <QListView>
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
using ItemModel::VariantTableModelStorageRule;
using ItemModel::SortProxyModel;
using ItemModel::StringNumericMode;

void SortProxyModelTest::initTestCase()
{
}

void SortProxyModelTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void SortProxyModelTest::columnSortOrderTest()
{
  using ItemModel::ColumnSortOrder;

  ColumnSortOrder cso(1, Qt::DescendingOrder);
  QCOMPARE(cso.column(), 1);
  QVERIFY(cso.sortOrder() == Qt::DescendingOrder);
}

void SortProxyModelTest::columnSortOrderListTest()
{
  using ItemModel::ColumnSortOrder;

  ItemModel::ColumnSortOrderList list;
  std::vector<ColumnSortOrder> resultList;

  QVERIFY(list.isEmpty());
  list.addColumn(1, Qt::AscendingOrder);
  QVERIFY(!list.isEmpty());
  list.addColumn(2, Qt::DescendingOrder);
  list.addColumn(3, Qt::AscendingOrder);
  std::copy(list.crbegin(), list.crend(), std::back_inserter(resultList));
  QCOMPARE((int)resultList.size(), 3);
  QCOMPARE(resultList.at(0).column(), 3);
  QVERIFY(resultList.at(0).sortOrder() == Qt::AscendingOrder);
  QCOMPARE(resultList.at(1).column(), 2);
  QVERIFY(resultList.at(1).sortOrder() == Qt::DescendingOrder);
  QCOMPARE(resultList.at(2).column(), 1);
  QVERIFY(resultList.at(2).sortOrder() == Qt::AscendingOrder);
  list.clear();
  QVERIFY(list.isEmpty());
}

void SortProxyModelTest::columnSortOrderListBenchmark()
{
  using ItemModel::ColumnSortOrder;

  ItemModel::ColumnSortOrderList list;
  std::vector<int> resultIndexList;

  list.addColumn(1, Qt::AscendingOrder);
  list.addColumn(2, Qt::DescendingOrder);
  list.addColumn(3, Qt::AscendingOrder);
  resultIndexList.reserve(3);
  QBENCHMARK{
    resultIndexList.clear();
    std::for_each(list.crbegin(), list.crend(), [&resultIndexList](const ColumnSortOrder & cso){
                    resultIndexList.push_back(cso.column());
                  });
  }
  QCOMPARE((int)resultIndexList.size(), 3);
  QCOMPARE(resultIndexList.at(0), 3);
  QCOMPARE(resultIndexList.at(1), 2);
  QCOMPARE(resultIndexList.at(2), 1);
}

void SortProxyModelTest::columnSortStringAttributesTest()
{
  using ItemModel::ColumnSortStringAttributes;

  // Valid attributes
  ColumnSortStringAttributes csa(1, Qt::CaseSensitive, true);
  QVERIFY(!csa.isNull());
  QCOMPARE(csa.column(), 1);
  QVERIFY(csa.caseSensitivity() == Qt::CaseSensitive);
  QVERIFY(csa.numericMode());
  // Null attributes
  ColumnSortStringAttributes nullCsa;
  QVERIFY(nullCsa.isNull());
  QVERIFY(!nullCsa.numericMode());
}

void SortProxyModelTest::columnSortStringAttributesListTest()
{
  using ItemModel::ColumnSortStringAttributes;
  using ItemModel::ColumnSortStringAttributesList;

  ColumnSortStringAttributesList list;

  /*
   * Check adding
   */
  list.addColumn(2, Qt::CaseSensitive, true);
  list.addColumn(1, Qt::CaseInsensitive, false);
  list.addColumn(3, Qt::CaseSensitive, false);
  list.addColumn(0, Qt::CaseInsensitive, true);
  // Check attributes for column 1
  auto csa = list.attributesForColumn(0);
  QVERIFY(!csa.isNull());
  QCOMPARE(csa.column(), 0);
  QVERIFY(csa.caseSensitivity() == Qt::CaseInsensitive);
  QVERIFY(csa.numericMode());
  // Check attributes for column 1
  csa = list.attributesForColumn(1);
  QVERIFY(!csa.isNull());
  QCOMPARE(csa.column(), 1);
  QVERIFY(csa.caseSensitivity() == Qt::CaseInsensitive);
  QVERIFY(!csa.numericMode());
  // Check attributes for column 2
  csa = list.attributesForColumn(2);
  QVERIFY(!csa.isNull());
  QCOMPARE(csa.column(), 2);
  QVERIFY(csa.caseSensitivity() == Qt::CaseSensitive);
  QVERIFY(csa.numericMode());
  // Check attributes for column 3
  csa = list.attributesForColumn(3);
  QVERIFY(!csa.isNull());
  QCOMPARE(csa.column(), 3);
  QVERIFY(csa.caseSensitivity() == Qt::CaseSensitive);
  QVERIFY(!csa.numericMode());
  /*
   * Check attributes for non existing column
   */
  csa = list.attributesForColumn(5);
  QVERIFY(csa.isNull());
  /*
   * Check setting
   */
  // Set attributes for existing column
  list.setColumn(1, Qt::CaseSensitive, true);
  csa = list.attributesForColumn(1);
  QVERIFY(!csa.isNull());
  QCOMPARE(csa.column(), 1);
  QVERIFY(csa.caseSensitivity() == Qt::CaseSensitive);
  QVERIFY(csa.numericMode());
  // Set attributes for non existing column
  list.setColumn(6, Qt::CaseSensitive, true);
  csa = list.attributesForColumn(6);
  QVERIFY(!csa.isNull());
  QCOMPARE(csa.column(), 6);
  QVERIFY(csa.caseSensitivity() == Qt::CaseSensitive);
  QVERIFY(csa.numericMode());
  /*
   * Clear
   */
  list.clear();
  QVERIFY(list.attributesForColumn(0).isNull());
  QVERIFY(list.attributesForColumn(1).isNull());
  QVERIFY(list.attributesForColumn(2).isNull());
  QVERIFY(list.attributesForColumn(3).isNull());
  QVERIFY(list.attributesForColumn(6).isNull());
}

void SortProxyModelTest::columnSortStringAttributesBenchmark()
{
  using ItemModel::ColumnSortStringAttributes;
  using ItemModel::ColumnSortStringAttributesList;

  ColumnSortStringAttributesList list;
  ColumnSortStringAttributes csa1(0, Qt::CaseInsensitive, false);
  ColumnSortStringAttributes csa2(0, Qt::CaseInsensitive, false);
  ColumnSortStringAttributes csa3(0, Qt::CaseInsensitive, false);
  ColumnSortStringAttributes csa4(0, Qt::CaseInsensitive, false);
  ColumnSortStringAttributes csa5(0, Qt::CaseInsensitive, false);

  list.addColumn(2, Qt::CaseSensitive, true);
  list.addColumn(5, Qt::CaseInsensitive, false);
  list.addColumn(3, Qt::CaseSensitive, false);
  list.addColumn(4, Qt::CaseSensitive, false);
  list.addColumn(1, Qt::CaseSensitive, false);
  QBENCHMARK{
    csa1 = list.attributesForColumn(1);
    csa2 = list.attributesForColumn(2);
    csa3 = list.attributesForColumn(3);
    csa4 = list.attributesForColumn(4);
    csa5 = list.attributesForColumn(5);
  }
  QCOMPARE(csa1.column(), 1);
  QCOMPARE(csa2.column(), 2);
  QCOMPARE(csa3.column(), 3);
  QCOMPARE(csa4.column(), 4);
  QCOMPARE(csa5.column(), 5);
}

void SortProxyModelTest::sortIntTest()
{
  VariantTableModel model;
  SortProxyModel proxyModel;
  /*
   * Setup models
   */
  proxyModel.setSourceModel(&model);
  proxyModel.setDynamicSortFilter(false);
  model.resize(5, 5);
  /*
   * Single column sort
   * Use sort(int column, Qt::SortOrder)
   */
  // Sort ascending
  model.populateColumn(0, {9,1,3,6,4});
  QVERIFY(!isModelColumnSortedNumeric(proxyModel, 0));
  proxyModel.sort(0, Qt::AscendingOrder);
  QCOMPARE(proxyModel.sortColumn(), 0);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant(3));
  QCOMPARE(getModelData(proxyModel, 2, 0), QVariant(4));
  QCOMPARE(getModelData(proxyModel, 3, 0), QVariant(6));
  QCOMPARE(getModelData(proxyModel, 4, 0), QVariant(9));
  // Sort descending
  proxyModel.sort(0, Qt::DescendingOrder);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(9));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant(6));
  QCOMPARE(getModelData(proxyModel, 2, 0), QVariant(4));
  QCOMPARE(getModelData(proxyModel, 3, 0), QVariant(3));
  QCOMPARE(getModelData(proxyModel, 4, 0), QVariant(1));
  /*
   * No column to sort
   */
  proxyModel.sort(-1);  // Restore model sort
  QVERIFY(!isModelColumnSortedNumeric(proxyModel, 0));
  proxyModel.sort();
  QVERIFY(!isModelColumnSortedNumeric(proxyModel, 0));
  /*
   * 1 column sort
   */
  // Sort ascending
  proxyModel.sort(-1);  // Restore model sort
  QVERIFY(!isModelColumnSortedNumeric(proxyModel, 0));
  proxyModel.addColumnToSortOrder(0, Qt::AscendingOrder);
  proxyModel.sort();
  QVERIFY(isModelColumnSortedNumeric(proxyModel, 0));
  // Clear
  proxyModel.sort(-1);  // Restore model sort
  QVERIFY(!isModelColumnSortedNumeric(proxyModel, 0));
  proxyModel.clearColumnsSortOrder();
  proxyModel.sort();
  QVERIFY(!isModelColumnSortedNumeric(proxyModel, 0));
  // Sort desending
  proxyModel.addColumnToSortOrder(0, Qt::DescendingOrder);
  proxyModel.sort();
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(9));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant(6));
  QCOMPARE(getModelData(proxyModel, 2, 0), QVariant(4));
  QCOMPARE(getModelData(proxyModel, 3, 0), QVariant(3));
  QCOMPARE(getModelData(proxyModel, 4, 0), QVariant(1));
  /*
   * 2 columns sort
   */
  model.populateColumn(0, {2,2,2,1,1});
  model.populateColumn(1, {3,2,1,2,1});
  proxyModel.clearColumnsSortOrder();
  proxyModel.addColumnToSortOrder(0, Qt::AscendingOrder);
  proxyModel.addColumnToSortOrder(1, Qt::AscendingOrder);
  QVERIFY(!isModelColumnSortedNumeric(proxyModel, 0));
  QVERIFY(!isModelColumnSortedNumeric(proxyModel, 1));
  proxyModel.sort();
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 1, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 2, 0), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 2, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 3, 0), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 3, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 4, 0), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 4, 1), QVariant(3));
  /*
   * 2 columns sort
   * -> Check other than by column 0, 1
   */
  model.populateColumn(3, {2,2,2,1,1});
  model.populateColumn(1, {3,2,1,2,1});
  proxyModel.clearColumnsSortOrder();
  proxyModel.addColumnToSortOrder(3, Qt::AscendingOrder);
  proxyModel.addColumnToSortOrder(1, Qt::AscendingOrder);
  proxyModel.sort(-1);  // Restore model sort
  QVERIFY(!isModelColumnSortedNumeric(proxyModel, 3));
  QVERIFY(!isModelColumnSortedNumeric(proxyModel, 1));
  proxyModel.sort();
  QCOMPARE(getModelData(proxyModel, 0, 3), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 1, 3), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 1, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 2, 3), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 2, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 3, 3), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 3, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 4, 3), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 4, 1), QVariant(3));
  /*
   * 3 columns sort
   */
  // Disable sorting before model resize
  proxyModel.sort(-1);
  model.resize(6, 5);
  model.populateColumn(0, {200,200,200,100,100,100});
  model.populateColumn(1, { 30, 30, 10, 30, 10, 10});
  model.populateColumn(2, {  6,  5,  4,  3,  2,  1});
  proxyModel.clearColumnsSortOrder();
  proxyModel.addColumnToSortOrder(0, Qt::AscendingOrder);
  proxyModel.addColumnToSortOrder(1, Qt::AscendingOrder);
  proxyModel.addColumnToSortOrder(2, Qt::AscendingOrder);
  QVERIFY(!isModelColumnSortedNumeric(proxyModel, 0));
  QVERIFY(!isModelColumnSortedNumeric(proxyModel, 1));
  QVERIFY(!isModelColumnSortedNumeric(proxyModel, 2));
  proxyModel.sort();
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(100));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(10));
  QCOMPARE(getModelData(proxyModel, 0, 2), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant(100));
  QCOMPARE(getModelData(proxyModel, 1, 1), QVariant(10));
  QCOMPARE(getModelData(proxyModel, 1, 2), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 2, 0), QVariant(100));
  QCOMPARE(getModelData(proxyModel, 2, 1), QVariant(30));
  QCOMPARE(getModelData(proxyModel, 2, 2), QVariant(3));
  QCOMPARE(getModelData(proxyModel, 3, 0), QVariant(200));
  QCOMPARE(getModelData(proxyModel, 3, 1), QVariant(10));
  QCOMPARE(getModelData(proxyModel, 3, 2), QVariant(4));
  QCOMPARE(getModelData(proxyModel, 4, 0), QVariant(200));
  QCOMPARE(getModelData(proxyModel, 4, 1), QVariant(30));
  QCOMPARE(getModelData(proxyModel, 4, 2), QVariant(5));
  QCOMPARE(getModelData(proxyModel, 5, 0), QVariant(200));
  QCOMPARE(getModelData(proxyModel, 5, 1), QVariant(30));
  QCOMPARE(getModelData(proxyModel, 5, 2), QVariant(6));
}

void SortProxyModelTest::sortIntBenchmark()
{
  QFETCH(int, n);
  VariantTableModel model;
  SortProxyModel proxyModel;
  /*
   * Setup models
   */
  model.resize(n, 5);
  proxyModel.setSourceModel(&model);
  proxyModel.setDynamicSortFilter(false);
  proxyModel.addColumnToSortOrder(1, Qt::DescendingOrder);
  proxyModel.addColumnToSortOrder(0, Qt::DescendingOrder);
  proxyModel.addColumnToSortOrder(3, Qt::DescendingOrder);
  model.populateColumnWithInt(0, 1);
  model.populateColumnWithInt(1, 1, 3);
  model.populateColumnWithInt(2, 10, 4);
  model.populateColumnWithInt(3, 100, 7);
  model.populateColumnWithInt(4, 15, 2);
  QBENCHMARK{
    proxyModel.sort();
  }
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(n));
  QCOMPARE(getModelData(proxyModel, n-1, 0), QVariant(1));
}

void SortProxyModelTest::sortIntBenchmark_data()
{
  QTest::addColumn<int>("n");

  QTest::newRow("10") << 10;
  QTest::newRow("100") << 100;
  QTest::newRow("1'000") << 1000;
  QTest::newRow("10'000") << 10000;
}

void SortProxyModelTest::sortRoleTest()
{
  VariantTableModel model(VariantTableModelStorageRule::SeparateDisplayAndEditRoleData);
  SortProxyModel proxyModel;
  /*
   * Setup models
   */
  proxyModel.setSourceModel(&model);
  proxyModel.setDynamicSortFilter(false);
  model.resize(5, 5);
  model.populateColumn(0, {"A","B","C"}, Qt::DisplayRole);
  model.populateColumn(0, {3,2,1}, Qt::EditRole);

  /*
   * Sort by DisplayRole
   */
  proxyModel.setSortRole(Qt::DisplayRole);
  proxyModel.sort(0, Qt::AscendingOrder);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant("A"));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant("B"));
  QCOMPARE(getModelData(proxyModel, 2, 0), QVariant("C"));
  /*
   * Sort by EditRole
   */
  proxyModel.setSortRole(Qt::EditRole);
  proxyModel.sort(0, Qt::AscendingOrder);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant("C"));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant("B"));
  QCOMPARE(getModelData(proxyModel, 2, 0), QVariant("A"));
}

void SortProxyModelTest::sortStringCsTest()
{
  VariantTableModel model;
  SortProxyModel proxyModel;
  /*
   * Setup models
   * Note: we must be shure that sorting is not done locale aware,
   *       else caseSensitivity is allways CaseInsensitive on some platforms.
   */
  proxyModel.setSourceModel(&model);
  proxyModel.setDynamicSortFilter(false);
  proxyModel.setSortLocaleAware(false);
  model.resize(6, 1);
  model.populateColumn(0, {"c","b","a","C","B","A"});
  QVERIFY(!proxyModel.isSortLocaleAware());
  /*
   * Sort using QSortFilterProxyModel::sortCaseSensitivity()
   */
  // Case sensitive
  proxyModel.setSortCaseSensitivity(Qt::CaseSensitive);
  QVERIFY(!isModelColumnSortedAscii(proxyModel, 0, Qt::CaseSensitive));
  proxyModel.sort(0, Qt::AscendingOrder);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant("A"));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant("B"));
  QCOMPARE(getModelData(proxyModel, 2, 0), QVariant("C"));
  QCOMPARE(getModelData(proxyModel, 3, 0), QVariant("a"));
  QCOMPARE(getModelData(proxyModel, 4, 0), QVariant("b"));
  QCOMPARE(getModelData(proxyModel, 5, 0), QVariant("c"));
  QVERIFY(isModelColumnSortedAscii(proxyModel, 0, Qt::CaseSensitive));
  // Case insensitive
  proxyModel.sort(-1);
  proxyModel.setSortCaseSensitivity(Qt::CaseInsensitive);
  QVERIFY(!isModelColumnSortedAscii(proxyModel, 0, Qt::CaseInsensitive));
  proxyModel.sort(0, Qt::AscendingOrder);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant("a"));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant("A"));
  QCOMPARE(getModelData(proxyModel, 2, 0), QVariant("b"));
  QCOMPARE(getModelData(proxyModel, 3, 0), QVariant("B"));
  QCOMPARE(getModelData(proxyModel, 4, 0), QVariant("c"));
  QCOMPARE(getModelData(proxyModel, 5, 0), QVariant("C"));
  QVERIFY(isModelColumnSortedAscii(proxyModel, 0, Qt::CaseInsensitive));
  /*
   * Sort using case sensitivity set by addColumnToSortOrder()
   */
  // Case sensitive
  proxyModel.setSortCaseSensitivity(Qt::CaseInsensitive); // Setup global case sensitiviy opposite to check
  proxyModel.addColumnToSortOrder(0, StringNumericMode::Alphabetical, Qt::AscendingOrder, Qt::CaseSensitive);
  proxyModel.sort(-1);
  QVERIFY(!isModelColumnSortedAscii(proxyModel, 0, Qt::CaseSensitive));
  proxyModel.sort();
  QVERIFY(isModelColumnSortedAscii(proxyModel, 0, Qt::CaseSensitive));
  // Case insensitive
  proxyModel.setSortCaseSensitivity(Qt::CaseSensitive); // Setup global case sensitiviy opposite to check
  proxyModel.clearColumnsSortOrder();
  proxyModel.addColumnToSortOrder(0, StringNumericMode::Alphabetical, Qt::AscendingOrder, Qt::CaseInsensitive);
  proxyModel.sort(-1);
  QVERIFY(!isModelColumnSortedAscii(proxyModel, 0, Qt::CaseInsensitive));
  proxyModel.sort();
  QVERIFY(isModelColumnSortedAscii(proxyModel, 0, Qt::CaseInsensitive));
  proxyModel.clearColumnsSortOrder();
  /*
   * Sort using case sensitivity set by setColumnStringSortAttribute()
   */
  // Case sensitive
  proxyModel.setSortCaseSensitivity(Qt::CaseInsensitive); // Setup global case sensitiviy opposite to check
  proxyModel.setColumnStringSortAttributes(0, StringNumericMode::Alphabetical, Qt::CaseSensitive);
  proxyModel.sort(-1);
  QVERIFY(!isModelColumnSortedAscii(proxyModel, 0, Qt::CaseSensitive));
  proxyModel.sort(0, Qt::AscendingOrder);
  QVERIFY(isModelColumnSortedAscii(proxyModel, 0, Qt::CaseSensitive));
  // Case insensitive
  proxyModel.setSortCaseSensitivity(Qt::CaseSensitive); // Setup global case sensitiviy opposite to check
  proxyModel.setColumnStringSortAttributes(0, StringNumericMode::Alphabetical, Qt::CaseInsensitive);
  proxyModel.sort(-1);
  QVERIFY(!isModelColumnSortedAscii(proxyModel, 0, Qt::CaseInsensitive));
  proxyModel.sort(0, Qt::AscendingOrder);
  QVERIFY(isModelColumnSortedAscii(proxyModel, 0, Qt::CaseInsensitive));
  /*
   * Check clearColumnsStringSortAttributes()
   */
  proxyModel.clearColumnsStringSortAttributes();
  // Case sensitive
  proxyModel.setSortCaseSensitivity(Qt::CaseSensitive);
  proxyModel.sort(-1);
  QVERIFY(!isModelColumnSortedAscii(proxyModel, 0, Qt::CaseSensitive));
  proxyModel.sort(0, Qt::AscendingOrder);
  QVERIFY(isModelColumnSortedAscii(proxyModel, 0, Qt::CaseSensitive));
  // Case insensitive
  proxyModel.setSortCaseSensitivity(Qt::CaseInsensitive);
  proxyModel.sort(-1);
  QVERIFY(!isModelColumnSortedAscii(proxyModel, 0, Qt::CaseInsensitive));
  proxyModel.sort(0, Qt::AscendingOrder);
  QVERIFY(isModelColumnSortedAscii(proxyModel, 0, Qt::CaseInsensitive));
}

void SortProxyModelTest::sortStringNumericModeTest()
{
  VariantTableModel model;
  SortProxyModel proxyModel;
  /*
   * Setup models
   */
  proxyModel.setSourceModel(&model);
  proxyModel.setDynamicSortFilter(false);
  proxyModel.setSortLocaleAware(false);
  model.resize(3, 1);
  model.populateColumn(0, {"a10","a2","a1"});
  /*
   * Alphabetical mumeric order
   * - Default
   */
  proxyModel.sort(-1);
  proxyModel.sort(0, Qt::AscendingOrder);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant("a1"));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant("a10"));
  QCOMPARE(getModelData(proxyModel, 2, 0), QVariant("a2"));
  /*
   * Natural numeric mode
   * - Setup with addColumnToSortOrder()
   */
  proxyModel.addColumnToSortOrder(0, StringNumericMode::Natural);
  proxyModel.sort(-1);
  proxyModel.sort(0, Qt::AscendingOrder);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant("a1"));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant("a2"));
  QCOMPARE(getModelData(proxyModel, 2, 0), QVariant("a10"));
  /*
   * Back to Alphabetical numeric order
   *  - clearColumnsStringSortAttributes()
   */
  proxyModel.clearColumnsStringSortAttributes();
  proxyModel.sort(-1);
  proxyModel.sort(0, Qt::AscendingOrder);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant("a1"));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant("a10"));
  QCOMPARE(getModelData(proxyModel, 2, 0), QVariant("a2"));
  /*
   * Natural numeric mode
   * - Setup with setColumnStringSortAttributes()
   */
  proxyModel.setColumnStringSortAttributes(0, StringNumericMode::Natural);
  proxyModel.sort(-1);
  proxyModel.sort(0, Qt::AscendingOrder);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant("a1"));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant("a2"));
  QCOMPARE(getModelData(proxyModel, 2, 0), QVariant("a10"));
}

void SortProxyModelTest::sortStringIntMixedTest()
{
  VariantTableModel model;
  SortProxyModel proxyModel;
  /*
   * Setup models
   */
  proxyModel.setSourceModel(&model);
  proxyModel.setDynamicSortFilter(false);
  proxyModel.setSortLocaleAware(false);
  model.resize(4, 1);
  model.populateColumn(0, {4,"3",2,"1"});
  /*
   * Check
   */
  proxyModel.sort(0, Qt::AscendingOrder);
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant("1"));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 2, 0), QVariant("3"));
  QCOMPARE(getModelData(proxyModel, 3, 0), QVariant(4));
}

void SortProxyModelTest::sortStringNonLocalAwareBenchmark()
{
  QFETCH(int, n);
  VariantTableModel model;
  SortProxyModel proxyModel;
  /*
   * Setup models
   */
  model.populate(n, 5);
  proxyModel.setSourceModel(&model);
  proxyModel.setDynamicSortFilter(false);
  proxyModel.setSortLocaleAware(false);
  proxyModel.addColumnToSortOrder(1, Qt::AscendingOrder);
  proxyModel.addColumnToSortOrder(0, Qt::AscendingOrder);
  proxyModel.addColumnToSortOrder(3, Qt::AscendingOrder);
  QBENCHMARK{
    proxyModel.sort();
  }
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant("0A"));
  QCOMPARE(getModelData(proxyModel, n-1, 0), QVariant("9A"));
}

void SortProxyModelTest::sortStringNonLocalAwareBenchmark_data()
{
  QTest::addColumn<int>("n");

  QTest::newRow("10") << 10;
  QTest::newRow("100") << 100;
  QTest::newRow("1'000") << 1000;
  QTest::newRow("10'000") << 10000;
}

void SortProxyModelTest::sortStringLocalAwareBenchmark()
{
  QFETCH(int, n);
  VariantTableModel model;
  SortProxyModel proxyModel;
  /*
   * Setup models
   */
  model.populate(n, 5);
  proxyModel.setSourceModel(&model);
  proxyModel.setDynamicSortFilter(false);
  proxyModel.setSortLocaleAware(true);
  proxyModel.addColumnToSortOrder(1, StringNumericMode::Natural, Qt::AscendingOrder);
  proxyModel.addColumnToSortOrder(0, StringNumericMode::Natural, Qt::AscendingOrder);
  proxyModel.addColumnToSortOrder(3, StringNumericMode::Natural, Qt::AscendingOrder);
  QBENCHMARK{
    proxyModel.sort();
  }
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant("0A"));
  QCOMPARE(getModelData(proxyModel, n-1, 0), QVariant(QString::number(n-1) + "A"));
}

void SortProxyModelTest::sortStringLocalAwareBenchmark_data()
{
  QTest::addColumn<int>("n");

  QTest::newRow("10") << 10;
  QTest::newRow("100") << 100;
  QTest::newRow("1'000") << 1000;
  QTest::newRow("10'000") << 10000;
}

void SortProxyModelTest::sortSetModelTest()
{
  /*
   * Setup proxy model without source model set
   */
  SortProxyModel proxyModel;
  proxyModel.setDynamicSortFilter(false);
  proxyModel.setColumnStringSortAttributes(0, StringNumericMode::Alphabetical);
  proxyModel.setSortLocaleAware(false);
  proxyModel.addColumnToSortOrder(0);
  /*
   * Try to sort without source model set
   */
  proxyModel.sort();
  /*
   * Setup source model
   */
  VariantTableModel model;
  model.resize(3, 1);
  model.populateColumn(0, {"C","B","A"});
  proxyModel.setSourceModel(&model);
  /*
   * Sort with source model set
   */
  QVERIFY(!isModelColumnSortedAscii(proxyModel, 0, Qt::CaseInsensitive));
  proxyModel.sort();
  QVERIFY(isModelColumnSortedAscii(proxyModel, 0, Qt::CaseInsensitive));
  /*
   * Check also setting up proxy model with source model set,
   * but only set dimentions after setup.
   * Such situation happens when using, for example, QSqlTableModel: 
   * We must be able to setup everything, and call select later.
   * Only once select was called, model will be populated.
   */
  VariantTableModel model2;
  SortProxyModel proxyModel2;
  proxyModel2.setSourceModel(&model2);
  proxyModel2.setDynamicSortFilter(false);
  proxyModel2.setColumnStringSortAttributes(0, StringNumericMode::Alphabetical);
  proxyModel2.setSortLocaleAware(false);
  proxyModel2.addColumnToSortOrder(0);
  model2.resize(3, 1);
  model2.populateColumn(0, {"C","B","A"});
  QVERIFY(!isModelColumnSortedAscii(proxyModel2, 0, Qt::CaseInsensitive));
  proxyModel2.sort();
  QVERIFY(isModelColumnSortedAscii(proxyModel2, 0, Qt::CaseInsensitive));
}

void SortProxyModelTest::sortColumnCountChangeTest()
{
  VariantTableModel model;
  SortProxyModel proxyModel;
  /*
   * Setup models
   */
  proxyModel.setSourceModel(&model);
  proxyModel.setDynamicSortFilter(false);
  proxyModel.addColumnToSortOrder(0, Qt::AscendingOrder);
  proxyModel.addColumnToSortOrder(1, Qt::AscendingOrder);
  proxyModel.addColumnToSortOrder(2, Qt::AscendingOrder);
  model.resize(8, 3);
  model.populateColumn(0, {2,2,2,2,1,1,1,1});
  model.populateColumn(1, {2,2,1,1,2,2,1,1});
  model.populateColumn(2, {2,1,2,1,2,1,2,1});
  /*
   * Initial:
   * Sort with in normal situation
   */
  QVERIFY(!isModelColumnSortedNumeric(proxyModel, 0));
  proxyModel.sort();
  QVERIFY(isModelColumnSortedNumeric(proxyModel, 0));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 1, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 2, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 3, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 4, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 5, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 6, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 7, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 0, 2), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 1, 2), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 2, 2), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 3, 2), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 4, 2), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 5, 2), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 6, 2), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 7, 2), QVariant(2));
  /*
   * Remove last column
   */
  proxyModel.sort(-1);
  QVERIFY(!isModelColumnSortedNumeric(proxyModel, 0));
  model.removeLastColumn();
  QCOMPARE(proxyModel.columnCount(), 2);
  proxyModel.sort();
  QVERIFY(isModelColumnSortedNumeric(proxyModel, 0));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 1, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 2, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 3, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 4, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 5, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 6, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 7, 1), QVariant(2));
  /*
   * Append a column
   */
  proxyModel.sort(-1);
  QVERIFY(!isModelColumnSortedNumeric(proxyModel, 0));
  model.appendColumn();
  model.populateColumn(2, {2,1,2,1,2,1,2,1});
  QCOMPARE(proxyModel.columnCount(), 3);
  proxyModel.sort();
  QVERIFY(isModelColumnSortedNumeric(proxyModel, 0));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 1, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 2, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 3, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 4, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 5, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 6, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 7, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 0, 2), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 1, 2), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 2, 2), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 3, 2), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 4, 2), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 5, 2), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 6, 2), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 7, 2), QVariant(2));
}

void SortProxyModelTest::sortSignalTest()
{
  QVariantList arguments;
  VariantTableModel model;
  SortProxyModel proxyModel;
  QSignalSpy columnSortedSpy(&proxyModel, &SortProxyModel::columnSorted);
  QSignalSpy modelSortedSpy(&proxyModel, &SortProxyModel::modelSorted);
  QVERIFY(columnSortedSpy.isValid());
  QVERIFY(modelSortedSpy.isValid());
  /*
   * Setup models
   */
  proxyModel.setSourceModel(&model);
  proxyModel.setDynamicSortFilter(false);
  proxyModel.addColumnToSortOrder(0);
  proxyModel.addColumnToSortOrder(1);
  model.resize(4, 2);
  model.populateColumn(0, {2,2,1,1});
  model.populateColumn(1, {2,1,2,1});
  columnSortedSpy.clear();
  modelSortedSpy.clear();
  /*
   * Sort column 0
   */
  proxyModel.sort(0, Qt::AscendingOrder);
  QCOMPARE(columnSortedSpy.count(), 1);
  arguments = columnSortedSpy.takeFirst();
  QCOMPARE(arguments.size(), 1);
  QCOMPARE(arguments.at(0), QVariant(0));
  QCOMPARE(modelSortedSpy.count(), 0);
  /*
   * Sort model
   */
  proxyModel.sort();
  QCOMPARE(columnSortedSpy.count(), 2);
  arguments = columnSortedSpy.takeFirst();
  QCOMPARE(arguments.size(), 1);
  QCOMPARE(arguments.at(0), QVariant(1));
  arguments = columnSortedSpy.takeFirst();
  QCOMPARE(arguments.size(), 1);
  QCOMPARE(arguments.at(0), QVariant(0));
  QCOMPARE(modelSortedSpy.count(), 1);
}

void SortProxyModelTest::sortSetterEventTest()
{
  VariantTableModel model;
  SortProxyModel proxyModel;
  /*
   * Setup models
   */
  proxyModel.setSourceModel(&model);
  proxyModel.setDynamicSortFilter(false);
  model.resize(4, 2);
  /*
   * When a setter is updated, QSortFilterProxyModel calls his private sort() implementation.
   * This version will sort column == QSortFilterProxyModel::sortColumn(),
   * which holds the column passed to QSortFilterProxyModel::sort(int, Qt::SortOrder).
   *
   * To make shure that our setters works, we test with 2 columns.
   *
   * Note:
   *  - addColumnToSortOrder() does not automatically sort
   */
  proxyModel.addColumnToSortOrder(0, Qt::AscendingOrder);
  proxyModel.addColumnToSortOrder(1, Qt::AscendingOrder);
  model.populateColumn(0, {4,3,2,1});
  model.populateColumn(1, {2,1,2,1});
  /*
   * Check setDynamicSortFilter()
   */
  proxyModel.setDynamicSortFilter(true);
  QVERIFY(proxyModel.dynamicSortFilter());
  proxyModel.sort(-1);
  QVERIFY(!isModelColumnSortedNumeric(proxyModel, 0));
  proxyModel.setDynamicSortFilter(false);
  QVERIFY(!proxyModel.dynamicSortFilter());
  QVERIFY(!isModelColumnSortedNumeric(proxyModel, 0));
  /*
   * Check setSortCaseSensitivity()
   */
  proxyModel.setSortCaseSensitivity(Qt::CaseInsensitive);
  QVERIFY(proxyModel.sortCaseSensitivity() == Qt::CaseInsensitive);
  proxyModel.sort(-1);
  QVERIFY(!isModelColumnSortedNumeric(proxyModel, 0));
  proxyModel.setSortCaseSensitivity(Qt::CaseSensitive);
  QVERIFY(proxyModel.sortCaseSensitivity() == Qt::CaseSensitive);
  QVERIFY(isModelColumnSortedNumeric(proxyModel, 0));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 1, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 2, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 3, 1), QVariant(2));
  /*
   * Check setSortLocaleAware()
   */
  proxyModel.setSortLocaleAware(true);
  QVERIFY(proxyModel.isSortLocaleAware());
  proxyModel.sort(-1);
  QVERIFY(!isModelColumnSortedNumeric(proxyModel, 0));
  proxyModel.setSortLocaleAware(false);
  QVERIFY(!proxyModel.isSortLocaleAware());
  QVERIFY(isModelColumnSortedNumeric(proxyModel, 0));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 1, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 2, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 3, 1), QVariant(2));
  /*
   * Check setSortRole()
   */
  proxyModel.setSortRole(Qt::EditRole);
  QVERIFY(proxyModel.sortRole() == Qt::EditRole);
  proxyModel.sort(-1);
  QVERIFY(!isModelColumnSortedNumeric(proxyModel, 0));
  proxyModel.setSortRole(Qt::DisplayRole);
  QVERIFY(proxyModel.sortRole() == Qt::DisplayRole);
  QVERIFY(isModelColumnSortedNumeric(proxyModel, 0));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 1, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 2, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 3, 1), QVariant(2));
}

void SortProxyModelTest::dynamicSortTest()
{
  QModelIndex index;
  QVariantList arguments;
  VariantTableModel model;
  SortProxyModel proxyModel;
  QSignalSpy columnSortedSpy(&proxyModel, &SortProxyModel::columnSorted);
  QSignalSpy modelSortedSpy(&proxyModel, &SortProxyModel::modelSorted);
  QVERIFY(columnSortedSpy.isValid());
  QVERIFY(modelSortedSpy.isValid());
  /*
   * When source model is updated, QSortFilterProxyModel updates his mapping,
   * which will sort column == QSortFilterProxyModel::sortColumn(),
   * which holds the column passed to QSortFilterProxyModel::sort(int, Qt::SortOrder).
   *
   * To make shure that SortProxyModel handles events correctly, test with 2 columns.
   *
   */
  proxyModel.addColumnToSortOrder(0, Qt::AscendingOrder);
  proxyModel.addColumnToSortOrder(1, Qt::AscendingOrder);
  /*
   * Check that setDynamicSortFilter() only callst sort() when switching OFF->ON
   */
  columnSortedSpy.clear();
  modelSortedSpy.clear();
  proxyModel.setDynamicSortFilter(false);
  QCOMPARE(columnSortedSpy.count(), 0);
  QCOMPARE(modelSortedSpy.count(), 0);
  proxyModel.setDynamicSortFilter(false);
  QCOMPARE(columnSortedSpy.count(), 0);
  QCOMPARE(modelSortedSpy.count(), 0);
  proxyModel.setDynamicSortFilter(true);
  QCOMPARE(columnSortedSpy.count(), 2);
  QCOMPARE(modelSortedSpy.count(), 1);
  columnSortedSpy.clear();
  modelSortedSpy.clear();
  proxyModel.setDynamicSortFilter(true);
  QCOMPARE(columnSortedSpy.count(), 0);
  QCOMPARE(modelSortedSpy.count(), 0);
  proxyModel.setDynamicSortFilter(false);
  QCOMPARE(columnSortedSpy.count(), 0);
  QCOMPARE(modelSortedSpy.count(), 0);
  // Enable dynamic sort for next tests
  proxyModel.setDynamicSortFilter(true);
  QCOMPARE(columnSortedSpy.count(), 2);
  QCOMPARE(modelSortedSpy.count(), 1);
  /*
   * Set model
   */
  // Populate model
  model.resize(4, 3);
  model.populateColumn(0, {2,2,1,1});
  model.populateColumn(1, {2,1,2,1});
  model.populateColumn(2, {"D","C","B","A"});
  columnSortedSpy.clear();
  modelSortedSpy.clear();
  // Set model and check
  proxyModel.setSourceModel(&model);
  QVERIFY(isModelColumnSortedNumeric(proxyModel, 0));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 1, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 2, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 3, 1), QVariant(2));
  QCOMPARE(columnSortedSpy.count(), 2);
  QCOMPARE(modelSortedSpy.count(), 1);
  /*
   * Remove last row
   *  - Has nothing to do
   */
  columnSortedSpy.clear();
  modelSortedSpy.clear();
  model.removeLastRow();
  QCOMPARE(proxyModel.rowCount(), 3);
  QVERIFY(isModelColumnSortedNumeric(proxyModel, 0));
  QCOMPARE(columnSortedSpy.count(), 0);
  QCOMPARE(modelSortedSpy.count(), 0);
  /*
   * Append a row
   */
  proxyModel.sort(-1);
  QVERIFY(!isModelColumnSortedNumeric(proxyModel, 0));
  columnSortedSpy.clear();
  modelSortedSpy.clear();
  model.appendRow();
  /*
   * Appending  row inserts Null values.
   * Null < anything is allways false,
   * so we cannot check data in a reliable way.
   */
  QCOMPARE(columnSortedSpy.count(), 2);
  QCOMPARE(modelSortedSpy.count(), 1);
//   QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(1));
//   QCOMPARE(getModelData(proxyModel, 1, 0), QVariant(2));
//   QCOMPARE(getModelData(proxyModel, 2, 0), QVariant(2));
//   QVERIFY(getModelData(proxyModel, 3, 0).isNull());
//   QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(2));
//   QCOMPARE(getModelData(proxyModel, 1, 1), QVariant(1));
//   QCOMPARE(getModelData(proxyModel, 2, 1), QVariant(2));
//   QVERIFY(getModelData(proxyModel, 3, 1).isNull());
  /*
   * Model reset
   */
  columnSortedSpy.clear();
  modelSortedSpy.clear();
  model.repopulateByColumns({{2,2,1,1},{2,1,2,1},{"D","C","B","A"}});
  // Check signals
  QCOMPARE(columnSortedSpy.count(), 2);
  QCOMPARE(modelSortedSpy.count(), 1);
  /*
   * Check that model is sorted
   * -----
   * |1|1|
   * -----
   * |1|2|
   * -----
   * |2|1|
   * -----
   * |2|2|
   * -----
   */
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 2, 0), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 3, 0), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 1, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 2, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 3, 1), QVariant(2));
  /*
   * Change data in column 0 (sorted column)
   */
  columnSortedSpy.clear();
  modelSortedSpy.clear();
  index = proxyModel.index(0, 0);
  proxyModel.setData(index, 5);
  // Check signals
  QCOMPARE(columnSortedSpy.count(), 2);
  QCOMPARE(modelSortedSpy.count(), 1);
  /*
   * Check that model is sorted
   * -----
   * |1|2|
   * -----
   * |2|1|
   * -----
   * |2|2|
   * -----
   * |5|1|
   * -----
   */
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 2, 0), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 3, 0), QVariant(5));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 1, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 2, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 3, 1), QVariant(1));
  /*
   * Restore to unsorted model
   * -----
   * |2|2|
   * -----
   * |2|1|
   * -----
   * |1|2|
   * -----
   * |1|1|
   * -----
   */
  model.repopulateByColumns({{2,2,1,1},{2,1,2,1},{"D","C","B","A"}});
  proxyModel.sort(-1);
  QVERIFY(!isModelColumnSortedNumeric(proxyModel, 0));
  /*
   * Change data in column 1 (sorted column)
   */
  columnSortedSpy.clear();
  modelSortedSpy.clear();
  index = proxyModel.index(0, 1);
  proxyModel.setData(index, 5);
  // Check signals
  QCOMPARE(columnSortedSpy.count(), 2);
  QCOMPARE(modelSortedSpy.count(), 1);
  /*
   * Check that model is sorted
   * -----
   * |1|1|
   * -----
   * |1|2|
   * -----
   * |2|1|
   * -----
   * |2|5|
   * -----
   */
  QCOMPARE(getModelData(proxyModel, 0, 0), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 1, 0), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 2, 0), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 3, 0), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 0, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 1, 1), QVariant(2));
  QCOMPARE(getModelData(proxyModel, 2, 1), QVariant(1));
  QCOMPARE(getModelData(proxyModel, 3, 1), QVariant(5));
  /*
   * Change data in column 2 (Not sorted column)
   */
  columnSortedSpy.clear();
  modelSortedSpy.clear();
  index = proxyModel.index(0, 2);
  proxyModel.setData(index, "Z");
  // Check signals
  QCOMPARE(columnSortedSpy.count(), 0);
  QCOMPARE(modelSortedSpy.count(), 0);
}

void SortProxyModelTest::fetchTest()
{
  QSKIP("Need more experience to implement. Maybe should be implemented in ItemModel_Sql ?");
}

/*
 * Helper functions
 */

QVariant SortProxyModelTest::getModelData(const QAbstractItemModel & model, int row, int column)
{
  auto index = model.index(row, column);
  Q_ASSERT(index.isValid());
  return model.data(index);
}

bool SortProxyModelTest::isModelColumnSortedNumeric(const QAbstractItemModel& model, int column)
{
  Q_ASSERT(column >= 0);
  Q_ASSERT(column < model.columnCount());

  std::vector<QVariant> data;
  const int n = model.rowCount();
  for(int row = 0; row < n; ++row){
    data.push_back( getModelData(model, row, column) );
  }
  return std::is_sorted(data.cbegin(), data.cend());
}

bool SortProxyModelTest::isModelColumnSortedAscii(const QAbstractItemModel& model, int column, Qt::CaseSensitivity caseSensitiviy)
{
  QStringList list;
  const int n = model.rowCount();
  for(int row = 0; row < n; ++row){
    list << getModelData(model, row, column).toString();
  }

  return std::is_sorted(list.cbegin(), list.cend(), [caseSensitiviy](const QString & a, const QString & b){ return QString::compare(a, b, caseSensitiviy) < 0; });
}

void SortProxyModelTest::displayModels(QAbstractItemModel* sourceModel, QSortFilterProxyModel* proxyModel)
{
  Q_ASSERT(sourceModel != nullptr);
  Q_ASSERT(proxyModel != nullptr);

  QTableView sourceView;
  QTableView proxyView;

  sourceView.setModel(sourceModel);
  sourceView.setWindowTitle("Source");
  proxyView.setModel(proxyModel);
  proxyView.setWindowTitle("Proxy");
  //proxyView.setSortingEnabled(true);
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
  SortProxyModelTest test;

  return QTest::qExec(&test, argc, argv);
}

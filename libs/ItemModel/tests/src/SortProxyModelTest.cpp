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
#include <iterator>
#include <vector>
#include <array>
#include <algorithm>

#include <QDebug>

namespace ItemModel = Mdt::ItemModel;
using ItemModel::VariantTableModel;
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
  proxyModel.setSourceModel(&model);
  proxyModel.setDynamicSortFilter(false);
  proxyModel.addColumnToSortOrder(1, Qt::DescendingOrder);
  proxyModel.addColumnToSortOrder(0, Qt::DescendingOrder);
  proxyModel.addColumnToSortOrder(3, Qt::DescendingOrder);
  model.resize(n, 5);
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

  /// \note process with ints..

  /*
   * Sort by DisplayRole
   */
  
  /*
   * Sort by EditRole
   */
  
  QFAIL("Not complete");
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

/// \todo Possibly this test is to compilaced and oberkill...
void SortProxyModelTest::sortStringLocaleTest()
{
  /*
   * Sort localAware
   */
  
  /*
   * Sort non locale localAware
   */
  
  
  QFAIL("Not complete");
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

void SortProxyModelTest::sortStringAlphaTest()
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
   * Single column sort with no attributes set
   */
  // Case sensitive
  
  // Case insensitive
  
  
  /*
   * Check isSortLocaleAware (from QSortFilterProxyModel)
   */
  
  /*
   * Single column sort with some attributes set
   */

  QFAIL("Not complete");
}

void SortProxyModelTest::sortStringMultiColumnTest()
{
  /*
   * Multi-column sort with no attributes set
   */

  QFAIL("Not complete");
}

void SortProxyModelTest::sortStringNonLocalAwareBenchmark()
{
  QFETCH(int, n);
  VariantTableModel model;
  SortProxyModel proxyModel;
  /*
   * Setup models
   */
  proxyModel.setSourceModel(&model);
  proxyModel.setDynamicSortFilter(false);
  proxyModel.setSortLocaleAware(false);
  proxyModel.addColumnToSortOrder(1, Qt::AscendingOrder);
  proxyModel.addColumnToSortOrder(0, Qt::AscendingOrder);
  proxyModel.addColumnToSortOrder(3, Qt::AscendingOrder);
  model.populate(n, 5);
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
  proxyModel.setSourceModel(&model);
  proxyModel.setDynamicSortFilter(false);
  proxyModel.setSortLocaleAware(true);
  proxyModel.addColumnToSortOrder(1, StringNumericMode::Natural, Qt::AscendingOrder);
  proxyModel.addColumnToSortOrder(0, StringNumericMode::Natural, Qt::AscendingOrder);
  proxyModel.addColumnToSortOrder(3, StringNumericMode::Natural, Qt::AscendingOrder);
  model.populate(n, 5);
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

void SortProxyModelTest::dynamicSortEventSingleColumnTest()
{
  VariantTableModel model;
  SortProxyModel proxyModel;

  /*
   * Setup models
   */
  proxyModel.setSourceModel(&model);
  proxyModel.setDynamicSortFilter(false);
  model.resize(5, 5);
  qDebug() << "TEST: set data 0, {9,1,3,6,4}";
  model.populateColumn(0, {9,1,3,6,4});
  QVERIFY(!isModelColumnSortedNumeric(proxyModel, 0));
  /*
   * When enabling dynamicSortFilter property, sort must be processed
   * -> Note: without having to call sort()
   */
  qDebug() << "TEST: setDynamicSortFilter(true)";
  proxyModel.setDynamicSortFilter(true);
  QVERIFY(isModelColumnSortedNumeric(proxyModel, 0));
  /*
   * Check catching at modelReset()
   */
  
  /*
   * Check data change:
   *  - When change in column == QSortFilterProxyModel::sortColumn()
   *  - When change in column != QSortFilterProxyModel::sortColumn()
   */
  

  QFAIL("Not complete");
}

void SortProxyModelTest::dynamicSortEventMultiColumnTest()
{
  QFAIL("Not complete");
}

void SortProxyModelTest::viewsTest()
{
  QFAIL("Not complete");
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
  proxyView.setSortingEnabled(true);
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

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}

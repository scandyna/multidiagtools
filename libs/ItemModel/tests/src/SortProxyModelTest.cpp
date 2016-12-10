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
#include <iterator>
#include <vector>
#include <array>
#include <algorithm>

namespace ItemModel = Mdt::ItemModel;
using ItemModel::VariantTableModel;
using ItemModel::SortProxyModel;

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

  list.addColumn(1, Qt::AscendingOrder);
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

  ColumnSortStringAttributes csa(1, Qt::CaseSensitive, true);
  QCOMPARE(csa.column(), 1);
  QVERIFY(csa.caseSensitivity() == Qt::CaseSensitive);
  QVERIFY(csa.numericMode());
}

void SortProxyModelTest::columnSortStringAttributesListTest()
{
  using ItemModel::ColumnSortStringAttributes;
  using ItemModel::ColumnSortStringAttributesList;

  ColumnSortStringAttributesList list;

  list.addColumn(2, Qt::CaseSensitive, true);
  list.addColumn(1, Qt::CaseInsensitive, false);
  list.addColumn(3, Qt::CaseSensitive, false);
  // Check attributes for column 1
  auto csa = list.attributesForColumn(1);
  QCOMPARE(csa.column(), 1);
  QVERIFY(csa.caseSensitivity() == Qt::CaseInsensitive);
  QVERIFY(!csa.numericMode());
  // Check attributes for column 2
  csa = list.attributesForColumn(2);
  QCOMPARE(csa.column(), 2);
  QVERIFY(csa.caseSensitivity() == Qt::CaseSensitive);
  QVERIFY(csa.numericMode());
  // Check attributes for column 3
  csa = list.attributesForColumn(3);
  QCOMPARE(csa.column(), 3);
  QVERIFY(csa.caseSensitivity() == Qt::CaseSensitive);
  QVERIFY(!csa.numericMode());
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

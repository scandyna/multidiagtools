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
  QFAIL("Not complete");
}

void SortProxyModelTest::columnSortOrderListBenchmark()
{
  QFAIL("Not complete");
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

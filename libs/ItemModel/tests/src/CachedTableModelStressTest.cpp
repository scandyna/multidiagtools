/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#include "CachedTableModelStressTest.h"
#include "PersonTableModel.h"
#include "Mdt/ItemModel/AbstractCachedTableModel.h"
#include "Mdt/Container/VariantRecord.h"
#include "Mdt/TestLib/ItemModel.h"
#include <QVariantList>
#include <QTableView>

using namespace Mdt::ItemModel;
using namespace Mdt::TestLib;
using Mdt::Container::VariantRecord;

void CachedTableModelStressTest::initTestCase()
{
  qRegisterMetaType<Mdt::Container::VariantRecord>();
}

/*
 * Tests
 */

template<typename Model>
void fetchAllAsyncTest(int rowCount, QAbstractItemView *view = nullptr)
{
  Model model;
  model.setCachedRowCountLimit(rowCount);
  QCOMPARE(model.columnCount(), 2);

  AsyncPersonRecordGenerator gen(rowCount);
  QObject::connect(&gen, &AsyncPersonRecordGenerator::newRecordAvailable, &model, &Model::fromBackendAppendRecord);

  if(view != nullptr){
    view->setModel(&model);
    view->show();
  }

  QCOMPARE(model.rowCount(), 0);
  gen.start();
  QTRY_COMPARE_WITH_TIMEOUT(model.rowCount(), rowCount, 5000);
  gen.wait();
}

void fetchAllAsyncTestBuildDataSet()
{
  QTest::addColumn<int>("rowCount");

  QTest::newRow("10 rows") << 10;
  QTest::newRow("100 rows") << 100;
  QTest::newRow("1'000 rows") << 1000;
  QTest::newRow("10'000 rows") << 10000;
  QTest::newRow("100'000 rows") << 100000;
  QTest::newRow("1'000'000 rows") << 1000000;
}

void CachedTableModelStressTest::readOnlyFetchAllAsyncTest()
{
  QFETCH(int, rowCount);

  fetchAllAsyncTest<ListPersonTableModel>(rowCount);
}

void CachedTableModelStressTest::readOnlyFetchAllAsyncTest_data()
{
  fetchAllAsyncTestBuildDataSet();
}

void CachedTableModelStressTest::editableFetchAllAsyncTest()
{
  QFETCH(int, rowCount);

  fetchAllAsyncTest<EditPersonTableModel>(rowCount);
}

void CachedTableModelStressTest::editableFetchAllAsyncTest_data()
{
  fetchAllAsyncTestBuildDataSet();
}

void CachedTableModelStressTest::readOnlyFetchAllAsyncTableViewTest()
{
  QFETCH(int, rowCount);

  QTableView view;

  fetchAllAsyncTest<ListPersonTableModel>(rowCount, &view);
}

void CachedTableModelStressTest::readOnlyFetchAllAsyncTableViewTest_data()
{
  fetchAllAsyncTestBuildDataSet();
}

void CachedTableModelStressTest::editableFetchAllAsyncTableViewTest()
{
  QFETCH(int, rowCount);

  QTableView view;

  fetchAllAsyncTest<EditPersonTableModel>(rowCount, &view);
}

void CachedTableModelStressTest::editableFetchAllAsyncTableViewTest_data()
{
  fetchAllAsyncTestBuildDataSet();
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  CachedTableModelStressTest test;

  return QTest::qExec(&test, argc, argv);
}

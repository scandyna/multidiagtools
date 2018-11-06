/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "AsyncCacheTest.h"
#include "AsyncPersonCache.h"

#include "Mdt/Entity/AbstractReadOnlyCacheTableModel.h"
#include <QTableView>

void AsyncCacheTest::sandbox()
{
  QTableView view;
  Mdt::Entity::AbstractReadOnlyCacheTableModel model;
  AsyncPersonCache cache;

  cache.setBackendLatency(250);
  model.setCache(&cache);
  view.setModel(&model);
  view.show();

  populatePersonStorage(cache, {"A","B","C","D","E","F"});
  cache.fetchAll();

  while(view.isVisible()){
    QTest::qWait(500);
  }
}

/*
 * Tests
 */

void AsyncCacheTest::readOnlyFetchAllTest()
{
  AsyncPersonCache cache;
  QCOMPARE(cache.rowCount(), 0);
  QCOMPARE(cache.columnCount(), 2);

  populatePersonStorage(cache, {"A","B"});
  QVERIFY(cache.fetchAll());
  QTRY_COMPARE_WITH_TIMEOUT(cache.rowCount(), 2, mTimeout);
  QCOMPARE(cache.data(0, 1), QVariant("A"));
  QCOMPARE(cache.data(1, 1), QVariant("B"));
}

void AsyncCacheTest::updateSubmitUpdateTest()
{
  /*
   * Initial state after editing in cache:
   * ---------
   * |id|name|
   * ---------
   * | 1| A  |
   * ---------
   * | 2| eB | edited
   * ---------
   * | 3| C  |
   * ---------
   */

  /*
   * After submitChanges:
   * ---------
   * |id|name|
   * ---------
   * | 1| A  |
   * ---------
   * | 2| eB | edited / pending ?
   * ---------
   * | 3| C  |
   * ---------
   */

  /*
   * submitChanges returns immediately.
   * We should not be able to change pending records,
   * but the other can be edited:
   * ---------
   * |id|name|
   * ---------
   * | 1| A  |
   * ---------
   * | 2| eB | edited / pending ?
   * ---------
   * | 3| eC | edited
   * ---------
   */

  /*
   * we should be able to call submitChanges again:
   * ---------
   * |id|name|
   * ---------
   * | 1| A  |
   * ---------
   * | 2| eB | edited / pending ?
   * ---------
   * | 3| eC | edited / pending ?
   * ---------
   */

  /*
   * Wait until change have been set to the backend and check:
   * ---------
   * |id|name|
   * ---------
   * | 1| A  |
   * ---------
   * | 2| eB |
   * ---------
   * | 3| eC |
   * ---------
   */

  QFAIL("Not complete");
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  /// Mdt::CoreApplication app(argc, argv);
  AsyncCacheTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}

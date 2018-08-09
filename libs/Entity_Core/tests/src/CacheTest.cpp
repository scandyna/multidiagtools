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
#include "CacheTest.h"
#include "PersonCache.h"
#include "Mdt/Entity/AbstractReadOnlyCache.h"
#include <QString>
#include <QStringList>
#include <QSignalSpy>
#include <QVariantList>
// #include <vector>
// #include <algorithm>

using namespace Mdt::Entity;
using Mdt::Container::TableCacheOperation;

/*
 * Tests
 */

template<typename Cache>
void rowCountLimitTest()
{
  Cache cache;
  QVERIFY(cache.cachedRowCountLimit() > 100);
  cache.setCachedRowCountLimit(10);
  QCOMPARE(cache.cachedRowCountLimit(), 10);

  QFAIL("Not complete");
}

void CacheTest::readOnlyRowCountLimitTest()
{
  rowCountLimitTest<PersonCache>();
}

void CacheTest::editableRowCountLimitTest()
{
  rowCountLimitTest<EditPersonCache>();
}

template<typename Cache>
void fetchAllTest()
{
  Cache pc;
  QCOMPARE(pc.rowCount(), 0);
  QCOMPARE(pc.columnCount(), 2);
  /*
   * Storage has no data
   */
  QVERIFY(pc.fetchAll());
  QCOMPARE(pc.rowCount(), 0);
  /*
   * Storage has less data than limit
   */
  pc.setCachedRowCountLimit(500);
  populatePersonStorage(pc, {"A","B","C"});
  QCOMPARE(pc.rowCount(), 0);
  QCOMPARE(pc.columnCount(), 2);
  QVERIFY(pc.fetchAll());
  QCOMPARE(pc.rowCount(), 3);
  QCOMPARE(pc.columnCount(), 2);
  QCOMPARE(pc.data(0, 1), QVariant("A"));
  QCOMPARE(pc.data(1, 1), QVariant("B"));
  QCOMPARE(pc.data(2, 1), QVariant("C"));
  /*
   * Storage has more data than limit
   */
  pc.setCachedRowCountLimit(2);
  populatePersonStorage(pc, {"A","B","C"});
  QVERIFY(pc.fetchAll());
  QCOMPARE(pc.rowCount(), 2);
  QCOMPARE(pc.data(0, 1), QVariant("A"));
  QCOMPARE(pc.data(1, 1), QVariant("B"));
  /*
   * Storage has the same amount of data than limit
   */
  pc.setCachedRowCountLimit(2);
  populatePersonStorage(pc, {"A","B"});
  QVERIFY(pc.fetchAll());
  QCOMPARE(pc.rowCount(), 2);
  QCOMPARE(pc.data(0, 1), QVariant("A"));
  QCOMPARE(pc.data(1, 1), QVariant("B"));
}

void CacheTest::readOnlyFetchAllTest()
{
  fetchAllTest<PersonCache>();
}

void CacheTest::editableFetchAllTest()
{
  fetchAllTest<EditPersonCache>();
}

template<typename Cache>
void fetchAllSignalTest()
{
  Cache pc;
  QSignalSpy cacheAboutToBeResetSpy(&pc, &Cache::cacheAboutToBeReset);
  QSignalSpy cacheResetSpy(&pc, &Cache::cacheReset);
  QVERIFY(cacheAboutToBeResetSpy.isValid());
  QVERIFY(cacheResetSpy.isValid());

  populatePersonStorage(pc, {"A","B","C"});
  QCOMPARE(cacheAboutToBeResetSpy.count(), 0);
  QCOMPARE(cacheResetSpy.count(), 0);
  QCOMPARE(pc.rowCount(), 0);

  QVERIFY(pc.fetchAll());
  QCOMPARE(cacheAboutToBeResetSpy.count(), 1);
  QCOMPARE(cacheResetSpy.count(), 1);
  QCOMPARE(pc.rowCount(), 3);
}

void CacheTest::readOnlyFetchAllSignalTest()
{
  fetchAllSignalTest<PersonCache>();
}

void CacheTest::editableFetchAllSignalTest()
{
  fetchAllSignalTest<EditPersonCache>();
}

template<typename Cache>
void appendRecordFromBackendTest()
{
  VariantRecord record(2);
  Cache pc;
  QCOMPARE(pc.rowCount(), 0);

  record.setValue(0, 1);
  record.setValue(1, "A");
  pc.appendRecordFromBackend(record);
  QCOMPARE(pc.rowCount(), 1);
  QCOMPARE(pc.data(0, 0), QVariant(1));
  QCOMPARE(pc.data(0, 1), QVariant("A"));
}

void CacheTest::readOnlyAppendRecordFromBackendTest()
{
  appendRecordFromBackendTest<PersonCache>();
}

void CacheTest::editableAppendRecordFromBackendTest()
{
  appendRecordFromBackendTest<EditPersonCache>();
}

template<typename Cache>
void appendRecordFromBackendSignalTest()
{

  QFAIL("Not complete");
}

void CacheTest::readOnlyAppendRecordFromBackendSignalTest()
{
  appendRecordFromBackendSignalTest<PersonCache>();
}

void CacheTest::editableAppendRecordFromBackendSignalTest()
{
  appendRecordFromBackendSignalTest<EditPersonCache>();
}

void CacheTest::setDataTest()
{
  EditPersonCache epc;
  populatePersonStorage(epc, {"A","B","C"});
  QVERIFY(epc.fetchAll());

  QCOMPARE(epc.rowCount(), 3);
  QCOMPARE(epc.data(0, 1), QVariant("A"));
  QCOMPARE(epc.data(1, 1), QVariant("B"));
  QCOMPARE(epc.data(2, 1), QVariant("C"));
  QCOMPARE(epc.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(epc.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(epc.operationAtRow(2), TableCacheOperation::None);

  epc.setData(0, 1, "eA");
  QCOMPARE(epc.rowCount(), 3);
  QCOMPARE(epc.data(0, 1), QVariant("eA"));
  QCOMPARE(epc.data(1, 1), QVariant("B"));
  QCOMPARE(epc.data(2, 1), QVariant("C"));
  QCOMPARE(epc.operationAtRow(0), TableCacheOperation::Update);
  QCOMPARE(epc.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(epc.operationAtRow(2), TableCacheOperation::None);
}

void CacheTest::setDataSignalTest()
{
  EditPersonCache epc;
  populatePersonStorage(epc, {"A","B","C"});
  QVERIFY(epc.fetchAll());

  QVariantList arguments;
  QSignalSpy dataAtRowsChangedSpy(&epc, &EditPersonCache::dataAtRowsChanged);
  QVERIFY(dataAtRowsChangedSpy.isValid());

  QCOMPARE(dataAtRowsChangedSpy.count(), 0);
  epc.setData(1, 1, "eB");
  QCOMPARE(dataAtRowsChangedSpy.count(), 1);
  arguments = dataAtRowsChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(1)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(1)); // lastRow
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  CacheTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}

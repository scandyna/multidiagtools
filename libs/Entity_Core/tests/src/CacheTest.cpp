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
  QSignalSpy rowsAboutToBeInsertedSpy(&pc, &PersonCache::rowsAboutToBeInserted);
  QVERIFY(rowsAboutToBeInsertedSpy.isValid());
  QSignalSpy rowsInsertedSpy(&pc, &PersonCache::rowsInserted);
  QVERIFY(rowsInsertedSpy.isValid());

  populatePersonStorage(pc, {"A","B","C"});
  QCOMPARE(cacheAboutToBeResetSpy.count(), 0);
  QCOMPARE(cacheResetSpy.count(), 0);
  QCOMPARE(pc.rowCount(), 0);
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);

  QVERIFY(pc.fetchAll());
  QCOMPARE(cacheAboutToBeResetSpy.count(), 1);
  QCOMPARE(cacheResetSpy.count(), 1);
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
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

void CacheTest::readOnlySetDataFromBackendTest()
{
  PersonCache cache;
  populatePersonStorage(cache, {"A","B","C"});
  QVERIFY(cache.fetchAll());

  QCOMPARE(cache.rowCount(), 3);
  QCOMPARE(cache.data(0, 1), QVariant("A"));
  QCOMPARE(cache.data(1, 1), QVariant("B"));
  QCOMPARE(cache.data(2, 1), QVariant("C"));

  cache.fromBackendSetData(0, 1, "eA");
  QCOMPARE(cache.rowCount(), 3);
  QCOMPARE(cache.data(0, 1), QVariant("eA"));
  QCOMPARE(cache.data(1, 1), QVariant("B"));
  QCOMPARE(cache.data(2, 1), QVariant("C"));
}

void CacheTest::editableSetDataFromBackendTest()
{
  EditPersonCache cache;
  populatePersonStorage(cache, {"A","B","C"});
  QVERIFY(cache.fetchAll());

  QCOMPARE(cache.rowCount(), 3);
  QCOMPARE(cache.data(0, 1), QVariant("A"));
  QCOMPARE(cache.data(1, 1), QVariant("B"));
  QCOMPARE(cache.data(2, 1), QVariant("C"));
  QCOMPARE(cache.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(cache.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(cache.operationAtRow(2), TableCacheOperation::None);

  cache.fromBackendSetData(0, 1, "eA");
  QCOMPARE(cache.rowCount(), 3);
  QCOMPARE(cache.data(0, 1), QVariant("eA"));
  QCOMPARE(cache.data(1, 1), QVariant("B"));
  QCOMPARE(cache.data(2, 1), QVariant("C"));
  QCOMPARE(cache.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(cache.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(cache.operationAtRow(2), TableCacheOperation::None);
}

void CacheTest::readOnlySetDataFromBackendSignalTest()
{
  PersonCache cache;
  populatePersonStorage(cache, {"A","B","C"});
  QVERIFY(cache.fetchAll());

  QVariantList arguments;
  QSignalSpy dataAtRowsChangedSpy(&cache, &PersonCache::dataAtRowsChanged);
  QVERIFY(dataAtRowsChangedSpy.isValid());

  QCOMPARE(dataAtRowsChangedSpy.count(), 0);
  cache.fromBackendSetData(1, 1, "eB");
  QCOMPARE(dataAtRowsChangedSpy.count(), 1);
  arguments = dataAtRowsChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(1)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(1)); // lastRow
}

void CacheTest::editableSetDataFromBackendSignalTest()
{
  EditPersonCache cache;
  populatePersonStorage(cache, {"A","B","C"});
  QVERIFY(cache.fetchAll());

  QVariantList arguments;
  QSignalSpy dataAtRowsChangedSpy(&cache, &EditPersonCache::dataAtRowsChanged);
  QVERIFY(dataAtRowsChangedSpy.isValid());

  QCOMPARE(dataAtRowsChangedSpy.count(), 0);
  cache.fromBackendSetData(1, 1, "eB");
  QCOMPARE(dataAtRowsChangedSpy.count(), 1);
  arguments = dataAtRowsChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(1)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(1)); // lastRow
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
  QSignalSpy operationAtRowsChangedSpy(&epc, &EditPersonCache::operationAtRowsChanged);
  QVERIFY(operationAtRowsChangedSpy.isValid());

  QCOMPARE(dataAtRowsChangedSpy.count(), 0);
  QCOMPARE(operationAtRowsChangedSpy.count(), 0);
  epc.setData(1, 1, "eB");
  QCOMPARE(dataAtRowsChangedSpy.count(), 1);
  arguments = dataAtRowsChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(1)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(1)); // lastRow
  QCOMPARE(operationAtRowsChangedSpy.count(), 1);
  arguments = operationAtRowsChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(1)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(1)); // lastRow
}

void CacheTest::setDataAndSubmitTest()
{
  EditPersonCache cache;
  populatePersonStorage(cache, {"A","B","C"});
  QVERIFY(cache.fetchAll());

  QCOMPARE(cache.rowCount(), 3);
  QCOMPARE(cache.data(0, 1), QVariant("A"));
  QCOMPARE(cache.data(1, 1), QVariant("B"));
  QCOMPARE(cache.data(2, 1), QVariant("C"));
  QCOMPARE(cache.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(cache.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(cache.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(cache.storageRowCount(), 3);
  QCOMPARE(cache.recordFromStorage(0).name, QString("A"));
  QCOMPARE(cache.recordFromStorage(1).name, QString("B"));
  QCOMPARE(cache.recordFromStorage(2).name, QString("C"));

  cache.setData(0, 1, "eA");
  cache.setData(2, 1, "eC");
  QCOMPARE(cache.rowCount(), 3);
  QCOMPARE(cache.data(0, 1), QVariant("eA"));
  QCOMPARE(cache.data(1, 1), QVariant("B"));
  QCOMPARE(cache.data(2, 1), QVariant("eC"));
  QCOMPARE(cache.operationAtRow(0), TableCacheOperation::Update);
  QCOMPARE(cache.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(cache.operationAtRow(2), TableCacheOperation::Update);
  QCOMPARE(cache.storageRowCount(), 3);
  QCOMPARE(cache.recordFromStorage(0).name, QString("A"));
  QCOMPARE(cache.recordFromStorage(1).name, QString("B"));
  QCOMPARE(cache.recordFromStorage(2).name, QString("C"));

  QVERIFY(cache.submitChanges());
  QCOMPARE(cache.rowCount(), 3);
  QCOMPARE(cache.data(0, 1), QVariant("eA"));
  QCOMPARE(cache.data(1, 1), QVariant("B"));
  QCOMPARE(cache.data(2, 1), QVariant("eC"));
  QCOMPARE(cache.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(cache.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(cache.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(cache.storageRowCount(), 3);
  QCOMPARE(cache.recordFromStorage(0).name, QString("eA"));
  QCOMPARE(cache.recordFromStorage(1).name, QString("B"));
  QCOMPARE(cache.recordFromStorage(2).name, QString("eC"));
}

void CacheTest::setDataAndSubmitSignalTest()
{
  EditPersonCache cache;
  populatePersonStorage(cache, {"A","B","C"});
  QVERIFY(cache.fetchAll());

  QVariantList arguments;
  QSignalSpy dataAtRowsChangedSpy(&cache, &EditPersonCache::dataAtRowsChanged);
  QVERIFY(dataAtRowsChangedSpy.isValid());
  QSignalSpy operationAtRowsChangedSpy(&cache, &EditPersonCache::operationAtRowsChanged);
  QVERIFY(operationAtRowsChangedSpy.isValid());

  QCOMPARE(dataAtRowsChangedSpy.count(), 0);
  QCOMPARE(operationAtRowsChangedSpy.count(), 0);
  cache.setData(1, 1, "eB");
  QCOMPARE(dataAtRowsChangedSpy.count(), 1);
  arguments = dataAtRowsChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(1)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(1)); // lastRow
  QCOMPARE(operationAtRowsChangedSpy.count(), 1);
  arguments = operationAtRowsChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(1)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(1)); // lastRow

  QCOMPARE(dataAtRowsChangedSpy.count(), 0);
  QCOMPARE(operationAtRowsChangedSpy.count(), 0);
  QVERIFY(cache.submitChanges());
  QCOMPARE(dataAtRowsChangedSpy.count(), 1);
  arguments = dataAtRowsChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(1)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(1)); // lastRow
  QCOMPARE(operationAtRowsChangedSpy.count(), 1);
  arguments = operationAtRowsChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(1)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(1)); // lastRow
}

void CacheTest::readOnlySetRecordFromBackendTest()
{
  VariantRecord record(2);
  PersonCache cache;

  QCOMPARE(cache.rowCount(), 0);
  record.setValue(1, "New");
  cache.fromBackendInsertRecords(0, 2, record);
  QCOMPARE(cache.rowCount(), 2);
  QCOMPARE(cache.data(0, 1), QVariant("New"));
  QCOMPARE(cache.data(1, 1), QVariant("New"));

  record.setValue(0, 1);
  record.setValue(1, "A");
  cache.fromBackendSetRecord(0, record);
  QCOMPARE(cache.data(0, 0), QVariant(1));
  QCOMPARE(cache.data(0, 1), QVariant("A"));
  QCOMPARE(cache.data(1, 1), QVariant("New"));
}

void CacheTest::editableSetRecordFromBackendTest()
{
  VariantRecord record(2);
  EditPersonCache cache;

  QCOMPARE(cache.rowCount(), 0);
  record.setValue(1, "New");
  cache.insertRecords(0, 2, record);
  QCOMPARE(cache.rowCount(), 2);
  QCOMPARE(cache.data(0, 1), QVariant("New"));
  QCOMPARE(cache.data(1, 1), QVariant("New"));
  QCOMPARE(cache.operationAtRow(0), TableCacheOperation::Insert);
  QCOMPARE(cache.operationAtRow(1), TableCacheOperation::Insert);

  record.setValue(0, 1);
  record.setValue(1, "A");
  cache.fromBackendSetRecord(0, record);
  QCOMPARE(cache.data(0, 0), QVariant(1));
  QCOMPARE(cache.data(0, 1), QVariant("A"));
  QCOMPARE(cache.data(1, 1), QVariant("New"));
  QCOMPARE(cache.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(cache.operationAtRow(1), TableCacheOperation::Insert);
}

void CacheTest::readOnlySetRecordFromBackendSignalTest()
{
  VariantRecord record(2);
  PersonCache cache;
  populatePersonStorage(cache, {"A","B","C"});
  QVERIFY(cache.fetchAll());

  QVariantList arguments;
  QSignalSpy dataAtRowsChangedSpy(&cache, &PersonCache::dataAtRowsChanged);
  QVERIFY(dataAtRowsChangedSpy.isValid());

  QCOMPARE(dataAtRowsChangedSpy.count(), 0);
  cache.fromBackendSetRecord(0, record);
  QCOMPARE(dataAtRowsChangedSpy.count(), 1);
  arguments = dataAtRowsChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(0)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(0)); // lastRow
}

void CacheTest::editableSetRecordFromBackendSignalTest()
{
  EditPersonCache cache;
  populatePersonStorage(cache, {"A","B","C"});
  QVERIFY(cache.fetchAll());

  QVariantList arguments;
  QSignalSpy dataAtRowsChangedSpy(&cache, &EditPersonCache::dataAtRowsChanged);
  QVERIFY(dataAtRowsChangedSpy.isValid());
  QSignalSpy operationAtRowsChangedSpy(&cache, &EditPersonCache::operationAtRowsChanged);
  QVERIFY(operationAtRowsChangedSpy.isValid());

  QCOMPARE(dataAtRowsChangedSpy.count(), 0);
  QCOMPARE(operationAtRowsChangedSpy.count(), 0);
  cache.fromBackendSetRecord(1, VariantRecord(2));
  QCOMPARE(dataAtRowsChangedSpy.count(), 1);
  arguments = dataAtRowsChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(1)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(1)); // lastRow
  QCOMPARE(operationAtRowsChangedSpy.count(), 1);
  arguments = operationAtRowsChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(1)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(1)); // lastRow
}

template<typename Cache>
void insertRecordsFromBackendTest()
{
  VariantRecord record(2);
  Cache cache;

  QCOMPARE(cache.rowCount(), 0);
  record.setValue(1, "New");
  cache.fromBackendInsertRecords(0, 2, record);
  QCOMPARE(cache.rowCount(), 2);
  QCOMPARE(cache.data(0, 1), QVariant("New"));
  QCOMPARE(cache.data(1, 1), QVariant("New"));
  /*
   * Near full cache
   */
  record.setValue(1, "Last");
  cache.setCachedRowCountLimit(3);
  cache.fromBackendInsertRecords(2, 1, record);
  QCOMPARE(cache.rowCount(), 3);
  QCOMPARE(cache.data(0, 1), QVariant("New"));
  QCOMPARE(cache.data(1, 1), QVariant("New"));
  QCOMPARE(cache.data(2, 1), QVariant("Last"));
}

void CacheTest::readOnlyInsertRecordsFromBackendTest()
{
  insertRecordsFromBackendTest<PersonCache>();
}

void CacheTest::editableInsertRecordsFromBackendTest()
{
  insertRecordsFromBackendTest<EditPersonCache>();
}

template<typename Cache>
void insertRecordsFromBackendSignalTest()
{
  VariantRecord record(2);
  Cache cache;
  QVariantList arguments;
  QSignalSpy rowsAboutToBeInsertedSpy(&cache, &Cache::rowsAboutToBeInserted);
  QVERIFY(rowsAboutToBeInsertedSpy.isValid());
  QSignalSpy rowsInsertedSpy(&cache, &Cache::rowsInserted);
  QVERIFY(rowsInsertedSpy.isValid());

  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
  cache.fromBackendInsertRecords(0, 2, record);
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
  arguments = rowsAboutToBeInsertedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(0)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(1)); // lastRow
  QCOMPARE(rowsInsertedSpy.count(), 1);
}

void CacheTest::readOnlyInsertRecordsFromBackendSignalTest()
{
  insertRecordsFromBackendSignalTest<PersonCache>();
}

void CacheTest::editableInsertRecordsFromBackendSignalTest()
{
  insertRecordsFromBackendSignalTest<EditPersonCache>();
}

template<typename Cache>
void appendRecordFromBackendTest()
{
  VariantRecord record(2);
  Cache pc;
  QCOMPARE(pc.rowCount(), 0);

  record.setValue(0, 1);
  record.setValue(1, "A");
  pc.fromBackendAppendRecord(record);
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
  VariantRecord record(2);
  Cache cache;
  QVariantList arguments;
  QSignalSpy rowsAboutToBeInsertedSpy(&cache, &Cache::rowsAboutToBeInserted);
  QVERIFY(rowsAboutToBeInsertedSpy.isValid());
  QSignalSpy rowsInsertedSpy(&cache, &Cache::rowsInserted);
  QVERIFY(rowsInsertedSpy.isValid());

  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
  cache.fromBackendAppendRecord(record);
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
  arguments = rowsAboutToBeInsertedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(0)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(0)); // lastRow
  QCOMPARE(rowsInsertedSpy.count(), 1);
}

void CacheTest::readOnlyAppendRecordFromBackendSignalTest()
{
  appendRecordFromBackendSignalTest<PersonCache>();
}

void CacheTest::editableAppendRecordFromBackendSignalTest()
{
  appendRecordFromBackendSignalTest<EditPersonCache>();
}

void CacheTest::insertRecordsTest()
{
  VariantRecord record(2);
  EditPersonCache cache;

  QCOMPARE(cache.rowCount(), 0);
  record.setValue(1, "New");
  cache.insertRecords(0, 2, record);
  QCOMPARE(cache.rowCount(), 2);
  QCOMPARE(cache.data(0, 1), QVariant("New"));
  QCOMPARE(cache.data(1, 1), QVariant("New"));
  QCOMPARE(cache.operationAtRow(0), TableCacheOperation::Insert);
  QCOMPARE(cache.operationAtRow(1), TableCacheOperation::Insert);
  /*
   * Near full cache
   */
  record.setValue(1, "Last");
  cache.setCachedRowCountLimit(3);
  cache.insertRecords(2, 1, record);
  QCOMPARE(cache.rowCount(), 3);
  QCOMPARE(cache.data(0, 1), QVariant("New"));
  QCOMPARE(cache.data(1, 1), QVariant("New"));
  QCOMPARE(cache.data(2, 1), QVariant("Last"));
  QCOMPARE(cache.operationAtRow(0), TableCacheOperation::Insert);
  QCOMPARE(cache.operationAtRow(1), TableCacheOperation::Insert);
  QCOMPARE(cache.operationAtRow(2), TableCacheOperation::Insert);
}

void CacheTest::insertRecordsSignalTest()
{
  VariantRecord record(2);
  EditPersonCache cache;
  QVariantList arguments;
  QSignalSpy rowsAboutToBeInsertedSpy(&cache, &EditPersonCache::rowsAboutToBeInserted);
  QVERIFY(rowsAboutToBeInsertedSpy.isValid());
  QSignalSpy rowsInsertedSpy(&cache, &EditPersonCache::rowsInserted);
  QVERIFY(rowsInsertedSpy.isValid());
  QSignalSpy operationAtRowsChangedSpy(&cache, &EditPersonCache::operationAtRowsChanged);
  QVERIFY(operationAtRowsChangedSpy.isValid());

  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
  QCOMPARE(operationAtRowsChangedSpy.count(), 0);
  cache.insertRecords(0, 2, record);
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
  arguments = rowsAboutToBeInsertedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(0)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(1)); // lastRow
  QCOMPARE(rowsInsertedSpy.count(), 1);
  QCOMPARE(operationAtRowsChangedSpy.count(), 1);
  arguments = operationAtRowsChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(0)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(1)); // lastRow
}

void CacheTest::insertRecordsAndSubmitTest()
{
  VariantRecord record(2);
  EditPersonCache cache;

  QCOMPARE(cache.rowCount(), 0);
  QCOMPARE(cache.storageRowCount(), 0);
  record.setValue(1, "New");
  cache.insertRecords(0, 2, record);
  QCOMPARE(cache.rowCount(), 2);
  QCOMPARE(cache.data(0, 0), QVariant());
  QCOMPARE(cache.data(0, 1), QVariant("New"));
  QCOMPARE(cache.data(1, 0), QVariant());
  QCOMPARE(cache.data(1, 1), QVariant("New"));
  QCOMPARE(cache.operationAtRow(0), TableCacheOperation::Insert);
  QCOMPARE(cache.operationAtRow(1), TableCacheOperation::Insert);
  QCOMPARE(cache.storageRowCount(), 0);

  QVERIFY(cache.submitChanges());
  QCOMPARE(cache.rowCount(), 2);
  QCOMPARE(cache.data(0, 0), QVariant(1));
  QCOMPARE(cache.data(0, 1), QVariant("New"));
  QCOMPARE(cache.data(1, 0), QVariant(2));
  QCOMPARE(cache.data(1, 1), QVariant("New"));
  QCOMPARE(cache.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(cache.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(cache.storageRowCount(), 2);
}

void CacheTest::insertRecordsAndSubmitSignalTest()
{
  VariantRecord record(2);
  EditPersonCache cache;
  QVariantList arguments;
  QSignalSpy rowsAboutToBeInsertedSpy(&cache, &EditPersonCache::rowsAboutToBeInserted);
  QVERIFY(rowsAboutToBeInsertedSpy.isValid());
  QSignalSpy rowsInsertedSpy(&cache, &EditPersonCache::rowsInserted);
  QVERIFY(rowsInsertedSpy.isValid());
  QSignalSpy operationAtRowsChangedSpy(&cache, &EditPersonCache::operationAtRowsChanged);
  QVERIFY(operationAtRowsChangedSpy.isValid());

  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
  QCOMPARE(operationAtRowsChangedSpy.count(), 0);
  cache.insertRecords(0, 1, record);
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
  arguments = rowsAboutToBeInsertedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(0)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(0)); // lastRow
  QCOMPARE(rowsInsertedSpy.count(), 1);
  rowsInsertedSpy.clear();
  QCOMPARE(operationAtRowsChangedSpy.count(), 1);
  arguments = operationAtRowsChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(0)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(0)); // lastRow

  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
  QCOMPARE(operationAtRowsChangedSpy.count(), 0);
  QVERIFY(cache.submitChanges());
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
  QCOMPARE(operationAtRowsChangedSpy.count(), 1);
  arguments = operationAtRowsChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(0)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(0)); // lastRow
}

template<typename Cache>
void removeRowsFromBackendTest()
{
  Cache cache;
  populatePersonStorage(cache, {"A","B","C"});
  QVERIFY(cache.fetchAll());

  QCOMPARE(cache.rowCount(), 3);
  QCOMPARE(cache.data(0, 1), QVariant("A"));
  QCOMPARE(cache.data(1, 1), QVariant("B"));
  QCOMPARE(cache.data(2, 1), QVariant("C"));

  cache.fromBackendRemoveRows(0, 2);
  QCOMPARE(cache.rowCount(), 1);
  QCOMPARE(cache.data(0, 1), QVariant("C"));

  cache.fromBackendRemoveRows(0, 1);
  QCOMPARE(cache.rowCount(), 0);
}

void CacheTest::readOnlyRemoveRowsFromBackendTest()
{
  removeRowsFromBackendTest<PersonCache>();
}

void CacheTest::editableRemoveRowsFromBackendTest()
{
  removeRowsFromBackendTest<EditPersonCache>();
}

template<typename Cache>
void removeRowsFromBackendSignalTest()
{
  Cache cache;
  populatePersonStorage(cache, {"A","B","C"});
  QVERIFY(cache.fetchAll());
  QVariantList arguments;
  QSignalSpy rowsAboutToBeRemovedSpy(&cache, &Cache::rowsAboutToBeRemoved);
  QVERIFY(rowsAboutToBeRemovedSpy.isValid());
  QSignalSpy rowsRemovedSpy(&cache, &Cache::rowsRemoved);
  QVERIFY(rowsRemovedSpy.isValid());

  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);
  cache.fromBackendRemoveRows(1, 2);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
  arguments = rowsAboutToBeRemovedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(1)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(2)); // lastRow
  QCOMPARE(rowsRemovedSpy.count(), 1);
}

void CacheTest::readOnlyRemoveRowsFromBackendSignalTest()
{
  removeRowsFromBackendSignalTest<PersonCache>();
}

void CacheTest::editableRemoveRowsFromBackendSignalTest()
{
  removeRowsFromBackendSignalTest<EditPersonCache>();
}

void CacheTest::removeRowsAndSubmitTest()
{
  EditPersonCache cache;
  populatePersonStorage(cache, {"A","B","C"});
  QVERIFY(cache.fetchAll());

  QCOMPARE(cache.rowCount(), 3);
  QCOMPARE(cache.data(0, 1), QVariant("A"));
  QCOMPARE(cache.data(1, 1), QVariant("B"));
  QCOMPARE(cache.data(2, 1), QVariant("C"));
  QCOMPARE(cache.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(cache.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(cache.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(cache.storageRowCount(), 3);
  QCOMPARE(cache.recordFromStorage(0).name, QString("A"));
  QCOMPARE(cache.recordFromStorage(1).name, QString("B"));
  QCOMPARE(cache.recordFromStorage(2).name, QString("C"));

  cache.removeRows(1, 2);
  QCOMPARE(cache.rowCount(), 3);
  QCOMPARE(cache.data(0, 1), QVariant("A"));
  QCOMPARE(cache.data(1, 1), QVariant("B"));
  QCOMPARE(cache.data(2, 1), QVariant("C"));
  QCOMPARE(cache.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(cache.operationAtRow(1), TableCacheOperation::Delete);
  QCOMPARE(cache.operationAtRow(2), TableCacheOperation::Delete);
  QCOMPARE(cache.storageRowCount(), 3);
  QCOMPARE(cache.recordFromStorage(0).name, QString("A"));
  QCOMPARE(cache.recordFromStorage(1).name, QString("B"));
  QCOMPARE(cache.recordFromStorage(2).name, QString("C"));

  QVERIFY(cache.submitChanges());
  QCOMPARE(cache.rowCount(), 1);
  QCOMPARE(cache.data(0, 1), QVariant("A"));
  QCOMPARE(cache.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(cache.storageRowCount(), 1);
  QCOMPARE(cache.recordFromStorage(0).name, QString("A"));
}

void CacheTest::removeRowsAndSubmitSignalTest()
{
  EditPersonCache cache;
  populatePersonStorage(cache, {"A","B","C"});
  QVERIFY(cache.fetchAll());
  QVariantList arguments;
  QSignalSpy rowsAboutToBeRemovedSpy(&cache, &EditPersonCache::rowsAboutToBeRemoved);
  QVERIFY(rowsAboutToBeRemovedSpy.isValid());
  QSignalSpy rowsRemovedSpy(&cache, &EditPersonCache::rowsRemoved);
  QVERIFY(rowsRemovedSpy.isValid());
  QSignalSpy operationAtRowsChangedSpy(&cache, &EditPersonCache::operationAtRowsChanged);
  QVERIFY(operationAtRowsChangedSpy.isValid());

  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);
  QCOMPARE(operationAtRowsChangedSpy.count(), 0);
  cache.removeRows(1, 2);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);
  QCOMPARE(operationAtRowsChangedSpy.count(), 1);
  arguments = operationAtRowsChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(1)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(2)); // lastRow

  QVERIFY(cache.submitChanges());
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
  arguments = rowsAboutToBeRemovedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(1)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(2)); // lastRow
  QCOMPARE(rowsRemovedSpy.count(), 1);
  QCOMPARE(operationAtRowsChangedSpy.count(), 0);
}

void CacheTest::insertRemoveRecordsAndSubmitTest()
{

  QFAIL("Not complete");
}

void CacheTest::insertRemoveRecordsAndSubmitSignalTest()
{

  QFAIL("Not complete");
}

void CacheTest::setDataRemoveRecordsAnsSubmitTest()
{

  QFAIL("Not complete");
}

void CacheTest::setDataRemoveRecordsAnsSubmitSignalTest()
{

  QFAIL("Not complete");
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

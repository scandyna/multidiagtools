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
#include "AbstractCachedRepositoryTest.h"
#include "Mdt/Entity/AbstractCachedRepository.h"
#include "Mdt/Entity/IntegralUniqueIdTemplate.h"
#include "Mdt/Container/StlContainer.h"
#include <QStringList>
#include <QSignalSpy>
#include <QVariantList>
#include <vector>
#include <algorithm>

// #include <QDebug>

using namespace Mdt::Entity;
using Mdt::Container::TableCacheOperation;

class PersonId : public Mdt::Entity::IntegralUniqueIdTemplate<PersonId, int>
{
 public:

  using IntegralUniqueIdTemplate::IntegralUniqueIdTemplate;
};


class PersonData
{
 public:

  void setId(PersonId id)
  {
    mId = id;
  }

  PersonId id() const
  {
    return mId;
  }

  void setFirstName(const QString & name)
  {
    mFirstName = name.trimmed();
    Q_ASSERT(!firstName().isEmpty());
  }

  QString firstName() const
  {
    return mFirstName;
  }

 private:

  PersonId mId;
  QString mFirstName;
};

/*
 * Repository
 */

class AbstractPersonRepository : public Mdt::Entity::AbstractCachedRepository<PersonData>
{
 public:

//   virtual PersonData personById(PersonId id) const = 0;

  int columnCount() const override
  {
    return 2;
  }

  QVariant data(int row, int column) const override
  {
    switch(column){
      case 0:
        return constRecordAt(row).id().value();
      case 1:
        return constRecordAt(row).firstName();
    }
    return QVariant();
  }

 private:

  void setDataToCache(int row, int column, const QVariant& data) override
  {
    switch(column){
      case 0:
        recordAt(row).setId(data.toULongLong());
        break;
      case 1:
        recordAt(row).setFirstName(data.toString());
        break;
    }
  }
};

PersonData buildPerson(const QString & baseName)
{
  PersonData data;
  data.setFirstName("f" + baseName);
  return data;
}

class MemoryPersonRepository : public AbstractPersonRepository
{
 public:

  void populate(const QStringList & baseNames)
  {
    mMem.clear();
    for(const auto & baseName : baseNames){
      mMem.push_back( buildPerson(baseName) );
    }
  }

//   PersonData personById(PersonId id) const override
//   {
//     Q_ASSERT(!id.isNull());
//   }

  int storageRowCount() const
  {
    return mMem.size();
  }

  std::vector<PersonData> & storage()
  {
    return mMem;
  }

 private:

  bool fetchRecords(int count) override
  {
    const int n = std::min(count, storageRowCount());
    for(int i = 0; i < n; ++i){
      appendRecordToCache(mMem[i]);
    }
    return true;
  }

  void setAutoIdToCache(int row, const QVariant& id) override
  {
    recordAt(row).setId(id.toInt());
  }

  bool insertRecordToStorage(const PersonData & record, QVariant & autoId) override
  {
    auto recordCpy = record;
    auto id = getNextId();
    recordCpy.setId(id);
    mMem.push_back(recordCpy);
    autoId = id;
    return true;
  }

  bool removeRecordFromStorage(int row) override
  {
    Mdt::Container::removeFromContainer(mMem, row, 1);
    return true;
  }

  bool updateRecordInStorage(int row) override
  {
    mMem[row] = constRecordAt(row);
    return true;
  }

  int getNextId() const
  {
    const auto cmp = [](const PersonData & a, const PersonData & b){
      return a.id() < b.id();
    };
    const auto it = std::max_element(mMem.cbegin(), mMem.cend(), cmp);
    if(it == mMem.cend()){
      return 1;
    }
    return it->id().value() + 1;
  }

  std::vector<PersonData> mMem;
};

/*
 * Tests
 */

void AbstractCachedRepositoryTest::rowCountLimitTest()
{
  MemoryPersonRepository repository;
  QVERIFY(repository.cachedRowCountLimit() > 0);
  repository.setCachedRowCountLimit(10);
  QCOMPARE(repository.cachedRowCountLimit(), 10);
}

void AbstractCachedRepositoryTest::fetchAllTest()
{
  MemoryPersonRepository repository;
  repository.setCachedRowCountLimit(500);
  QCOMPARE(repository.rowCount(), 0);
  /*
   * Storage has no data
   */
  QVERIFY(repository.fetchAll());
  QCOMPARE(repository.rowCount(), 0);
  /*
   * Storage has less data than limit
   */
  repository.setCachedRowCountLimit(500);
  repository.populate({"A","B","C"});
  QCOMPARE(repository.rowCount(), 0);
  QVERIFY(repository.fetchAll());
  QCOMPARE(repository.rowCount(), 3);
  QCOMPARE(repository.constRecordAt(0).firstName(), QString("fA"));
  QCOMPARE(repository.constRecordAt(1).firstName(), QString("fB"));
  QCOMPARE(repository.constRecordAt(2).firstName(), QString("fC"));
  /*
   * Storage has more data than limit
   */
  repository.setCachedRowCountLimit(2);
  repository.populate({"A","B","C"});
  QVERIFY(repository.fetchAll());
  QCOMPARE(repository.rowCount(), 2);
  QCOMPARE(repository.constRecordAt(0).firstName(), QString("fA"));
  QCOMPARE(repository.constRecordAt(1).firstName(), QString("fB"));
  /*
   * Storage has the same amount of data than limit
   */
  repository.setCachedRowCountLimit(2);
  repository.populate({"A","B"});
  QVERIFY(repository.fetchAll());
  QCOMPARE(repository.rowCount(), 2);
  QCOMPARE(repository.constRecordAt(0).firstName(), QString("fA"));
  QCOMPARE(repository.constRecordAt(1).firstName(), QString("fB"));
}

void AbstractCachedRepositoryTest::fetchAllSignalTest()
{
  MemoryPersonRepository repository;
  QSignalSpy cacheAboutToBeResetSpy(&repository, &MemoryPersonRepository::cacheAboutToBeReset);
  QSignalSpy cacheResetSpy(&repository, &MemoryPersonRepository::cacheReset);
  QVERIFY(cacheAboutToBeResetSpy.isValid());
  QVERIFY(cacheResetSpy.isValid());

  repository.populate({"A","B","C"});
  QCOMPARE(cacheAboutToBeResetSpy.count(), 0);
  QCOMPARE(cacheResetSpy.count(), 0);
  QCOMPARE(repository.rowCount(), 0);

  QVERIFY(repository.fetchAll());
  QCOMPARE(cacheAboutToBeResetSpy.count(), 1);
  QCOMPARE(cacheResetSpy.count(), 1);
  QCOMPARE(repository.rowCount(), 3);
}

void AbstractCachedRepositoryTest::insertRecordsTest()
{
  MemoryPersonRepository repository;
  QCOMPARE(repository.rowCount(), 0);
  repository.insertRecords(0, 2, buildPerson("Z"));
  QCOMPARE(repository.rowCount(), 2);
  QCOMPARE(repository.constRecordAt(0).firstName(), QString("fZ"));
  QCOMPARE(repository.constRecordAt(1).firstName(), QString("fZ"));
}

void AbstractCachedRepositoryTest::insertRecordsAndSubmitTest()
{
  MemoryPersonRepository repository;
  QCOMPARE(repository.rowCount(), 0);
  QCOMPARE(repository.storageRowCount(), 0);

  repository.insertRecords(0, 1, buildPerson("A"));
  QCOMPARE(repository.rowCount(), 1);
  QCOMPARE(repository.constRecordAt(0).firstName(), QString("fA"));
  QCOMPARE(repository.operationAtRow(0), Mdt::Container::TableCacheOperation::Insert);
  QCOMPARE(repository.storageRowCount(), 0);

  QVERIFY(repository.submitChanges());
  QCOMPARE(repository.rowCount(), 1);
  QCOMPARE(repository.constRecordAt(0).id().value(), 1);
  QCOMPARE(repository.constRecordAt(0).firstName(), QString("fA"));
  QCOMPARE(repository.operationAtRow(0), Mdt::Container::TableCacheOperation::None);
  QCOMPARE(repository.storageRowCount(), 1);
  QCOMPARE(repository.storage()[0].id().value(), 1);
  QCOMPARE(repository.storage()[0].firstName(), QString("fA"));
}

void AbstractCachedRepositoryTest::insertRecordsAndSubmitSignalTest()
{
  MemoryPersonRepository repository;
  QCOMPARE(repository.rowCount(), 0);
  QSignalSpy opChangedSpy(&repository, &MemoryPersonRepository::operationAtRowsChanged);
  QVERIFY(opChangedSpy.isValid());
  QSignalSpy rowsAboutToBeInsertedSpy(&repository, &MemoryPersonRepository::rowsAboutToBeInserted);
  QVERIFY(rowsAboutToBeInsertedSpy.isValid());
  QSignalSpy rowsInsertedSpy(&repository, &MemoryPersonRepository::rowsInserted);
  QVERIFY(rowsInsertedSpy.isValid());
  QVariantList arguments;

  /*
   * Submit must be callable when nothing is to change
   */
  QCOMPARE(opChangedSpy.count(), 0);
  QVERIFY(repository.submitChanges());
  QCOMPARE(repository.rowCount(), 0);
  QCOMPARE(opChangedSpy.count(), 0);
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);

  repository.insertRecords(0, 2, buildPerson("A"));
  QCOMPARE(repository.rowCount(), 2);
  QCOMPARE(opChangedSpy.count(), 0);
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
  arguments = rowsAboutToBeInsertedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(0)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(1)); // lastRow
  QCOMPARE(rowsInsertedSpy.count(), 1);
  arguments = rowsInsertedSpy.takeFirst();
  QCOMPARE(arguments.count(), 0);

  QVERIFY(repository.submitChanges());
  QCOMPARE(repository.rowCount(), 2);
  QCOMPARE(opChangedSpy.count(), 1);
  arguments = opChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(0)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(1)); // lastRow
  QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
  QCOMPARE(rowsInsertedSpy.count(), 0);
}

void AbstractCachedRepositoryTest::removeRecordsAndSubmitTest()
{
  MemoryPersonRepository repository;

  repository.populate({"A","B","C","D","E"});
  QVERIFY(repository.fetchAll());
  QCOMPARE(repository.rowCount(), 5);
  QCOMPARE(repository.constRecordAt(0).firstName(), QString("fA"));
  QCOMPARE(repository.constRecordAt(1).firstName(), QString("fB"));
  QCOMPARE(repository.constRecordAt(2).firstName(), QString("fC"));
  QCOMPARE(repository.constRecordAt(3).firstName(), QString("fD"));
  QCOMPARE(repository.constRecordAt(4).firstName(), QString("fE"));
  QCOMPARE(repository.storageRowCount(), 5);
  QCOMPARE(repository.storage().at(0).firstName(), QString("fA"));
  QCOMPARE(repository.storage().at(1).firstName(), QString("fB"));
  QCOMPARE(repository.storage().at(2).firstName(), QString("fC"));
  QCOMPARE(repository.storage().at(3).firstName(), QString("fD"));
  QCOMPARE(repository.storage().at(4).firstName(), QString("fE"));

  /*
   * Remove 1 record
   */

  repository.removeRecords(4, 1);
  QCOMPARE(repository.rowCount(), 5);
  QCOMPARE(repository.constRecordAt(0).firstName(), QString("fA"));
  QCOMPARE(repository.constRecordAt(1).firstName(), QString("fB"));
  QCOMPARE(repository.constRecordAt(2).firstName(), QString("fC"));
  QCOMPARE(repository.constRecordAt(3).firstName(), QString("fD"));
  QCOMPARE(repository.constRecordAt(4).firstName(), QString("fE"));
  QCOMPARE(repository.storageRowCount(), 5);
  QCOMPARE(repository.storage().at(0).firstName(), QString("fA"));
  QCOMPARE(repository.storage().at(1).firstName(), QString("fB"));
  QCOMPARE(repository.storage().at(2).firstName(), QString("fC"));
  QCOMPARE(repository.storage().at(3).firstName(), QString("fD"));
  QCOMPARE(repository.storage().at(4).firstName(), QString("fE"));

  QVERIFY(repository.submitChanges());
  QCOMPARE(repository.rowCount(), 4);
  QCOMPARE(repository.constRecordAt(0).firstName(), QString("fA"));
  QCOMPARE(repository.constRecordAt(1).firstName(), QString("fB"));
  QCOMPARE(repository.constRecordAt(2).firstName(), QString("fC"));
  QCOMPARE(repository.constRecordAt(3).firstName(), QString("fD"));
  QCOMPARE(repository.storageRowCount(), 4);
  QCOMPARE(repository.storage().at(0).firstName(), QString("fA"));
  QCOMPARE(repository.storage().at(1).firstName(), QString("fB"));
  QCOMPARE(repository.storage().at(2).firstName(), QString("fC"));
  QCOMPARE(repository.storage().at(3).firstName(), QString("fD"));

  /*
   * Remove 2 non consecutive records
   */

  repository.removeRecords(0, 1);
  repository.removeRecords(2, 1);
  QCOMPARE(repository.rowCount(), 4);
  QCOMPARE(repository.constRecordAt(0).firstName(), QString("fA"));
  QCOMPARE(repository.constRecordAt(1).firstName(), QString("fB"));
  QCOMPARE(repository.constRecordAt(2).firstName(), QString("fC"));
  QCOMPARE(repository.constRecordAt(3).firstName(), QString("fD"));
  QCOMPARE(repository.storageRowCount(), 4);
  QCOMPARE(repository.storage().at(0).firstName(), QString("fA"));
  QCOMPARE(repository.storage().at(1).firstName(), QString("fB"));
  QCOMPARE(repository.storage().at(2).firstName(), QString("fC"));
  QCOMPARE(repository.storage().at(3).firstName(), QString("fD"));

  QVERIFY(repository.submitChanges());
  QCOMPARE(repository.rowCount(), 2);
  QCOMPARE(repository.constRecordAt(0).firstName(), QString("fB"));
  QCOMPARE(repository.constRecordAt(1).firstName(), QString("fD"));
  QCOMPARE(repository.storageRowCount(), 2);
  QCOMPARE(repository.storage().at(0).firstName(), QString("fB"));
  QCOMPARE(repository.storage().at(1).firstName(), QString("fD"));
}

void AbstractCachedRepositoryTest::removeRecordsAndSubmitSignalTest()
{
  MemoryPersonRepository repository;
  QSignalSpy opChangedSpy(&repository, &MemoryPersonRepository::operationAtRowsChanged);
  QSignalSpy rowsAboutToBeRemovedSpy(&repository, &MemoryPersonRepository::rowsAboutToBeRemoved);
  QSignalSpy rowsRemovedSpy(&repository, &MemoryPersonRepository::rowsRemoved);
  QVERIFY(opChangedSpy.isValid());
  QVERIFY(rowsAboutToBeRemovedSpy.isValid());
  QVERIFY(rowsRemovedSpy.isValid());
  QVariantList arguments;

  repository.populate({"A","B"});
  QVERIFY(repository.fetchAll());
  QCOMPARE(opChangedSpy.count(), 0);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);

  repository.removeRecords(1, 1);
  QCOMPARE(opChangedSpy.count(), 1);
  arguments = opChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(1)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(1)); // lastRow
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);

  QVERIFY(repository.submitChanges());
  QCOMPARE(opChangedSpy.count(), 0);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
  arguments = rowsAboutToBeRemovedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(1)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(1)); // lastRow
  QCOMPARE(rowsRemovedSpy.count(), 1);
}

void AbstractCachedRepositoryTest::cancelRemoveRecordsAndSubmitTest()
{
  MemoryPersonRepository repository;

  repository.populate({"A","B","C"});
  QVERIFY(repository.fetchAll());
  QCOMPARE(repository.rowCount(), 3);
  QCOMPARE(repository.constRecordAt(0).firstName(), QString("fA"));
  QCOMPARE(repository.constRecordAt(1).firstName(), QString("fB"));
  QCOMPARE(repository.constRecordAt(2).firstName(), QString("fC"));
  QCOMPARE(repository.storageRowCount(), 3);
  QCOMPARE(repository.storage().at(0).firstName(), QString("fA"));
  QCOMPARE(repository.storage().at(1).firstName(), QString("fB"));
  QCOMPARE(repository.storage().at(2).firstName(), QString("fC"));

  repository.removeRecords(1, 1);
  QCOMPARE(repository.rowCount(), 3);
  QCOMPARE(repository.constRecordAt(0).firstName(), QString("fA"));
  QCOMPARE(repository.constRecordAt(1).firstName(), QString("fB"));
  QCOMPARE(repository.constRecordAt(2).firstName(), QString("fC"));
  QCOMPARE(repository.storageRowCount(), 3);
  QCOMPARE(repository.storage().at(0).firstName(), QString("fA"));
  QCOMPARE(repository.storage().at(1).firstName(), QString("fB"));
  QCOMPARE(repository.storage().at(2).firstName(), QString("fC"));

  repository.cancelRemoveRecords(1, 1);
  QCOMPARE(repository.rowCount(), 3);
  QCOMPARE(repository.constRecordAt(0).firstName(), QString("fA"));
  QCOMPARE(repository.constRecordAt(1).firstName(), QString("fB"));
  QCOMPARE(repository.constRecordAt(2).firstName(), QString("fC"));
  QCOMPARE(repository.storageRowCount(), 3);
  QCOMPARE(repository.storage().at(0).firstName(), QString("fA"));
  QCOMPARE(repository.storage().at(1).firstName(), QString("fB"));
  QCOMPARE(repository.storage().at(2).firstName(), QString("fC"));

  repository.cancelRemoveRecords(0, 1);
  QCOMPARE(repository.rowCount(), 3);
  QCOMPARE(repository.constRecordAt(0).firstName(), QString("fA"));
  QCOMPARE(repository.constRecordAt(1).firstName(), QString("fB"));
  QCOMPARE(repository.constRecordAt(2).firstName(), QString("fC"));
  QCOMPARE(repository.storageRowCount(), 3);
  QCOMPARE(repository.storage().at(0).firstName(), QString("fA"));
  QCOMPARE(repository.storage().at(1).firstName(), QString("fB"));
  QCOMPARE(repository.storage().at(2).firstName(), QString("fC"));

  QVERIFY(repository.submitChanges());
  QCOMPARE(repository.rowCount(), 3);
  QCOMPARE(repository.constRecordAt(0).firstName(), QString("fA"));
  QCOMPARE(repository.constRecordAt(1).firstName(), QString("fB"));
  QCOMPARE(repository.constRecordAt(2).firstName(), QString("fC"));
  QCOMPARE(repository.storageRowCount(), 3);
  QCOMPARE(repository.storage().at(0).firstName(), QString("fA"));
  QCOMPARE(repository.storage().at(1).firstName(), QString("fB"));
  QCOMPARE(repository.storage().at(2).firstName(), QString("fC"));
}

void AbstractCachedRepositoryTest::cancelRemoveRecordsAndSubmitSignalTest()
{
  MemoryPersonRepository repository;
  QSignalSpy opChangedSpy(&repository, &MemoryPersonRepository::operationAtRowsChanged);
  QVERIFY(opChangedSpy.isValid());
  QVariantList arguments;

  repository.populate({"A","B"});
  QVERIFY(repository.fetchAll());
  QCOMPARE(opChangedSpy.count(), 0);

  repository.removeRecords(1, 1);
  QCOMPARE(opChangedSpy.count(), 1);
  arguments = opChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(1)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(1)); // lastRow

  repository.cancelRemoveRecords(1, 1);
  QCOMPARE(opChangedSpy.count(), 1);
  arguments = opChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(1)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(1)); // lastRow
  
  repository.cancelRemoveRecords(0, 1);
  QCOMPARE(opChangedSpy.count(), 0);

  QVERIFY(repository.submitChanges());
  QCOMPARE(opChangedSpy.count(), 0);
}

void AbstractCachedRepositoryTest::insertRecordsRemoveAndSubmitTest()
{
  MemoryPersonRepository repository;
  QCOMPARE(repository.rowCount(), 0);
  QCOMPARE(repository.storageRowCount(), 0);

  repository.populate({"A","B"});
  QVERIFY(repository.fetchAll());
  QCOMPARE(repository.rowCount(), 2);
  QCOMPARE(repository.constRecordAt(0).firstName(), QString("fA"));
  QCOMPARE(repository.constRecordAt(1).firstName(), QString("fB"));
  QCOMPARE(repository.storageRowCount(), 2);
  QCOMPARE(repository.storage().at(0).firstName(), QString("fA"));
  QCOMPARE(repository.storage().at(1).firstName(), QString("fB"));

  repository.insertRecords(2, 3, buildPerson("N"));
  QCOMPARE(repository.rowCount(), 5);
  QCOMPARE(repository.constRecordAt(0).firstName(), QString("fA"));
  QCOMPARE(repository.constRecordAt(1).firstName(), QString("fB"));
  QCOMPARE(repository.constRecordAt(2).firstName(), QString("fN"));
  QCOMPARE(repository.constRecordAt(3).firstName(), QString("fN"));
  QCOMPARE(repository.constRecordAt(4).firstName(), QString("fN"));
  QCOMPARE(repository.operationAtRow(0), Mdt::Container::TableCacheOperation::None);
  QCOMPARE(repository.operationAtRow(1), Mdt::Container::TableCacheOperation::None);
  QCOMPARE(repository.operationAtRow(2), Mdt::Container::TableCacheOperation::Insert);
  QCOMPARE(repository.operationAtRow(3), Mdt::Container::TableCacheOperation::Insert);
  QCOMPARE(repository.operationAtRow(4), Mdt::Container::TableCacheOperation::Insert);
  QCOMPARE(repository.storageRowCount(), 2);
  QCOMPARE(repository.storage().at(0).firstName(), QString("fA"));
  QCOMPARE(repository.storage().at(1).firstName(), QString("fB"));

  repository.removeRecords(3, 2);
  QCOMPARE(repository.rowCount(), 5);
  QCOMPARE(repository.constRecordAt(0).firstName(), QString("fA"));
  QCOMPARE(repository.constRecordAt(1).firstName(), QString("fB"));
  QCOMPARE(repository.constRecordAt(2).firstName(), QString("fN"));
  QCOMPARE(repository.constRecordAt(3).firstName(), QString("fN"));
  QCOMPARE(repository.constRecordAt(4).firstName(), QString("fN"));
  QCOMPARE(repository.operationAtRow(0), Mdt::Container::TableCacheOperation::None);
  QCOMPARE(repository.operationAtRow(1), Mdt::Container::TableCacheOperation::None);
  QCOMPARE(repository.operationAtRow(2), Mdt::Container::TableCacheOperation::Insert);
  QCOMPARE(repository.operationAtRow(3), Mdt::Container::TableCacheOperation::InsertDelete);
  QCOMPARE(repository.operationAtRow(4), Mdt::Container::TableCacheOperation::InsertDelete);
  QCOMPARE(repository.storageRowCount(), 2);
  QCOMPARE(repository.storage().at(0).firstName(), QString("fA"));
  QCOMPARE(repository.storage().at(1).firstName(), QString("fB"));

  QVERIFY(repository.submitChanges());
  QCOMPARE(repository.rowCount(), 3);
  QCOMPARE(repository.constRecordAt(0).firstName(), QString("fA"));
  QCOMPARE(repository.constRecordAt(1).firstName(), QString("fB"));
  QCOMPARE(repository.constRecordAt(2).firstName(), QString("fN"));
  QCOMPARE(repository.operationAtRow(0), Mdt::Container::TableCacheOperation::None);
  QCOMPARE(repository.operationAtRow(1), Mdt::Container::TableCacheOperation::None);
  QCOMPARE(repository.operationAtRow(2), Mdt::Container::TableCacheOperation::None);
  QCOMPARE(repository.storageRowCount(), 3);
  QCOMPARE(repository.storage().at(0).firstName(), QString("fA"));
  QCOMPARE(repository.storage().at(1).firstName(), QString("fB"));
  QCOMPARE(repository.storage().at(2).firstName(), QString("fN"));

  repository.insertRecords(3, 2, buildPerson("N2"));
  QCOMPARE(repository.rowCount(), 5);
  QCOMPARE(repository.constRecordAt(0).firstName(), QString("fA"));
  QCOMPARE(repository.constRecordAt(1).firstName(), QString("fB"));
  QCOMPARE(repository.constRecordAt(2).firstName(), QString("fN"));
  QCOMPARE(repository.constRecordAt(3).firstName(), QString("fN2"));
  QCOMPARE(repository.constRecordAt(4).firstName(), QString("fN2"));
  QCOMPARE(repository.operationAtRow(0), Mdt::Container::TableCacheOperation::None);
  QCOMPARE(repository.operationAtRow(1), Mdt::Container::TableCacheOperation::None);
  QCOMPARE(repository.operationAtRow(2), Mdt::Container::TableCacheOperation::None);
  QCOMPARE(repository.operationAtRow(3), Mdt::Container::TableCacheOperation::Insert);
  QCOMPARE(repository.operationAtRow(4), Mdt::Container::TableCacheOperation::Insert);
  QCOMPARE(repository.storageRowCount(), 3);
  QCOMPARE(repository.storage().at(0).firstName(), QString("fA"));
  QCOMPARE(repository.storage().at(1).firstName(), QString("fB"));
  QCOMPARE(repository.storage().at(2).firstName(), QString("fN"));

  repository.removeRecords(3, 1);
  QCOMPARE(repository.rowCount(), 5);
  QCOMPARE(repository.constRecordAt(0).firstName(), QString("fA"));
  QCOMPARE(repository.constRecordAt(1).firstName(), QString("fB"));
  QCOMPARE(repository.constRecordAt(2).firstName(), QString("fN"));
  QCOMPARE(repository.constRecordAt(3).firstName(), QString("fN2"));
  QCOMPARE(repository.constRecordAt(4).firstName(), QString("fN2"));
  QCOMPARE(repository.operationAtRow(0), Mdt::Container::TableCacheOperation::None);
  QCOMPARE(repository.operationAtRow(1), Mdt::Container::TableCacheOperation::None);
  QCOMPARE(repository.operationAtRow(2), Mdt::Container::TableCacheOperation::None);
  QCOMPARE(repository.operationAtRow(3), Mdt::Container::TableCacheOperation::InsertDelete);
  QCOMPARE(repository.operationAtRow(4), Mdt::Container::TableCacheOperation::Insert);
  QCOMPARE(repository.storageRowCount(), 3);
  QCOMPARE(repository.storage().at(0).firstName(), QString("fA"));
  QCOMPARE(repository.storage().at(1).firstName(), QString("fB"));
  QCOMPARE(repository.storage().at(2).firstName(), QString("fN"));

  QVERIFY(repository.submitChanges());
  QCOMPARE(repository.rowCount(), 4);
  QCOMPARE(repository.constRecordAt(0).firstName(), QString("fA"));
  QCOMPARE(repository.constRecordAt(1).firstName(), QString("fB"));
  QCOMPARE(repository.constRecordAt(2).firstName(), QString("fN"));
  QCOMPARE(repository.constRecordAt(3).firstName(), QString("fN2"));
  QCOMPARE(repository.operationAtRow(0), Mdt::Container::TableCacheOperation::None);
  QCOMPARE(repository.operationAtRow(1), Mdt::Container::TableCacheOperation::None);
  QCOMPARE(repository.operationAtRow(2), Mdt::Container::TableCacheOperation::None);
  QCOMPARE(repository.operationAtRow(3), Mdt::Container::TableCacheOperation::None);
  QCOMPARE(repository.storageRowCount(), 4);
  QCOMPARE(repository.storage().at(0).firstName(), QString("fA"));
  QCOMPARE(repository.storage().at(1).firstName(), QString("fB"));
  QCOMPARE(repository.storage().at(2).firstName(), QString("fN"));
  QCOMPARE(repository.storage().at(3).firstName(), QString("fN2"));
}

void AbstractCachedRepositoryTest::insertRecordsRemoveAndSubmitSignalTest()
{
  MemoryPersonRepository repository;
  QSignalSpy opChangedSpy(&repository, &MemoryPersonRepository::operationAtRowsChanged);
  QSignalSpy rowsAboutToBeRemovedSpy(&repository, &MemoryPersonRepository::rowsAboutToBeRemoved);
  QSignalSpy rowsRemovedSpy(&repository, &MemoryPersonRepository::rowsRemoved);
  QVERIFY(opChangedSpy.isValid());
  QVERIFY(rowsAboutToBeRemovedSpy.isValid());
  QVERIFY(rowsRemovedSpy.isValid());
  QVariantList arguments;

  /*
   * |row|data|operation|
   * --------------------
   * | 0 | fA |  None   |
   * | 1 | fB |  None   |
   */
  repository.populate({"A","B"});
  QVERIFY(repository.fetchAll());
  QCOMPARE(repository.rowCount(), 2);
  QCOMPARE(opChangedSpy.count(), 0);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);

  /*
   * |row|data|operation|
   * --------------------
   * | 0 | fA | None    |
   * | 1 | fB | None    |
   * | 2 | fN | Insert  |
   * | 3 | fN | Insert  |
   * | 4 | fN | Insert  |
   */
  repository.insertRecords(2, 3, buildPerson("N"));
  QCOMPARE(repository.rowCount(), 5);
  QCOMPARE(opChangedSpy.count(), 0);
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);

  /*
   * |row|data|  operation   |
   * -------------------------
   * | 0 | fA | None         |
   * | 1 | fB | None         |
   * | 2 | fN | Insert       |
   * | 3 | fN | InsertDelete |
   * | 4 | fN | InsertDelete |
   */
  repository.removeRecords(3, 2);
  QCOMPARE(repository.rowCount(), 5);
  QCOMPARE(opChangedSpy.count(), 1);
  arguments = opChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(3)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(4)); // lastRow
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);

  /*
   * |row|data|operation|
   * --------------------
   * | 0 | fA | None    |
   * | 1 | fB | None    |
   * | 2 | fN | None    |
   */
  QVERIFY(repository.submitChanges());
  QCOMPARE(repository.rowCount(), 3);
  QCOMPARE(opChangedSpy.count(), 1);
  arguments = opChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(2)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(2)); // lastRow
  // NOTE: Currently, adjacent rows are not grouped (this could be a optimisation)
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 2);
  arguments = rowsAboutToBeRemovedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(4)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(4)); // lastRow
  arguments = rowsAboutToBeRemovedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(3)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(3)); // lastRow
  QCOMPARE(rowsRemovedSpy.count(), 2);
  arguments = rowsRemovedSpy.takeFirst();
  QCOMPARE(arguments.count(), 0);
  arguments = rowsRemovedSpy.takeFirst();
  QCOMPARE(arguments.count(), 0);

  /*
   * |row|data|  operation   |
   * -------------------------
   * | 0 | fA | None         |
   * | 1 | fB | None         |
   * | 2 | fN | None         |
   * | 3 | fO | InsertDelete |
   * | 4 | fO | Insert       |
   */
  repository.insertRecords(3, 2, buildPerson("O"));
  repository.removeRecords(3, 1);
  QCOMPARE(repository.rowCount(), 5);
  QCOMPARE(opChangedSpy.count(), 1);
  arguments = opChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(3)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(3)); // lastRow
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
  QCOMPARE(rowsRemovedSpy.count(), 0);

  /*
   * |row|data|  operation   |
   * -------------------------
   * | 0 | fA | None         |
   * | 1 | fB | None         |
   * | 2 | fN | None         |
   * | 3 | fO | None         |
   */
  QVERIFY(repository.submitChanges());
  QCOMPARE(repository.rowCount(), 4);
  QCOMPARE(opChangedSpy.count(), 1);
  arguments = opChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(3)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(3)); // lastRow
  QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
  arguments = rowsAboutToBeRemovedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(3)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(3)); // lastRow
  QCOMPARE(rowsRemovedSpy.count(), 1);
  arguments = rowsRemovedSpy.takeFirst();
  QCOMPARE(arguments.count(), 0);
}

void AbstractCachedRepositoryTest::dataTest()
{
  MemoryPersonRepository repository;
  repository.populate({"A","B","C"});
  QVERIFY(repository.fetchAll());

  QCOMPARE(repository.rowCount(), 3);
  QCOMPARE(repository.columnCount(), 2);
  QCOMPARE(repository.data(0, 1), QVariant("fA"));
  QCOMPARE(repository.data(1, 1), QVariant("fB"));
  QCOMPARE(repository.data(2, 1), QVariant("fC"));
}

void AbstractCachedRepositoryTest::setDataTest()
{
  MemoryPersonRepository repository;
  repository.populate({"","",""});
  QVERIFY(repository.fetchAll());

  QCOMPARE(repository.rowCount(), 3);
  QCOMPARE(repository.columnCount(), 2);
  QCOMPARE(repository.data(0, 1), QVariant("f"));
  QCOMPARE(repository.data(1, 1), QVariant("f"));
  QCOMPARE(repository.data(2, 1), QVariant("f"));
  repository.setData(0, 0, 1);
  repository.setData(0, 1, "A");
  QCOMPARE(repository.data(0, 0), QVariant(1));
  QCOMPARE(repository.data(0, 1), QVariant("A"));
  QCOMPARE(repository.data(1, 1), QVariant("f"));
  QCOMPARE(repository.data(2, 1), QVariant("f"));
}

void AbstractCachedRepositoryTest::setDataAndSubmitTest()
{
  MemoryPersonRepository repository;
  repository.populate({"","",""});
  QVERIFY(repository.fetchAll());

  QCOMPARE(repository.rowCount(), 3);
  QCOMPARE(repository.columnCount(), 2);
  QCOMPARE(repository.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(repository.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(repository.operationAtRow(2), TableCacheOperation::None);
  repository.setData(0, 1, "A");
  QCOMPARE(repository.data(0, 1), QVariant("A"));
  QCOMPARE(repository.data(1, 1), QVariant("f"));
  QCOMPARE(repository.data(2, 1), QVariant("f"));
  QCOMPARE(repository.operationAtRow(0), TableCacheOperation::Update);
  QCOMPARE(repository.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(repository.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(repository.storage()[0].firstName(), QString("f"));
  QCOMPARE(repository.storage()[1].firstName(), QString("f"));
  QCOMPARE(repository.storage()[2].firstName(), QString("f"));

  QVERIFY(repository.submitChanges());
  QCOMPARE(repository.data(0, 1), QVariant("A"));
  QCOMPARE(repository.data(1, 1), QVariant("f"));
  QCOMPARE(repository.data(2, 1), QVariant("f"));
  QCOMPARE(repository.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(repository.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(repository.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(repository.storage()[0].firstName(), QString("A"));
  QCOMPARE(repository.storage()[1].firstName(), QString("f"));
  QCOMPARE(repository.storage()[2].firstName(), QString("f"));
}

void AbstractCachedRepositoryTest::setDataAndSubmitSignalTest()
{
  MemoryPersonRepository repository;
  repository.populate({"",""});
  QVERIFY(repository.fetchAll());
  QSignalSpy dataAtRowsChangedSpy(&repository, &MemoryPersonRepository::dataAtRowsChanged);
  QVERIFY(dataAtRowsChangedSpy.isValid());
  QSignalSpy operationAtRowsChangedSpy(&repository, &MemoryPersonRepository::operationAtRowsChanged);
  QVERIFY(operationAtRowsChangedSpy.isValid());
  QVariantList arguments;

  QCOMPARE(repository.rowCount(), 2);
  QCOMPARE(dataAtRowsChangedSpy.count(), 0);
  QCOMPARE(operationAtRowsChangedSpy.count(), 0);

  repository.setData(0, 1, "A");
  QCOMPARE(dataAtRowsChangedSpy.count(), 1);
  arguments = dataAtRowsChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(0)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(0)); // lastRow
  QCOMPARE(operationAtRowsChangedSpy.count(), 1);
  arguments = operationAtRowsChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(0)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(0)); // lastRow

  QVERIFY(repository.submitChanges());
  QCOMPARE(dataAtRowsChangedSpy.count(), 0);
  QCOMPARE(operationAtRowsChangedSpy.count(), 1);
  arguments = operationAtRowsChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(0)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(0)); // lastRow
}

void AbstractCachedRepositoryTest::setDataAndRevertTest()
{
  MemoryPersonRepository repository;
  repository.populate({"A","B","C"});
  QVERIFY(repository.fetchAll());

  QCOMPARE(repository.rowCount(), 3);
  QCOMPARE(repository.data(0, 1), QVariant("fA"));
  QCOMPARE(repository.data(1, 1), QVariant("fB"));
  QCOMPARE(repository.data(2, 1), QVariant("fC"));
  QCOMPARE(repository.operationAtRow(0), TableCacheOperation::None);
  QCOMPARE(repository.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(repository.operationAtRow(2), TableCacheOperation::None);
  repository.setData(0, 1, "E-A");
  QCOMPARE(repository.data(0, 1), QVariant("E-A"));
  QCOMPARE(repository.data(1, 1), QVariant("fB"));
  QCOMPARE(repository.data(2, 1), QVariant("fC"));
  QCOMPARE(repository.operationAtRow(0), TableCacheOperation::Update);
  QCOMPARE(repository.operationAtRow(1), TableCacheOperation::None);
  QCOMPARE(repository.operationAtRow(2), TableCacheOperation::None);
  QCOMPARE(repository.storage()[0].firstName(), QString("fA"));
  QCOMPARE(repository.storage()[1].firstName(), QString("fB"));
  QCOMPARE(repository.storage()[2].firstName(), QString("fC"));

  
  QFAIL("Not complete");
}

void AbstractCachedRepositoryTest::setDataAndRevertSignalTest()
{

  QFAIL("Not complete");
}

void AbstractCachedRepositoryTest::insertRecordSetDataAndSubmitTest()
{
  QFAIL("Not complete");
}

void AbstractCachedRepositoryTest::insertRecordSetDataAndSubmitSignalTest()
{
  QFAIL("Not complete");
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  AbstractCachedRepositoryTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}

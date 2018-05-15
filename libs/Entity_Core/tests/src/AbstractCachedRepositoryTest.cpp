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
#include <QStringList>
#include <QSignalSpy>
#include <QVariantList>
#include <vector>
#include <algorithm>

// #include <QDebug>

using namespace Mdt::Entity;

class PersonId : public Mdt::Entity::IntegralUniqueIdTemplate<>
{
 public:

  using IntegralUniqueIdTemplate<>::IntegralUniqueIdTemplate;
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

  virtual PersonData personById(PersonId id) const = 0;

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
      case 1:
        recordAt(row).setFirstName(data.toString());
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
    qDebug() << "Mem: pop..";
    mMem.clear();
    for(const auto & baseName : baseNames){
      mMem.push_back( buildPerson(baseName) );
    }
  }

  PersonData personById(PersonId id) const override
  {
    Q_ASSERT(!id.isNull());
  }

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

  bool insertRecordToStorage(const PersonData& record) override
  {
    mMem.push_back(record);
    return true;
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

void AbstractCachedRepositoryTest::getSetRecordAtTest()
{
  MemoryPersonRepository repository;
  repository.populate({"A","B"});
  QVERIFY(repository.fetchAll());
  QCOMPARE(repository.rowCount(), 2);
  QCOMPARE(repository.constRecordAt(0).firstName(), QString("fA"));
  QCOMPARE(repository.constRecordAt(1).firstName(), QString("fB"));
  repository.recordAt(0).setFirstName("EfA");
  repository.recordAt(1).setFirstName("EfB");
  QCOMPARE(repository.constRecordAt(0).firstName(), QString("EfA"));
  QCOMPARE(repository.constRecordAt(1).firstName(), QString("EfB"));
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
  QCOMPARE(repository.constRecordAt(0).firstName(), QString("fA"));
  QCOMPARE(repository.operationAtRow(0), Mdt::Container::TableCacheOperation::None);
  QCOMPARE(repository.storageRowCount(), 1);
  QCOMPARE(repository.storage()[0].firstName(), QString("fA"));
}

void AbstractCachedRepositoryTest::insertRecordsAndSubmitSignalTest()
{
  MemoryPersonRepository repository;
  QCOMPARE(repository.rowCount(), 0);
  QSignalSpy opChangedSpy(&repository, &MemoryPersonRepository::operationAtRowsChanged);
  QVERIFY(opChangedSpy.isValid());
  QVariantList arguments;

  /*
   * Submit must be callable when nothing is to change
   */
  QCOMPARE(opChangedSpy.count(), 0);
  QVERIFY(repository.submitChanges());
  QCOMPARE(repository.rowCount(), 0);
  QCOMPARE(opChangedSpy.count(), 0);

  QCOMPARE(opChangedSpy.count(), 0);
  repository.insertRecords(0, 1, buildPerson("A"));
  QCOMPARE(repository.rowCount(), 1);
  QCOMPARE(opChangedSpy.count(), 0);

  QVERIFY(repository.submitChanges());
  QCOMPARE(repository.rowCount(), 1);
  QCOMPARE(opChangedSpy.count(), 1);
  arguments = opChangedSpy.takeFirst();
  QCOMPARE(arguments.count(), 2);
  QCOMPARE(arguments.at(0), QVariant(0)); // firstRow
  QCOMPARE(arguments.at(1), QVariant(0)); // lastRow

  QFAIL("Not complete");
}

void AbstractCachedRepositoryTest::removeRecordsTest()
{
  MemoryPersonRepository repository;
  repository.populate({"A","B"});
  QVERIFY(repository.fetchAll());
  QCOMPARE(repository.rowCount(), 2);
  repository.removeRecords(0, 2);
  QCOMPARE(repository.rowCount(), 0);
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
  QCOMPARE(repository.operationAtRow(0), Mdt::Container::TableCacheOperation::None);
  QCOMPARE(repository.operationAtRow(1), Mdt::Container::TableCacheOperation::None);
  QCOMPARE(repository.operationAtRow(2), Mdt::Container::TableCacheOperation::None);
  repository.setData(0, 1, "A");
  QCOMPARE(repository.data(0, 1), QVariant("A"));
  QCOMPARE(repository.data(1, 1), QVariant("f"));
  QCOMPARE(repository.data(2, 1), QVariant("f"));
  QCOMPARE(repository.operationAtRow(0), Mdt::Container::TableCacheOperation::Update);
  QCOMPARE(repository.operationAtRow(1), Mdt::Container::TableCacheOperation::None);
  QCOMPARE(repository.operationAtRow(2), Mdt::Container::TableCacheOperation::None);
  QCOMPARE(repository.storage()[0].firstName(), QString("f"));
  QCOMPARE(repository.storage()[1].firstName(), QString("f"));
  QCOMPARE(repository.storage()[2].firstName(), QString("f"));

  QVERIFY(repository.submitChanges());
  QCOMPARE(repository.data(0, 1), QVariant("A"));
  QCOMPARE(repository.data(1, 1), QVariant("f"));
  QCOMPARE(repository.data(2, 1), QVariant("f"));
  QCOMPARE(repository.operationAtRow(0), Mdt::Container::TableCacheOperation::None);
  QCOMPARE(repository.operationAtRow(1), Mdt::Container::TableCacheOperation::None);
  QCOMPARE(repository.operationAtRow(2), Mdt::Container::TableCacheOperation::None);
  QCOMPARE(repository.storage()[0].firstName(), QString("A"));
  QCOMPARE(repository.storage()[1].firstName(), QString("f"));
  QCOMPARE(repository.storage()[2].firstName(), QString("f"));
}

void AbstractCachedRepositoryTest::setDataAndSubmitSignalTest()
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

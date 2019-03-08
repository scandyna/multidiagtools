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
#include "AbstractCachedEntityRepositoryTest.h"
#include "Mdt/Entity/AbstractCachedEntityRepository.h"
#include "Mdt/Entity/Def.h"
#include "Mdt/Entity/DataTemplate.h"
#include "Mdt/Entity/IntegralUniqueIdTemplate.h"
#include "Mdt/Container/StlContainer.h"
// #include "Mdt/Entity/FieldAt.h"
#include <QStringList>
#include <QSignalSpy>
#include <QVariantList>
#include <vector>
#include <algorithm>

// #include <QDebug>

using namespace Mdt::Entity;

/*
 * Entities and data
 */

struct PersonDataStruct
{
  qulonglong id;
  QString firstName;
};

MDT_ENTITY_DEF(
  (PersonDataStruct),
  Person,
  (id, FieldFlag::IsPrimaryKey),
  (firstName, FieldMaxLength(5))
)

class PersonId : public Mdt::Entity::IntegralUniqueIdTemplate<PersonId, int>
{
 public:

  using IntegralUniqueIdTemplate::IntegralUniqueIdTemplate;
};

class PersonData : public Mdt::Entity::DataTemplate<PersonEntity>
{
 public:

  void setId(PersonId id)
  {
    dataStruct().id = id.value();
  }

  PersonId id() const
  {
    return PersonId( constDataStruct().id );
  }

  void setFirstName(const QString & name)
  {
    dataStruct().firstName = name.trimmed();
    Q_ASSERT(!firstName().isEmpty());
  }

  QString firstName() const
  {
    return constDataStruct().firstName;
  }
};

/*
 * Repository
 */

class AbstractPersonRepository : public Mdt::Entity::AbstractCachedEntityRepository<PersonData>
{
 public:

  virtual void populate(const QStringList & baseNames) = 0;

  virtual PersonData personById(PersonId id) const = 0;

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

  void populate(const QStringList & baseNames) override
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

  bool insertRecordToStorage(const PersonData& record, QVariant & autoId) override
  {
    auto recordCpy = record;
    auto id = getNextId();
    recordCpy.setId(PersonId(id));
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

void AbstractCachedEntityRepositoryTest::columnCountTest()
{
  MemoryPersonRepository repository;
  QCOMPARE(repository.columnCount(), 2);
}

void AbstractCachedEntityRepositoryTest::dataTest()
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

void AbstractCachedEntityRepositoryTest::setDataTest()
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

void AbstractCachedEntityRepositoryTest::insertRecordsAndSubmitTest()
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

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  AbstractCachedEntityRepositoryTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}

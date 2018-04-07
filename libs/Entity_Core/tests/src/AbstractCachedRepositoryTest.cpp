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
#include "Mdt/Entity/Def.h"
#include "Mdt/Entity/DataTemplate.h"
#include "Mdt/Entity/IntegralUniqueIdTemplate.h"
#include <QStringList>
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

class PersonId : public Mdt::Entity::IntegralUniqueIdTemplate<>
{
 public:

  using IntegralUniqueIdTemplate<>::IntegralUniqueIdTemplate;
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

class AbstractPersonRepository : public Mdt::Entity::AbstractCachedRepository<PersonData>
{
 public:

  virtual void populate(const QStringList & baseNames) = 0;

  virtual PersonData personById(PersonId id) const = 0;
};

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

 private:

  bool fetchRecords(int count) override
  {
    const int n = std::min(count, storageRowCount());
    for(int i = 0; i < n; ++i){
      appendRecordToCache(mMem[i]);
    }
    return true;
  }

  PersonData buildPerson(const QString & baseName) const
  {
    PersonData data;
    data.setFirstName("f" + baseName);
    return data;
  }

  int storageRowCount() const
  {
    return mMem.size();
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

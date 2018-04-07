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
#include "RepositoryHandleTest.h"
#include "Mdt/Entity/Def.h"
#include "Mdt/Entity/DataTemplate.h"
#include "Mdt/Entity/AbstractCachedRepository.h"
#include "Mdt/Entity/RepositoryHandle.h"
#include <QStringList>

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

class PersonData : public Mdt::Entity::DataTemplate<PersonEntity>
{
 public:

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

};

using PersonRepository = Mdt::Entity::RepositoryHandle<AbstractPersonRepository>;

class MemoryPersonRepository : public AbstractPersonRepository
{
 public:

  void populate(const QStringList & baseNames) override
  {
    mMem.clear();
    for(const auto & baseName : baseNames){
      mMem.push_back( buildPerson(baseName) );
    }
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
 * Business objects
 */

class PersonBo
{
 public:

  void setRepository(const PersonRepository & repository)
  {
    mRepository = repository;
  }

  int availablePersonCount() const
  {
    if(mRepository.isNull()){
      return 0;
    }
    return mRepository.constRepository().rowCount();
  }

 private:

  PersonRepository mRepository;
};

/*
 * Tests
 */

void RepositoryHandleTest::createAndUseTest()
{
  RepositoryHandle<AbstractPersonRepository> personRepository;
  QVERIFY(personRepository.isNull());

  personRepository = PersonRepository::make<MemoryPersonRepository>();
  QVERIFY(!personRepository.isNull());
  personRepository.repository().populate({"A","B"});
  QVERIFY(personRepository.repository().fetchAll());
  QCOMPARE(personRepository.constRepository().rowCount(), 2);
  QCOMPARE(personRepository.constRepository().constRecordAt(0).firstName(), QString("fA"));
  QCOMPARE(personRepository.constRepository().constRecordAt(1).firstName(), QString("fB"));
}

void RepositoryHandleTest::useInObjectsTest()
{
  auto personRepository = PersonRepository::make<MemoryPersonRepository>();
  personRepository.repository().populate({"A","B"});
  QVERIFY(personRepository.repository().fetchAll());

  PersonBo pbo1;
  QCOMPARE(pbo1.availablePersonCount(), 0);
  pbo1.setRepository(personRepository);
  QCOMPARE(pbo1.availablePersonCount(), 2);

  PersonBo pbo2;
  QCOMPARE(pbo2.availablePersonCount(), 0);
  pbo2.setRepository(personRepository);
  QCOMPARE(pbo2.availablePersonCount(), 2);
  QCOMPARE(pbo1.availablePersonCount(), 2);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  RepositoryHandleTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}

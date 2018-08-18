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
#include "MemoryEntityRepositoryTest.h"
#include "Mdt/Entity/MemoryEntityRepository.h"
#include "Mdt/Entity/Def.h"
#include "Mdt/Entity/IntegralUniqueIdTemplate.h"
#include "Mdt/Entity/DataTemplate.h"

/*
 * Entities
 */

struct PersonDataStruct
{
  int id = 0;
  QString firstName;
};

MDT_ENTITY_DEF(
  (PersonDataStruct),
  Person,
  (id, FieldFlag::IsPrimaryKey),
  (firstName, FieldMaxLength(250))
)

class PersonId : public Mdt::Entity::IntegralUniqueIdTemplate<int>
{
 public:

  using IntegralUniqueIdTemplate::IntegralUniqueIdTemplate;
};

class PersonData : public Mdt::Entity::DataTemplate<PersonEntity>
{
 public:

  void setPersonId(PersonId id)
  {
    Q_ASSERT(!id.isNull());
    dataStruct().id = id.value();
  }

  PersonId id() const
  {
    return PersonId(constDataStruct().id);
  }

  void setFirstName(const QString & name)
  {
    dataStruct().firstName = name;
  }

  QString firstName() const
  {
    return constDataStruct().firstName;
  }
};

/*
 * Repositories
 */

class GenericTestPersonRepository : public Mdt::Entity::MemoryEntityRepository<PersonData, PersonId>
{
};

class PersonRepository : public virtual Mdt::Entity::AbstractEntityRepository<PersonData, PersonId>
{
};

class TestPersonRepository : public PersonRepository, public Mdt::Entity::MemoryEntityRepository<PersonData, PersonId>
{
};

using namespace Mdt::Entity;

/*
 * Helpers
 */

bool addPersonToRepository(const PersonData & person, const std::shared_ptr<PersonRepository> & repository)
{
  Q_ASSERT(!person.id().isNull());
  Q_ASSERT(repository.get() != nullptr);

  const auto id = repository->add(person);
  if(!id){
    return false;
  }
  Q_ASSERT(*id == person.id());

  return true;
}

/*
 * Tests
 */

void MemoryEntityRepositoryTest::genericUsageTest()
{
  auto repository = std::make_shared<GenericTestPersonRepository>();
  QCOMPARE(repository->storageCount(), 0);

  PersonId personId(1);
  PersonData personData;
  auto personDataExp = repository->getById(personId);
  QVERIFY(personDataExp);
  personData = *personDataExp;
  QVERIFY(personData.id().isNull());
  QVERIFY(personData.firstName().isEmpty());

  personData.setPersonId(personId);
  personData.setFirstName("P1");
  auto personIdExp = repository->add(personData);
  QVERIFY(personIdExp);
  personId = *personIdExp;
  QCOMPARE(personId.value(), 1);
  QCOMPARE(repository->storageCount(), 1);

  personDataExp = repository->getById(personId);
  QVERIFY(personDataExp);
  personData = *personDataExp;
  QCOMPARE(personData.id().value(), 1);
  QCOMPARE(personData.firstName(), QString("P1"));
}

void MemoryEntityRepositoryTest::entityImplClassTest()
{
  auto testRepository = std::make_shared<TestPersonRepository>();
  // We have to check check polymorphic calls
  std::shared_ptr<PersonRepository> repository = testRepository;
  QCOMPARE(testRepository->storageCount(), 0);

  PersonId personId(1);
  PersonData personData;
  auto personDataExp = repository->getById(personId);
  QVERIFY(personDataExp);
  personData = *personDataExp;
  QVERIFY(personData.id().isNull());
  QVERIFY(personData.firstName().isEmpty());

  personData.setPersonId(personId);
  personData.setFirstName("P1");
  auto personIdExp = repository->add(personData);
  QVERIFY(personIdExp);
  personId = *personIdExp;
  QCOMPARE(personId.value(), 1);
  QCOMPARE(testRepository->storageCount(), 1);

  personDataExp = repository->getById(personId);
  QVERIFY(personDataExp);
  personData = *personDataExp;
  QCOMPARE(personData.id().value(), 1);
  QCOMPARE(personData.firstName(), QString("P1"));
}

void MemoryEntityRepositoryTest::addWithNullIdTest()
{
  auto repository = std::make_shared<TestPersonRepository>();
  PersonId personId;
  PersonData person;

  PersonData person1;
  person1.setFirstName("P1");
  QVERIFY(person1.id().isNull());
  auto personIdExp = repository->add(person1);
  QVERIFY(personIdExp);
  personId = *personIdExp;
  QCOMPARE(personId.value(), 1);
  auto personDataExp = repository->getById(personId);
  QVERIFY(personDataExp);
  person = *personDataExp;
  QCOMPARE(person.id().value(), 1);
  QCOMPARE(person.firstName(), QString("P1"));

  PersonData person2;
  person2.setFirstName("P2");
  QVERIFY(person2.id().isNull());
  personIdExp = repository->add(person2);
  QVERIFY(personIdExp);
  personId = *personIdExp;
  QCOMPARE(personId.value(), 2);
  personDataExp = repository->getById(personId);
  QVERIFY(personDataExp);
  person = *personDataExp;
  QCOMPARE(person.id().value(), 2);
  QCOMPARE(person.firstName(), QString("P2"));
}

void MemoryEntityRepositoryTest::addWithIdTest()
{
  auto repository = std::make_shared<TestPersonRepository>();
  PersonId personId;
  PersonData person;

  person.setPersonId(PersonId(10));
  person.setFirstName("P10");
  auto personIdExp = repository->add(person);
  QVERIFY(personIdExp);
  personId = *personIdExp;
  QCOMPARE(personId.value(), 10);
  auto personDataExp = repository->getById(personId);
  QVERIFY(personDataExp);
  person = *personDataExp;
  QCOMPARE(person.id().value(), 10);
  QCOMPARE(person.firstName(), QString("P10"));

  person.setPersonId(PersonId(15));
  person.setFirstName("P15");
  personIdExp = repository->add(person);
  QVERIFY(personIdExp);
  personId = *personIdExp;
  QCOMPARE(personId.value(), 15);
  personDataExp = repository->getById(personId);
  QVERIFY(personDataExp);
  person = *personDataExp;
  QCOMPARE(person.id().value(), 15);
  QCOMPARE(person.firstName(), QString("P15"));
}

void MemoryEntityRepositoryTest::addIdUniqueConstraintTest()
{
  auto repository = std::make_shared<TestPersonRepository>();
  PersonId personId;
  PersonData person;

  person.setPersonId(PersonId(5));
  person.setFirstName("P5");
  auto personIdExp = repository->add(person);
  QVERIFY(personIdExp);
  personId = *personIdExp;
  QCOMPARE(personId.value(), 5);
  QVERIFY(repository->lastError().isNull());
  personIdExp = repository->add(person);
  QVERIFY(!personIdExp);
  QVERIFY(!repository->lastError().isNull());
}

void MemoryEntityRepositoryTest::updateTest()
{
  auto repository = std::make_shared<TestPersonRepository>();
  PersonData person;

  person.setPersonId(PersonId(2));
  person.setFirstName("A");
  QVERIFY(addPersonToRepository(person, repository));

  person.setFirstName("EA");
  QVERIFY(repository->update(person));
  auto personExp = repository->getById(person.id());
  QVERIFY(personExp);
  person = *personExp;
  QCOMPARE(person.id().value(), 2);
  QCOMPARE(person.firstName(), QString("EA"));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  MemoryEntityRepositoryTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}

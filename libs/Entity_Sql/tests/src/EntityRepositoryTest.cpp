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
#include "EntityRepositoryTest.h"
#include "Mdt/Entity/SqlEntityRepository.h"
#include "Mdt/Entity/Def.h"
#include "Mdt/Entity/IntegralUniqueIdTemplate.h"
#include "Mdt/Entity/DataTemplate.h"
#include "Mdt/Entity/SqlTable.h"
#include "Mdt/Sql/Schema/Driver.h"
#include "Mdt/Sql/DeleteQuery.h"
#include <QSqlQuery>
#include <QString>
#include <memory>

using namespace Mdt::Entity;

void EntityRepositoryTest::initTestCase()
{
  QVERIFY(initDatabaseSqlite());
  QVERIFY(createDatabaseSchema());
}

void EntityRepositoryTest::cleanupTestCase()
{
}

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

class PersonId : public IntegralUniqueIdTemplate<PersonId, int>
{
 public:

  using IntegralUniqueIdTemplate::IntegralUniqueIdTemplate;
};

class PersonData : public DataTemplate<PersonEntity>
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

class PersonRepository : public virtual AbstractEntityRepository<PersonData, PersonId>
{
 public:

  
};

class SqlPersonRepository : public PersonRepository, public SqlEntityRepository<PersonData, PersonId>
{
 public:

  
};

/*
 * Helpers (plain)
 */

/*
 * Helpers
 */

bool addPersonToRepository(const PersonData & person, PersonRepository & repository)
{
  Q_ASSERT(!person.id().isNull());

  const auto id = repository.add(person);
  if(!id){
    return false;
  }
  Q_ASSERT(*id == person.id());

  return true;
}

QString getPersonNameFromRepository(PersonId id, const PersonRepository & repository)
{
  Q_ASSERT(!id.isNull());

  const auto person = repository.getById(id);
  Q_ASSERT(person);

  return person->firstName();
}

/*
 * Tests
 */

void EntityRepositoryTest::genericUsageTest()
{
  using PersonRepositoryAlias = AbstractEntityRepository<PersonData, PersonId>;

  QVERIFY(cleanupPersonTable());

  auto sqlRepository = std::make_shared< SqlEntityRepository<PersonData, PersonId> >();
  sqlRepository->setDatabase(database());
  // We have to check check polymorphic calls
  std::shared_ptr<PersonRepositoryAlias> repository = sqlRepository;

  PersonData personData;
  PersonId personId(1);
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

  personDataExp = repository->getById(personId);
  QVERIFY(personDataExp);
  personData = *personDataExp;
  QCOMPARE(personData.id().value(), 1);
  QCOMPARE(personData.firstName(), QString("P1"));
}

void EntityRepositoryTest::entityImplClassTest()
{
  QVERIFY(cleanupPersonTable());

  auto sqlRepository = std::make_shared<SqlPersonRepository>();
  sqlRepository->setDatabase(database());
  // We have to check check polymorphic calls
  std::shared_ptr<PersonRepository> repository = sqlRepository;

  PersonData personData;
  PersonId personId(1);
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

  personDataExp = repository->getById(personId);
  QVERIFY(personDataExp);
  personData = *personDataExp;
  QCOMPARE(personData.id().value(), 1);
  QCOMPARE(personData.firstName(), QString("P1"));
}

void EntityRepositoryTest::addWithNullIdTest()
{
  QVERIFY(cleanupPersonTable());

  SqlPersonRepository repository;
  repository.setDatabase(database());

  PersonData personData;
  personData.setFirstName("A");
  auto personIdExp = repository.add(personData);
  QVERIFY(personIdExp);
  auto personId = *personIdExp;
  QVERIFY(!personId.isNull());
  QVERIFY(personData.id().isNull());
  auto personDataExp = repository.getById(personId);
  QVERIFY(personDataExp);
  personData = *personDataExp;
  QCOMPARE(personData.id().value(), personId.value());
  QCOMPARE(personData.firstName(), QString("A"));

  PersonData personB;
  personB.setFirstName("B");
  personIdExp = repository.add(personB);
  QVERIFY(personIdExp);
  personId = *personIdExp;
  QVERIFY(!personId.isNull());
}

void EntityRepositoryTest::updateTest()
{
  QVERIFY(cleanupPersonTable());

  SqlPersonRepository repository;
  repository.setDatabase(database());
  PersonData person;

  person.setPersonId(PersonId(2));
  person.setFirstName("A");
  QVERIFY(addPersonToRepository(person, repository));

  person.setFirstName("EA");
  QVERIFY(repository.update(person));
  auto personExp = repository.getById(person.id());
  QVERIFY(personExp);
  person = *personExp;
  QCOMPARE(person.id().value(), 2);
  QCOMPARE(person.firstName(), QString("EA"));
}

void EntityRepositoryTest::removeTest()
{
  QVERIFY(cleanupPersonTable());

  SqlPersonRepository repository;
  repository.setDatabase(database());
  PersonData person;

  person.setPersonId(PersonId(1));
  person.setFirstName("A");
  QVERIFY(addPersonToRepository(person, repository));
  person.setPersonId(PersonId(2));
  person.setFirstName("B");
  QVERIFY(addPersonToRepository(person, repository));
  QCOMPARE(getPersonNameFromRepository(PersonId(1), repository), QString("A"));
  QCOMPARE(getPersonNameFromRepository(PersonId(2), repository), QString("B"));

  QVERIFY(repository.remove(PersonId(1)));
  QVERIFY(getPersonNameFromRepository(PersonId(1), repository).isEmpty());
  QCOMPARE(getPersonNameFromRepository(PersonId(2), repository), QString("B"));

  QVERIFY(repository.remove(PersonId(5)));
  QVERIFY(getPersonNameFromRepository(PersonId(1), repository).isEmpty());
  QCOMPARE(getPersonNameFromRepository(PersonId(2), repository), QString("B"));
}

void EntityRepositoryTest::removeAllTest()
{
  QVERIFY(cleanupPersonTable());

  SqlPersonRepository repository;
  repository.setDatabase(database());
  PersonData person;

  person.setFirstName("P1");
  QVERIFY(repository.add(person));
  person.setFirstName("P2");
  QVERIFY(repository.add(person));
  QCOMPARE(getStorageCount(), 2);

  QVERIFY(repository.removeAll());
  QCOMPARE(getStorageCount(), 0);
}

/*
 * Helpers
 */

bool EntityRepositoryTest::createDatabaseSchema()
{
  Mdt::Sql::Schema::Driver driver(database());
  if(!driver.isValid()){
    qWarning() << "DB backend probably not supported";
    return false;
  }
  if( !driver.createTable( Mdt::Entity::SqlTable::fromEntity<PersonEntity>() ) ){
    qWarning() << "Create Person table failed";
    return false;
  }
  return true;
}

bool EntityRepositoryTest::cleanupPersonTable()
{
  Mdt::Sql::DeleteQuery query(database());
  query.setTableName(PersonEntity::def().entityName());
  if(!query.exec()){
    return false;
  }

  return true;
}

int EntityRepositoryTest::getStorageCount()
{
  QSqlQuery query(database());
  const QString sql = "SELECT COUNT(*) FROM " + PersonEntity::def().entityName();

  if(!query.exec(sql)){
    return -1;
  }
  query.next();

  return query.value(0).toInt();
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  EntityRepositoryTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}

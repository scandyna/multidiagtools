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
#include "SelectQueryTest.h"
#include "Mdt/QueryExpression/EntityAndField.h"
#include "Mdt/QueryExpression/SqlSelectQuery.h"
#include "Mdt/QueryExpression/SqlSelectQueryFactory.h"
#include "Mdt/Entity/QueryEntity.h"
#include "Mdt/Entity/EntitySelectStatement.h"
#include "Mdt/Container/VariantRecord.h"
#include "Mdt/Expected.h"
#include <QSignalSpy>
#include <QVariantList>
#include <memory>
#include <vector>

using namespace Mdt::Entity;
using Mdt::QueryExpression::SqlSelectQuery;
using Mdt::QueryExpression::SqlSelectQueryFactory;
using Mdt::Container::VariantRecord;

/*
 * Init / cleanup
 */

void SelectQueryTest::initTestCase()
{
  QVERIFY(initDatabaseSqlite());
  QVERIFY(createTestSchema());
}

void SelectQueryTest::cleanupTestCase()
{
}

/*
 * Use case
 */

class SomeUseCase
{
  using SelectQueryFacotory = Mdt::QueryExpression::AbstractSelectQueryFactory;

 public:

  using PersonList = std::vector<PersonDataStruct>;

  SomeUseCase(const std::shared_ptr<SelectQueryFacotory> & selectQueryFacotory)
   : mSelectQueryFactory(selectQueryFacotory)
  {
  }

  Mdt::Expected<PersonList> getPersonAbove29()
  {
    Mdt::Expected<PersonList> personListExp;
    PersonList personList;

    EntitySelectStatement<PersonEntity> stm;

    const auto id = stm.makeSelectField( stm.def().id() );
    const auto firstName = stm.makeSelectField( stm.def().firstName() );
    const auto age = stm.makeSelectField( stm.def().age() );

    stm.addField(id);
    stm.addField(firstName);
    stm.addField(age);
    stm.setFilter( age > 29 );

    auto query = mSelectQueryFactory->createSelectQuery();
    if( !query->exec(stm) ){
      personListExp = query->lastError();
      return personListExp;
    }
    const auto idIndex = query->fieldIndex(id);
    const auto fnIndex = query->fieldIndex(firstName);
    const auto ageIndex = query->fieldIndex(age);
    while(query->next()){
      PersonDataStruct person;
      person.id = query->value(idIndex).toULongLong();
      person.firstName = query->value(fnIndex).toString();
      person.age = query->value(ageIndex).toInt();
      personList.push_back(person);
    }
    personListExp = personList;

    return personListExp;
  }

 private:

  std::shared_ptr<SelectQueryFacotory> mSelectQueryFactory;
};

/*
 * Tests
 */

void SelectQueryTest::execQueryTest()
{
  QVERIFY(cleanupPersonTable());
  QVERIFY(insertPerson(1, "P1", 10, "R1"));
  QVERIFY(insertPerson(2, "P2", 20, "R2"));

  EntitySelectStatement<PersonEntity> stm;
  stm.selectAllFields();

  SqlSelectQuery query;
  query.setDatabase(database());

  QCOMPARE(query.fieldCount(), 0);
  QVERIFY(query.exec(stm));
  QCOMPARE(query.fieldCount(), 4);

  QVERIFY(query.next());
  QCOMPARE(query.fieldCount(), 4);
  QCOMPARE(query.value(0), QVariant(1));
  QCOMPARE(query.value(1), QVariant("P1"));
  QCOMPARE(query.value(2), QVariant(10));
  QCOMPARE(query.value(3), QVariant("R1"));
  QVERIFY(query.next());
  QCOMPARE(query.value(0), QVariant(2));
  QCOMPARE(query.value(1), QVariant("P2"));
  QCOMPARE(query.value(2), QVariant(20));
  QCOMPARE(query.value(3), QVariant("R2"));
  QVERIFY(!query.next());
}

void SelectQueryTest::execQueryMaxRowsTest()
{
  QVERIFY(cleanupPersonTable());
  QVERIFY(insertPerson(1, "P1", 10, "R1"));
  QVERIFY(insertPerson(2, "P2", 20, "R2"));

  EntitySelectStatement<PersonEntity> stm;
  stm.selectAllFields();

  SqlSelectQuery query;
  query.setDatabase(database());

  QVERIFY(query.exec(stm, 0));
  QVERIFY(query.next());
  QCOMPARE(query.fieldCount(), 4);
  QCOMPARE(query.value(0), QVariant(1));
  QCOMPARE(query.value(1), QVariant("P1"));
  QCOMPARE(query.value(2), QVariant(10));
  QCOMPARE(query.value(3), QVariant("R1"));
  QVERIFY(query.next());
  QCOMPARE(query.value(0), QVariant(2));
  QCOMPARE(query.value(1), QVariant("P2"));
  QCOMPARE(query.value(2), QVariant(20));
  QCOMPARE(query.value(3), QVariant("R2"));
  QVERIFY(!query.next());

  QVERIFY(query.exec(stm, 1));
  QVERIFY(query.next());
  QCOMPARE(query.fieldCount(), 4);
  QCOMPARE(query.value(0), QVariant(1));
  QCOMPARE(query.value(1), QVariant("P1"));
  QCOMPARE(query.value(2), QVariant(10));
  QCOMPARE(query.value(3), QVariant("R1"));
  QVERIFY(!query.next());
}

void SelectQueryTest::execAndFetchAllTest()
{
  QVERIFY(cleanupPersonTable());
  QVERIFY(insertPerson(1, "P1", 10, "R1"));
  QVERIFY(insertPerson(2, "P2", 20, "R2"));

  EntitySelectStatement<PersonEntity> stm;
  stm.selectAllFields();

  SqlSelectQuery query;
  query.setDatabase(database());

  QVariantList arguments;
  VariantRecord record;
  QSignalSpy newRecordAvailableSpy(&query, &SqlSelectQuery::newRecordAvailable);
  QVERIFY(newRecordAvailableSpy.isValid());

  QCOMPARE(newRecordAvailableSpy.count(), 0);
  QVERIFY(query.execAndFetchAll(stm));
  QCOMPARE(newRecordAvailableSpy.count(), 2);
  arguments = newRecordAvailableSpy.takeFirst();
  QCOMPARE(arguments.count(), 1);
  record = arguments.at(0).value<VariantRecord>();
  QCOMPARE(record.columnCount(), 4);
  QCOMPARE(record.value(0), QVariant(1));
  QCOMPARE(record.value(1), QVariant("P1"));
  QCOMPARE(record.value(2), QVariant(10));
  QCOMPARE(record.value(3), QVariant("R1"));
  arguments = newRecordAvailableSpy.takeFirst();
  QCOMPARE(arguments.count(), 1);
  record = arguments.at(0).value<VariantRecord>();
  QCOMPARE(record.columnCount(), 4);
  QCOMPARE(record.value(0), QVariant(2));
  QCOMPARE(record.value(1), QVariant("P2"));
  QCOMPARE(record.value(2), QVariant(20));
  QCOMPARE(record.value(3), QVariant("R2"));

  QVERIFY(!query.next());
  QCOMPARE(newRecordAvailableSpy.count(), 0);
}

void SelectQueryTest::fieldIndexTest()
{
  using Mdt::QueryExpression::SelectField;
  using Mdt::QueryExpression::FieldName;
  using Mdt::QueryExpression::EntityName;
  using Mdt::QueryExpression::SelectEntity;
  using Mdt::QueryExpression::SelectStatement;

  QVERIFY(cleanupPersonTable());
  QVERIFY(insertPerson(1, "P1", 10, "R1"));
  QVERIFY(insertPerson(2, "P2", 20, "R2"));

  SelectEntity person(EntityName("Person"));

  SelectField firstName( FieldName("firstName") );
  SelectField age( FieldName("age"), "A" );
  SelectField remarks( person, FieldName("remarks") );

  SelectStatement stm;
  stm.setEntity(person);
  stm.addField(remarks);
  stm.addField(firstName);
  stm.addField(age);

  SqlSelectQuery query;
  query.setDatabase(database());

  QVERIFY(query.exec(stm));
  QCOMPARE(query.fieldCount(), 3);
  QCOMPARE(query.fieldIndex(remarks), 0);
  QCOMPARE(query.fieldIndex(firstName), 1);
  QCOMPARE(query.fieldIndex(age), 2);
}

void SelectQueryTest::fieldIndexEntityTest()
{
  QVERIFY(cleanupPersonTable());
  QVERIFY(insertPerson(1, "P1", 10, "R1"));
  QVERIFY(insertPerson(2, "P2", 20, "R2"));

  EntitySelectStatement<PersonEntity> stm;
  const auto firstName = stm.makeSelectField( stm.def().firstName() );
  const auto age = stm.makeSelectField( stm.def().age(), "PA" );
  const auto remarks = stm.makeSelectField( stm.def().remarks() );
  stm.addField(age);
  stm.addField(remarks);
  stm.addField(firstName);

  SqlSelectQuery query;
  query.setDatabase(database());

  QVERIFY(query.exec(stm));
  QCOMPARE(query.fieldCount(), 3);
  QCOMPARE(query.fieldIndex(age), 0);
  QCOMPARE(query.fieldIndex(remarks), 1);
  QCOMPARE(query.fieldIndex(firstName), 2);
}

void SelectQueryTest::fieldIndexMultiEntityTest()
{

  QFAIL("Not complete");
}

void SelectQueryTest::execQueryFilterTest()
{
  QVERIFY(cleanupPersonTable());
  QVERIFY(insertPerson(1, "P1", 10, "R1"));
  QVERIFY(insertPerson(2, "P2", 20, "R2"));
  QVERIFY(insertPerson(3, "P3", 30, "R3"));

  EntitySelectStatement<PersonEntity> stm;
  const auto age = stm.makeSelectField( stm.def().age() );
  stm.selectAllFields();
  stm.setFilter( (age > 10)&&(age < 30) );

  SqlSelectQuery query;
  query.setDatabase(database());

  QVERIFY(query.exec(stm));
  QCOMPARE(query.fieldCount(), 4);
  const auto firstNameIndex = query.fieldIndex( stm.makeSelectField(stm.def().firstName()) );
  const auto ageIndex = query.fieldIndex(age);
  QVERIFY(query.next());
  QCOMPARE(query.value(firstNameIndex), QVariant("P2"));
  QCOMPARE(query.value(ageIndex), QVariant(20));
  QVERIFY(!query.next());
}

void SelectQueryTest::useCaseFactoryTest()
{
  QVERIFY(cleanupPersonTable());
  QVERIFY(insertPerson(1, "P1", 10, "R1"));
  QVERIFY(insertPerson(2, "P2", 20, "R2"));
  QVERIFY(insertPerson(3, "P3", 30, "R3"));

  auto selectQueryFactory = std::make_shared<SqlSelectQueryFactory>();
  selectQueryFactory->setDatabase(database());

  SomeUseCase someUseCase(selectQueryFactory);
  const auto personListExp = someUseCase.getPersonAbove29();
  QVERIFY(personListExp);
  const auto personList = *personListExp;
  QCOMPARE(personList.size(), 1ul);
  QCOMPARE(personList[0].id, 3ull);
  QCOMPARE(personList[0].firstName, QString("P3"));
  QCOMPARE(personList[0].age, 30);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  SelectQueryTest test;

  return QTest::qExec(&test, argc, argv);
}

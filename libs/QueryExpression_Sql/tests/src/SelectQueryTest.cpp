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
#include "Mdt/Sql/InsertQuery.h"
#include "Mdt/Sql/Schema/Driver.h"
#include "Mdt/Entity/Def.h"
#include "Mdt/Entity/QueryEntity.h"
#include "Mdt/Entity/EntitySelectStatement.h"
#include "Mdt/Entity/SqlTable.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

using namespace Mdt::Entity;

/*
 * Init / cleanup
 */

SelectQueryTest::SelectQueryTest()
 : mQuery(Mdt::QueryExpression::SelectQuery::make<Mdt::QueryExpression::SqlSelectQuery>())
{
}

void SelectQueryTest::initTestCase()
{
  QVERIFY(initDatabaseSqlite());
  QVERIFY(createTestSchema());
  auto & sqlSelectQuery = mQuery.impl<Mdt::QueryExpression::SqlSelectQuery>();
  sqlSelectQuery.setDatabase(database());
}

void SelectQueryTest::cleanupTestCase()
{
}

/*
 * Entities
 */

struct PersonDataStruct
{
  qulonglong id;
  QString firstName;
  int age;
  QString remarks;
};

MDT_ENTITY_DEF(
  (PersonDataStruct),
  Person,
  (id, FieldFlag::IsPrimaryKey),
  (firstName, FieldMaxLength(5)),
  (age),
  (remarks)
)

struct AddressDataStruct
{
  qulonglong id;
  QString street;
  qulonglong personId;
  QString remarks;
};

MDT_ENTITY_DEF(
  (AddressDataStruct),
  Address,
  (id, FieldFlag::IsPrimaryKey),
  (street, FieldMaxLength(20)),
  (personId),
  (remarks)
)

/*
 * Tests
 */

void SelectQueryTest::execQueryTest()
{
  QVERIFY(insertPerson(1, "P1", 10, "R1"));
  QVERIFY(insertPerson(2, "P2", 20, "R2"));

  EntitySelectStatement<PersonEntity> stm;
  stm.selectAllFields();

  QCOMPARE(mQuery.fieldCount(), 0);
  QVERIFY(mQuery.exec(stm));
  QCOMPARE(mQuery.fieldCount(), 4);

  QVERIFY(mQuery.next());
  QCOMPARE(mQuery.fieldCount(), 4);
  QCOMPARE(mQuery.value(0), QVariant(1));
  QCOMPARE(mQuery.value(1), QVariant("P1"));
  QCOMPARE(mQuery.value(2), QVariant(10));
  QCOMPARE(mQuery.value(3), QVariant("R1"));
  QVERIFY(mQuery.next());
  QCOMPARE(mQuery.value(0), QVariant(2));
  QCOMPARE(mQuery.value(1), QVariant("P2"));
  QCOMPARE(mQuery.value(2), QVariant(20));
  QCOMPARE(mQuery.value(3), QVariant("R2"));
  QVERIFY(!mQuery.next());

  QVERIFY(cleanupPersonTable());
}

void SelectQueryTest::fieldIndexTest()
{
  using Mdt::QueryExpression::SelectField;
  using Mdt::QueryExpression::FieldName;
  using Mdt::QueryExpression::EntityName;
  using Mdt::QueryExpression::SelectEntity;
  using Mdt::QueryExpression::SelectStatement;

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

  QVERIFY(mQuery.exec(stm));
  QCOMPARE(mQuery.fieldCount(), 3);
  QCOMPARE(mQuery.fieldIndex(remarks), 0);
  QCOMPARE(mQuery.fieldIndex(firstName), 1);
  QCOMPARE(mQuery.fieldIndex(age), 2);

  QVERIFY(cleanupPersonTable());
}

void SelectQueryTest::fieldIndexEntityTest()
{
  QVERIFY(insertPerson(1, "P1", 10, "R1"));
  QVERIFY(insertPerson(2, "P2", 20, "R2"));

  EntitySelectStatement<PersonEntity> stm;
  const auto firstName = stm.makeSelectField( stm.def().firstName() );
  const auto age = stm.makeSelectField( stm.def().age(), "PA" );
  const auto remarks = stm.makeSelectField( stm.def().remarks() );
  stm.addField(age);
  stm.addField(remarks);
  stm.addField(firstName);

  QVERIFY(mQuery.exec(stm));
  QCOMPARE(mQuery.fieldCount(), 3);
  QCOMPARE(mQuery.fieldIndex(age), 0);
  QCOMPARE(mQuery.fieldIndex(remarks), 1);
  QCOMPARE(mQuery.fieldIndex(firstName), 2);

  QVERIFY(cleanupPersonTable());
}

void SelectQueryTest::fieldIndexMultiEntityTest()
{

  QFAIL("Not complete");
}

void SelectQueryTest::execQueryFilterTest()
{
  QVERIFY(insertPerson(1, "P1", 10, "R1"));
  QVERIFY(insertPerson(2, "P2", 20, "R2"));
  QVERIFY(insertPerson(3, "P3", 30, "R3"));

  EntitySelectStatement<PersonEntity> stm;
  const auto age = stm.makeSelectField( stm.def().age() );
  stm.selectAllFields();
  stm.setFilter( (age > 10)&&(age < 30) );

  QVERIFY(mQuery.exec(stm));
  QCOMPARE(mQuery.fieldCount(), 4);
  const auto firstNameIndex = mQuery.fieldIndex( stm.makeSelectField(stm.def().firstName()) );
  const auto ageIndex = mQuery.fieldIndex(age);
  QVERIFY(mQuery.next());
  QCOMPARE(mQuery.value(firstNameIndex), QVariant("P2"));
  QCOMPARE(mQuery.value(ageIndex), QVariant(20));
  QVERIFY(!mQuery.next());

  QVERIFY(cleanupPersonTable());
}

/*
 * Helpers
 */

bool SelectQueryTest::createTestSchema()
{
  Q_ASSERT(database().isValid());

  Mdt::Sql::Schema::Driver driver(database());
  if(!driver.isValid()){
    qWarning() << "Could not create test schema, error: " << driver.lastError().text();
    return false;
  }

  if( !driver.createTable( Mdt::Entity::SqlTable::fromEntity<PersonEntity>() ) ){
    qWarning() << "Could not create table for PersonEntity, error: " << driver.lastError().text();
    return false;
  }
  if( !driver.createTable( Mdt::Entity::SqlTable::fromEntity<AddressEntity>() ) ){
    qWarning() << "Could not create table for AddressEntity, error: " << driver.lastError().text();
    return false;
  }

  return true;
}

bool SelectQueryTest::insertPerson(int id, const QString& firstName, int age, const QString& remarks)
{
  constexpr PersonEntity person;
  Mdt::Sql::InsertQuery query(database());

  query.setTableName(person.def().entityName());
  query.addValue( Mdt::Sql::FieldName(person.def().id().fieldName()), id );
  query.addValue( Mdt::Sql::FieldName(person.def().firstName().fieldName()), firstName );
  query.addValue( Mdt::Sql::FieldName(person.def().age().fieldName()), age );
  query.addValue( Mdt::Sql::FieldName(person.def().remarks().fieldName()), remarks );
  if(!query.exec()){
    qWarning() << "Insert person failed: " << query.lastError().text();
    return false;
  }

  return true;
}

bool SelectQueryTest::cleanupPersonTable()
{
  QSqlQuery query(database());

  if(!query.exec("DELETE FROM " + PersonEntity::def().entityName())){
    qWarning() << "Cleaning Person table failed: " << query.lastError();
    return false;
  }

  return true;
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

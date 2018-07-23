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

using namespace Mdt::Entity;

/*
 * Init / cleanup
 */

void SelectQueryTest::initTestCase()
{
  QVERIFY(initDatabaseSqlite());
  QVERIFY(createTestSchema());
  auto & sqlQueryFactory = mQueryFactory.makeImpl<Mdt::QueryExpression::SqlSelectQueryFactory>();
  sqlQueryFactory.setDatabase(database());
}

void SelectQueryTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void SelectQueryTest::execQueryTest()
{
  QVERIFY(insertPerson(1, "P1", 10, "R1"));
  QVERIFY(insertPerson(2, "P2", 20, "R2"));

  EntitySelectStatement<PersonEntity> stm;
  stm.selectAllFields();

  auto query = mQueryFactory.createQuery();
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

  auto query = mQueryFactory.createQuery();
  QVERIFY(query.exec(stm));
  QCOMPARE(query.fieldCount(), 3);
  QCOMPARE(query.fieldIndex(remarks), 0);
  QCOMPARE(query.fieldIndex(firstName), 1);
  QCOMPARE(query.fieldIndex(age), 2);

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

  auto query = mQueryFactory.createQuery();
  QVERIFY(query.exec(stm));
  QCOMPARE(query.fieldCount(), 3);
  QCOMPARE(query.fieldIndex(age), 0);
  QCOMPARE(query.fieldIndex(remarks), 1);
  QCOMPARE(query.fieldIndex(firstName), 2);

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

  auto query = mQueryFactory.createQuery();
  QVERIFY(query.exec(stm));
  QCOMPARE(query.fieldCount(), 4);
  const auto firstNameIndex = query.fieldIndex( stm.makeSelectField(stm.def().firstName()) );
  const auto ageIndex = query.fieldIndex(age);
  QVERIFY(query.next());
  QCOMPARE(query.value(firstNameIndex), QVariant("P2"));
  QCOMPARE(query.value(ageIndex), QVariant(20));
  QVERIFY(!query.next());

  QVERIFY(cleanupPersonTable());
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

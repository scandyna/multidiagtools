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
#include "Mdt/QueryExpression/SqlSelectQuery.h"
#include "Mdt/Sql/Schema/Driver.h"
#include "Mdt/Entity/Def.h"
#include "Mdt/Entity/QueryEntity.h"
#include "Mdt/Entity/EntitySelectQuery.h"
#include "Mdt/Entity/SqlTable.h"

using namespace Mdt::Entity;

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
  Mdt::QueryExpression::SqlSelectQuery sqlQuery(database());

  EntitySelectQuery<PersonEntity> query;
  query.selectAllFields();
  QVERIFY(sqlQuery.exec(query));
  /// \todo See QSqlQuery::next() etc...

  QFAIL("Not complete");
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


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  SelectQueryTest test;

  return QTest::qExec(&test, argc, argv);
}

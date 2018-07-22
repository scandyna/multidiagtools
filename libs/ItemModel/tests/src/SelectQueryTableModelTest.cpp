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
#include "SelectQueryTableModelTest.h"
#include "qtmodeltest.h"
#include "Mdt/ItemModel/SelectQueryTableModel.h"
#include "Mdt/QueryExpression/SelectStatement.h"
#include "Mdt/QueryExpression/SqlSelectQuery.h"
#include "Mdt/Entity/Def.h"
#include "Mdt/Entity/QueryEntity.h"
#include "Mdt/Entity/EntitySelectStatement.h"
#include "Mdt/Entity/SqlTable.h"
#include "Mdt/Sql/Schema/Driver.h"
#include "Mdt/Sql/InsertQuery.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSignalSpy>
#include <QVariant>
#include <QVariantList>
#include <QDebug>

using namespace Mdt::ItemModel;
using namespace Mdt::Entity;

/*
 * Init / cleanup
 */

SelectQueryTableModelTest::SelectQueryTableModelTest()
 : mQuery(Mdt::QueryExpression::CachedSelectQuery::make<Mdt::QueryExpression::SqlSelectQuery>())
{
}

void SelectQueryTableModelTest::initTestCase()
{
  QVERIFY(initDatabaseSqlite());
  QVERIFY(createTestSchema());
  auto & sqlSelectQuery = mQuery.impl<Mdt::QueryExpression::SqlSelectQuery>();
  sqlSelectQuery.setDatabase(database());
}

void SelectQueryTableModelTest::cleanupTestCase()
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

/*
 * Tests
 */

void SelectQueryTableModelTest::setupTest()
{
  SelectQueryTableModel model;
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.columnCount(), 0);

  model.setQuery(mQuery);
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.columnCount(), 0);
}

void SelectQueryTableModelTest::setupSignalTest()
{
  SelectQueryTableModel model;
  QSignalSpy modelResetSpy(&model, &SelectQueryTableModel::modelReset);
  QVERIFY(modelResetSpy.isValid());
  QCOMPARE(modelResetSpy.count(), 0);

  model.setQuery(mQuery);
  QCOMPARE(modelResetSpy.count(), 1);
}

void SelectQueryTableModelTest::setStatementTest()
{
  QVERIFY(insertPerson(1, "P1", 10, "R1"));
  QVERIFY(insertPerson(2, "P2", 20, "R2"));

  EntitySelectStatement<PersonEntity> stm;
  stm.selectAllFields();

  SelectQueryTableModel model;
  model.setQuery(mQuery);
  QVERIFY(model.setStatement(stm));
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(model.columnCount(), 4);
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QCOMPARE(getModelData(model, 0, 1), QVariant("P1"));
  QCOMPARE(getModelData(model, 0, 2), QVariant(10));
  QCOMPARE(getModelData(model, 0, 3), QVariant("R1"));
  QCOMPARE(getModelData(model, 1, 0), QVariant(2));
  QCOMPARE(getModelData(model, 1, 1), QVariant("P2"));
  QCOMPARE(getModelData(model, 1, 2), QVariant(20));
  QCOMPARE(getModelData(model, 1, 3), QVariant("R2"));

  QVERIFY(cleanupPersonTable());
}

void SelectQueryTableModelTest::setStatementSignalTest()
{
  SelectQueryTableModel model;
  model.setQuery(mQuery);
  QSignalSpy modelResetSpy(&model, &SelectQueryTableModel::modelReset);
  QVERIFY(modelResetSpy.isValid());
  QCOMPARE(modelResetSpy.count(), 0);

  EntitySelectStatement<PersonEntity> stm;
  stm.selectAllFields();
  QVERIFY(model.setStatement(stm));
  QCOMPARE(modelResetSpy.count(), 1);
}

void SelectQueryTableModelTest::qtModelTest()
{
  QVERIFY(insertPerson(1, "P1", 10, "R1"));
  QVERIFY(insertPerson(2, "P2", 20, "R2"));

  EntitySelectStatement<PersonEntity> stm;
  stm.selectAllFields();

  SelectQueryTableModel model;
  model.setQuery(mQuery);
  QVERIFY(model.setStatement(stm));
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(model.columnCount(), 4);
  QtModelTest mt(&model);

  QVERIFY(cleanupPersonTable());
}

/*
 * Helpers
 */

bool SelectQueryTableModelTest::createTestSchema()
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

  return true;
}

bool SelectQueryTableModelTest::insertPerson(int id, const QString& firstName, int age, const QString& remarks)
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

bool SelectQueryTableModelTest::cleanupPersonTable()
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
  SelectQueryTableModelTest test;

  return QTest::qExec(&test, argc, argv);
}

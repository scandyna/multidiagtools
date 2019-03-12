/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#include "QueryTest.h"
#include "Mdt/Sql/Schema/Driver.h"
// #include "Mdt/Sql/Query.h"
#include "Mdt/Sql/InsertStatement.h"
#include "Mdt/Sql/InsertQuery.h"
#include "Mdt/Sql/UpdateStatement.h"
#include "Mdt/Sql/UpdateQuery.h"
#include "Mdt/Sql/DeleteStatement.h"
#include "Mdt/Sql/DeleteQuery.h"
#include "Mdt/Sql/SelectQuery.h"
#include "Mdt/QueryExpression/SelectStatement.h"
#include "Mdt/Error.h"
#include "Mdt/ErrorCode.h"
#include "Schema/TestSchema.h"
#include "Schema/Client.h"
#include "Schema/Address.h"
#include <QSqlQuery>
#include <QSqlError>

// #include <QDebug>

using Mdt::Sql::FieldName;

/*
 * Init/cleanup functions
 */

void QueryTest::initTestCase()
{
  QVERIFY(initDatabaseSqlite());
  QVERIFY(createClientTable());
}

void QueryTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void QueryTest::insertStatementTest()
{
  Mdt::Sql::InsertStatement statement;
  QString expectedSql;
  const auto db = database();

  /*
   * Check basic API - 1 field/value
   */
  statement.setTableName("Client_tbl");
  statement.addValue(FieldName("Name"), "Name 1");
  expectedSql = "INSERT INTO \"Client_tbl\" (\"Name\") VALUES (?)";
  QCOMPARE(statement.toPrepareStatementSql(db), expectedSql);
  QCOMPARE(statement.toValueList(), QVariantList({"Name 1"}));

  /*
   * Check basic API - many values
   */
  statement.clear();
  statement.setTableName("Client_tbl");
  statement.addValue(FieldName("FirstName"), "F 1");
  statement.addValue(FieldName("LastName"), "L 1");
  expectedSql = "INSERT INTO \"Client_tbl\" (\"FirstName\",\"LastName\") VALUES (?,?)";
  QCOMPARE(statement.toPrepareStatementSql(db), expectedSql);
  QCOMPARE(statement.toValueList(), QVariantList({"F 1","L 1"}));
}

void QueryTest::insertQueryTest()
{
  Mdt::Sql::InsertQuery query(database());
  Schema::Client client;
  QString expectedSql;

  QVERIFY(cleanupClientTable());

  /*
   * Check basic API - 1 field/value
   */
  query.setTableName("Client_tbl");
  query.addValue(FieldName("Id_PK"), 1);
  QVERIFY(query.exec());
  QCOMPARE(query.lastInsertId(), QVariant(1));
  {
    QSqlQuery q(database());
    QVERIFY(q.exec("SELECT Id_PK, Name FROM Client_tbl"));
    QVERIFY(q.next());
    QCOMPARE(q.value(0), QVariant(1));
  }

  /*
   * Check basic API - many values
   */
  query.clear();
  query.setTableName("Client_tbl");
  query.addValue(FieldName("Id_PK"), 2);
  query.addValue(FieldName("Name"), "Name 2");
  QVERIFY(query.exec());
  QCOMPARE(query.lastInsertId(), QVariant(2));
  {
    QSqlQuery q(database());
    QVERIFY(q.exec("SELECT Id_PK, Name FROM Client_tbl"));
    QVERIFY(q.next());
    QVERIFY(q.next());
    QCOMPARE(q.value(0), QVariant(2));
    QCOMPARE(q.value(1), QVariant("Name 2"));
  }

  /*
   * Check more typed API
   */
  query.clear();
  query.setTable(client);
  query.addValue(client.Id_PK(), 3);
  query.addValue(client.Name(), "Name 3");
  QVERIFY(query.exec());
  QCOMPARE(query.lastInsertId(), QVariant(3));
  {
    QSqlQuery q(database());
    QVERIFY(q.exec("SELECT Id_PK, Name FROM Client_tbl"));
    QVERIFY(q.next());
    QVERIFY(q.next());
    QVERIFY(q.next());
    QCOMPARE(q.value(0), QVariant(3));
    QCOMPARE(q.value(1), QVariant("Name 3"));
  }

  /*
   * Execute statement
   */
  Mdt::Sql::InsertStatement statement;
  statement.setTableName("Client_tbl");
  statement.addValue(FieldName("Id_PK"), 4);
  statement.addValue(FieldName("Name"), "Name 4");
  QVERIFY(query.execStatement(statement));
  {
    QSqlQuery q(database());
    QVERIFY(q.exec("SELECT Id_PK, Name FROM Client_tbl"));
    QVERIFY(q.next());
    QVERIFY(q.next());
    QVERIFY(q.next());
    QVERIFY(q.next());
    QCOMPARE(q.value(0), QVariant(4));
    QCOMPARE(q.value(1), QVariant("Name 4"));
  }

  QVERIFY(cleanupClientTable());
}

void QueryTest::insertQueryErrorTest()
{
  QVERIFY(cleanupClientTable());

  Mdt::Sql::InsertQuery query(database());

  query.setTableName("Client_tbl");
  query.addValue(FieldName("Id_PK"), 1);
  query.addValue(FieldName("Name"), "Name 1");
  QVERIFY(query.exec());
  QVERIFY(!query.exec());
  QCOMPARE(query.lastError().error<Mdt::ErrorCode>(), Mdt::ErrorCode::UniqueConstraintError);

  QVERIFY(cleanupClientTable());
}

void QueryTest::updateStatementTest()
{
  Mdt::Sql::UpdateStatement statement;
  Mdt::Sql::PrimaryKeyRecord pkr;
  QString expectedSql;
  const auto db = database();

  /*
   * Check basic API - 1 field/value
   */
  statement.clear();
  statement.setTableName("Client_tbl");
  statement.addValue(FieldName("Name"), "Name 111");
  statement.setConditions( buildPrimaryKeyRecord(1) );
  expectedSql = "UPDATE \"Client_tbl\"\n"\
                "SET \"Name\"=?\n"
                "WHERE \"Id_PK\"=?";
  QCOMPARE(statement.toPrepareStatementSql(db), expectedSql);
  /*
   * Check basic API - many values
   */
  statement.clear();
  statement.setTableName("Client_tbl");
  statement.addValue(FieldName("FirstName"), "F");
  statement.addValue(FieldName("LastName"), "L");
  statement.setConditions( buildPrimaryKeyRecord(1) );
  expectedSql = "UPDATE \"Client_tbl\"\n"\
                "SET \"FirstName\"=?,\"LastName\"=?\n"
                "WHERE \"Id_PK\"=?";
  QCOMPARE(statement.toPrepareStatementSql(db), expectedSql);
  /*
   * Basic API - no condition
   */
  statement.clear();
  statement.setTableName("Client_tbl");
  statement.addValue(FieldName("Name"), "Name 111");
  expectedSql = "UPDATE \"Client_tbl\"\n"\
                "SET \"Name\"=?";
  QCOMPARE(statement.toPrepareStatementSql(db), expectedSql);
}

void QueryTest::updateStatementPrimaryKeyConditionsTest()
{
  Mdt::Sql::UpdateStatement statement;
  QString expectedSql;
  const auto db = database();

  /*
   * 1 field PK
   */
  statement.clear();
  statement.setTableName("Client_tbl");
  statement.addValue(FieldName("Name"), "Name 111");
  statement.setConditions( buildPrimaryKeyRecord(1) );
  expectedSql = "UPDATE \"Client_tbl\"\n"\
                "SET \"Name\"=?\n"
                "WHERE \"Id_PK\"=?";
  QCOMPARE(statement.toPrepareStatementSql(db), expectedSql);
  /*
   * 2 fields PK
   */
  statement.clear();
  statement.setTableName("Link_tbl");
  statement.addValue(FieldName("Identification"), "1234");
  statement.setConditions( buildPrimaryKeyRecord(1, 2) );
  expectedSql = "UPDATE \"Link_tbl\"\n"\
                "SET \"Identification\"=?\n"
                "WHERE \"IdA_PK\"=? AND \"IdB_PK\"=?";
  QCOMPARE(statement.toPrepareStatementSql(db), expectedSql);
}

void QueryTest::updateStatementToConditionsValueListTest()
{
  Mdt::Sql::UpdateStatement statement;

  /*
   * 1 field PK
   */
  statement.clear();
  statement.setTableName("Client_tbl");
  statement.addValue(FieldName("Name"), "Name 111");
  statement.setConditions( buildPrimaryKeyRecord(1) );
  QCOMPARE(statement.toConditionsValueList(), QVariantList({1}));
  /*
   * 2 fields PK
   */
  statement.setConditions( buildPrimaryKeyRecord(2,1) );
  QCOMPARE(statement.toConditionsValueList(), QVariantList({2,1}));
}

void QueryTest::updateQueryTest()
{
  Mdt::Sql::UpdateQuery query(database());

  QVERIFY(cleanupClientTable());
  QVERIFY(insertClient(1, "Name 1"));
  QVERIFY(insertClient(2, "Name 2"));

  query.setTableName("Client_tbl");
  query.addValue(FieldName("Name"), "A");
  query.setConditions( buildPrimaryKeyRecord(1) );
  QVERIFY(query.exec());
  {
    QSqlQuery q(database());
    QVERIFY(q.exec("SELECT Id_PK, Name FROM Client_tbl"));
    QVERIFY(q.next());
    QCOMPARE(q.value(0), QVariant(1));
    QCOMPARE(q.value(1), QVariant("A"));
    QVERIFY(q.next());
    QCOMPARE(q.value(0), QVariant(2));
    QCOMPARE(q.value(1), QVariant("Name 2"));
  }

  /*
   * Execute statement
   */
  Mdt::Sql::UpdateStatement statement;
  statement.setTableName("Client_tbl");
  statement.addValue(FieldName("Name"), "B");
  statement.setConditions( buildPrimaryKeyRecord(1) );
  QVERIFY(query.execStatement(statement));
  {
    QSqlQuery q(database());
    QVERIFY(q.exec("SELECT Id_PK, Name FROM Client_tbl"));
    QVERIFY(q.next());
    QCOMPARE(q.value(0), QVariant(1));
    QCOMPARE(q.value(1), QVariant("B"));
    QVERIFY(q.next());
    QCOMPARE(q.value(0), QVariant(2));
    QCOMPARE(q.value(1), QVariant("Name 2"));
  }

  QVERIFY(cleanupClientTable());
}

void QueryTest::updateQueryErrorTest()
{
  QVERIFY(cleanupClientTable());
  QVERIFY(insertClient(1, "Name 1"));
  QVERIFY(insertClient(2, "Name 2"));

  Mdt::Sql::UpdateQuery query(database());
  query.setTableName("Client_tbl");
  query.addValue(FieldName("Id_PK"), 1);
  query.addValue(FieldName("Name"), "Name 1");
  query.setConditions( buildPrimaryKeyRecord(2) );
  QVERIFY(!query.exec());
  QCOMPARE(query.lastError().error<Mdt::ErrorCode>(), Mdt::ErrorCode::UniqueConstraintError);
}

void QueryTest::deleteStatementFilterExpressionTest_1()
{
  using namespace Mdt::Sql;

  QString expectedSql;
  const auto db = database();

  QueryField id("Id_PK");

  DeleteStatement statement;
  statement.setTableName("Client_tbl");
  statement.setConditions( id == 21 );
  expectedSql = "DELETE FROM \"Client_tbl\"\n"\
                "WHERE \"Id_PK\"=21";
  QCOMPARE(statement.toSql(db), expectedSql);
}

void QueryTest::deleteStatementFilterExpressionTest_2()
{
  using namespace Mdt::Sql;
  using Mdt::QueryExpression::QueryField;

  QString expectedSql;
  const auto db = database();

  QueryField id("Id_PK");

  DeleteStatement statement;
  statement.setTableName("Client_tbl");
  statement.setConditions( id == 21 );
  expectedSql = "DELETE FROM \"Client_tbl\"\n"\
                "WHERE \"Id_PK\"=21";
  QCOMPARE(statement.toSql(db), expectedSql);
}

void QueryTest::deleteStatementPrimaryKeyConditionsTest()
{
  Mdt::Sql::DeleteStatement statement;
  QString expectedSql;
  const auto db = database();

  /*
   * 1 field PK
   */
  statement.setTableName("Client_tbl");
  statement.setConditions( buildPrimaryKeyRecord(1) );
  expectedSql = "DELETE FROM \"Client_tbl\"\n"\
                "WHERE \"Id_PK\"=1";
  QCOMPARE(statement.toSql(db), expectedSql);
  /*
   * 2 fields PK
   */
  statement.clear();
  statement.setTableName("Link_tbl");
  statement.setConditions( buildPrimaryKeyRecord(1, 2) );
  expectedSql = "DELETE FROM \"Link_tbl\"\n"\
                "WHERE (\"IdA_PK\"=1)AND(\"IdB_PK\"=2)";
  QCOMPARE(statement.toSql(db), expectedSql);
  /*
   * Clear
   */
  statement.clear();
  statement.setTableName("Client_tbl");
  expectedSql = "DELETE FROM \"Client_tbl\"";
  QCOMPARE(statement.toSql(db), expectedSql);
}

void QueryTest::deleteQueryTest()
{
  Mdt::Sql::DeleteQuery query(database());

  QVERIFY(cleanupClientTable());
  QVERIFY(insertClient(1, "Name 1"));
  QVERIFY(insertClient(2, "Name 2"));
  QVERIFY(insertClient(3, "Name 3"));
  QVERIFY(insertClient(4, "Name 4"));

  /*
   * Use PrimaryKeyRecord as conditions
   */
  query.setTableName("Client_tbl");
  query.setConditions( buildPrimaryKeyRecord(2) );
  QVERIFY(query.exec());
  {
    QSqlQuery q(database());
    QVERIFY(q.exec("SELECT Id_PK, Name FROM Client_tbl"));
    QVERIFY(q.next());
    QCOMPARE(q.value(0), QVariant(1));
    QCOMPARE(q.value(1), QVariant("Name 1"));
    QVERIFY(q.next());
    QCOMPARE(q.value(0), QVariant(3));
    QCOMPARE(q.value(1), QVariant("Name 3"));
    QVERIFY(q.next());
    QCOMPARE(q.value(0), QVariant(4));
    QCOMPARE(q.value(1), QVariant("Name 4"));
  }

  /*
   * Execute statement
   * Use a expression as conditions
   */
  Mdt::Sql::DeleteStatement statement;
  Mdt::Sql::QueryField id("Id_PK");
  statement.setTableName("Client_tbl");
  statement.setConditions(id == 3);
  QVERIFY(query.execStatement(statement));
  {
    QSqlQuery q(database());
    QVERIFY(q.exec("SELECT Id_PK, Name FROM Client_tbl"));
    QVERIFY(q.next());
    QCOMPARE(q.value(0), QVariant(1));
    QCOMPARE(q.value(1), QVariant("Name 1"));
    QVERIFY(q.next());
    QCOMPARE(q.value(0), QVariant(4));
    QCOMPARE(q.value(1), QVariant("Name 4"));
  }

  query.clear();
  query.setTableName("Client_tbl");
  QVERIFY(query.exec());
  {
    QSqlQuery q(database());
    QVERIFY(q.exec("SELECT Id_PK, Name FROM Client_tbl"));
    QVERIFY(!q.next());
  }
}

void QueryTest::selectQueryTest()
{
  using namespace Mdt::QueryExpression;

  Mdt::Sql::SelectQuery query(database());

  QVERIFY(cleanupClientTable());
  QVERIFY(insertClient(1, "Name 1"));
  QVERIFY(insertClient(2, "Name 2"));
  QVERIFY(insertClient(3, "Name 3"));

  /*
   * Execute a statement
   */
  SelectStatement statement;
  statement.setEntityName("Client_tbl");
  statement.addField("Id_PK");
  statement.addField("Name");
  QCOMPARE(query.fieldCount(), 0);
  QVERIFY(query.execStatement(statement));
  QCOMPARE(query.fieldCount(), 2);
  QVERIFY(query.next());
  QCOMPARE(query.fieldCount(), 2);
  QCOMPARE(query.value(0), QVariant(1));
  QCOMPARE(query.value(1), QVariant("Name 1"));
  QVERIFY(query.next());
  QCOMPARE(query.value(0), QVariant(2));
  QCOMPARE(query.value(1), QVariant("Name 2"));
  QVERIFY(query.next());
  QCOMPARE(query.value(0), QVariant(3));
  QCOMPARE(query.value(1), QVariant("Name 3"));

  /*
   * Fetch a single record
   */
  QueryField clientId("Id_PK");
  QSqlRecord record;

  QVERIFY(query.execStatement(statement));
  record = query.fetchSingleRecord();
  QVERIFY(record.isEmpty());
  statement.setFilter(clientId == 2);
  QVERIFY(query.execStatement(statement));
  record = query.fetchSingleRecord();
  QVERIFY(!record.isEmpty());
  QCOMPARE(record.value(0), QVariant(2));
  QCOMPARE(record.value(1), QVariant("Name 2"));
}

/*
 * Helpers
 */

Mdt::Sql::PrimaryKeyRecord QueryTest::buildPrimaryKeyRecord(int id)
{
  Mdt::Sql::PrimaryKeyRecord pkr;
  pkr.addValue(FieldName("Id_PK"), id);

  return pkr;
}

Mdt::Sql::PrimaryKeyRecord QueryTest::buildPrimaryKeyRecord(int idA, int idB)
{
  Mdt::Sql::PrimaryKeyRecord pkr;
  pkr.addValue(FieldName("IdA_PK"), idA);
  pkr.addValue(FieldName("IdB_PK"), idB);

  return pkr;
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  QueryTest test;

  return QTest::qExec(&test, argc, argv);
}

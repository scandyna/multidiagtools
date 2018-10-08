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
#include "QueryTest.h"
#include "Mdt/Sql/Schema/Driver.h"
// #include "Mdt/Sql/Query.h"
#include "Mdt/Sql/InsertQuery.h"
#include "Mdt/Sql/UpdateStatement.h"
#include "Mdt/Sql/UpdateQuery.h"
#include "Mdt/Sql/DeleteStatement.h"
#include "Mdt/Sql/DeleteQuery.h"
#include "Mdt/Sql/SelectQuery.h"
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
 * Class to test InsertQuery
 */
class InsertQueryTest : public Mdt::Sql::InsertQuery
{
 public:

  InsertQueryTest(const QSqlDatabase& db)
   : InsertQuery(db) {}

  QString getPrepareStatement() const
  {
    return InsertQuery::getPrepareStatement();
  }
};

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

void QueryTest::insertQueryTest()
{
  InsertQueryTest query(database());
  Schema::Client client;
  QString expectedSql;

  /*
   * Check basic API - 1 field/value
   */
  query.setTableName("Client_tbl");
  query.addValue(FieldName("Id_PK"), 1);
  // Check
  expectedSql = "INSERT INTO \"Client_tbl\" (\"Id_PK\") VALUES (?)";
  QCOMPARE(query.getPrepareStatement(), expectedSql);
  // Clear
  query.clear();
  /*
   * Check basic API - many values
   */
  query.setTableName("Client_tbl");
  query.addValue(FieldName("Id_PK"), 1);
  query.addValue(FieldName("Name"), "Name 1");
  // Check
  expectedSql = "INSERT INTO \"Client_tbl\" (\"Id_PK\",\"Name\") VALUES (?,?)";
  QCOMPARE(query.getPrepareStatement(), expectedSql);
  // Clear
  query.clear();
  /*
   * Check more typed API
   */
  query.setTable(client);
  query.addValue(client.Id_PK(), 1);
  query.addValue(client.Name(), "Name 1");
  // Check
  expectedSql = "INSERT INTO \"Client_tbl\" (\"Id_PK\",\"Name\") VALUES (?,?)";
  QCOMPARE(query.getPrepareStatement(), expectedSql);
  /*
   * Check insertion to database
   */
  // Make shure Client_tbl is empty
  QVERIFY(cleanupClientTable());
  // Insert and check
  QVERIFY(query.exec());
  {
    QSqlQuery q(database());
    QVERIFY(q.exec("SELECT Id_PK, Name FROM Client_tbl"));
    QVERIFY(q.next());
    QCOMPARE(q.value(0), QVariant(1));
    QCOMPARE(q.value(1), QVariant("Name 1"));
  }
  // Cleanup
  QVERIFY(cleanupClientTable());
}

void QueryTest::insertQueryErrorTest()
{
  QVERIFY(cleanupClientTable());

  InsertQueryTest query(database());

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
  }

  QVERIFY(cleanupClientTable());
}

void QueryTest::deleteStatementTest()
{
  Mdt::Sql::DeleteStatement statement;
  Mdt::Sql::PrimaryKeyRecord pkr;
  QString expectedSql;
  const auto db = database();

  /*
   * Check basic API - 1 field condition
   */
  statement.clear();
  statement.setTableName("Client_tbl");
  statement.setConditions( buildPrimaryKeyRecord(1) );
  expectedSql = "DELETE FROM \"Client_tbl\"\n"\
                "WHERE \"Id_PK\"=?";
  QCOMPARE(statement.toPrepareStatementSql(db), expectedSql);
  /*
   * Basic API - no condition
   */
  statement.clear();
  statement.setTableName("Client_tbl");
  expectedSql = "DELETE FROM \"Client_tbl\"";
  QCOMPARE(statement.toPrepareStatementSql(db), expectedSql);
}

void QueryTest::deleteStatementPrimaryKeyConditionsTest()
{
  Mdt::Sql::DeleteStatement statement;
  QString expectedSql;
  const auto db = database();

  /*
   * 1 field PK
   */
  statement.clear();
  statement.setTableName("Client_tbl");
  statement.setConditions( buildPrimaryKeyRecord(1) );
  expectedSql = "DELETE FROM \"Client_tbl\"\n"\
                "WHERE \"Id_PK\"=?";
  QCOMPARE(statement.toPrepareStatementSql(db), expectedSql);
  /*
   * 2 fields PK
   */
  statement.clear();
  statement.setTableName("Link_tbl");
  statement.setConditions( buildPrimaryKeyRecord(1, 2) );
  expectedSql = "DELETE FROM \"Link_tbl\"\n"\
                "WHERE \"IdA_PK\"=? AND \"IdB_PK\"=?";
  QCOMPARE(statement.toPrepareStatementSql(db), expectedSql);
}

void QueryTest::deleteStatementToConditionsValueListTest()
{
  Mdt::Sql::DeleteStatement statement;

  /*
   * 1 field PK
   */
  statement.clear();
  statement.setTableName("Client_tbl");
  statement.setConditions( buildPrimaryKeyRecord(1) );
  QCOMPARE(statement.toConditionsValueList(), QVariantList({1}));
  /*
   * 2 fields PK
   */
  statement.setConditions( buildPrimaryKeyRecord(2,1) );
  QCOMPARE(statement.toConditionsValueList(), QVariantList({2,1}));
}

void QueryTest::deleteQueryTest()
{
  Mdt::Sql::DeleteQuery query(database());

  QVERIFY(cleanupClientTable());
  QVERIFY(insertClient(1, "Name 1"));
  QVERIFY(insertClient(2, "Name 2"));
  QVERIFY(insertClient(3, "Name 3"));

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

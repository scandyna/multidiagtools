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
// #include "Mdt/Sql/SelectQuery.h"
#include "Mdt/Error.h"
#include "Schema/TestSchema.h"
#include "Schema/Client.h"
#include "Schema/Address.h"
#include <QSqlQuery>
#include <QSqlError>

#include <QDebug>

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
  /*
   * Init and open database
   */
  mDatabase = QSqlDatabase::addDatabase("QSQLITE");
  QVERIFY(mDatabase.isValid());
  QVERIFY(mTempFile.open());
  mTempFile.close();
  mDatabase.setDatabaseName(mTempFile.fileName());
  QVERIFY(mDatabase.open());
  /*
   * Create schema
   */
  Mdt::Sql::Schema::Driver driver(mDatabase);
  QVERIFY(driver.isValid());
  QVERIFY(driver.createSchema(Schema::TestSchema()));
}

void QueryTest::cleanupTestCase()
{
  mDatabase.close();
}

/*
 * Tests
 */

void QueryTest::insertQueryTest()
{
  InsertQueryTest query(mDatabase);
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
    QSqlQuery q(mDatabase);
    QVERIFY(q.exec("SELECT Id_PK, Name FROM Client_tbl"));
    QVERIFY(q.next());
    QCOMPARE(q.value(0), QVariant(1));
    QCOMPARE(q.value(1), QVariant("Name 1"));
  }
  // Cleanup
  QVERIFY(cleanupClientTable());
}

void QueryTest::updateStatementTest()
{
  Mdt::Sql::UpdateStatement statement;
  Mdt::Sql::PrimaryKeyRecord pkr;
  QString expectedSql;
  const auto db = mDatabase;

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
  const auto db = mDatabase;

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
  Mdt::Sql::UpdateQuery query(mDatabase);

  QVERIFY(cleanupClientTable());
  QVERIFY(insertClient(1, "Name 1"));

  query.setTableName("Client_tbl");
  query.addValue(FieldName("Name"), "A");
  query.setConditions( buildPrimaryKeyRecord(1) );
  QVERIFY(query.exec());
  {
    QSqlQuery q(mDatabase);
    QVERIFY(q.exec("SELECT Id_PK, Name FROM Client_tbl"));
    QVERIFY(q.next());
    QCOMPARE(q.value(0), QVariant(1));
    QCOMPARE(q.value(1), QVariant("A"));
  }

  QVERIFY(cleanupClientTable());
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

bool QueryTest::insertClient(int id, const QString& name)
{
  Schema::Client client;
  Mdt::Sql::InsertQuery query(mDatabase);
  query.setTable(client);
  query.addValue(client.Id_PK(), id);
  query.addValue(client.Name(), name);
  if(!query.exec()){
    qWarning() << "Insert client failed: " << query.lastError().text();
    return false;
  }
  return true;
}

bool QueryTest::cleanupClientTable()
{
  QSqlQuery query(mDatabase);
  if(!query.exec("DELETE FROM Client_tbl")){
    qWarning() << "Cleanup Client_tbl failed: " << query.lastError().text();
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
  QueryTest test;

  return QTest::qExec(&test, argc, argv);
}

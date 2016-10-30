/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "Mdt/Application.h"
#include "Mdt/Sql/Query.h"
#include "Mdt/Sql/InsertQuery.h"
#include "Mdt/Sql/SelectQuery.h"
#include "Schema/TestSchema.h"
#include "Schema/Client_tbl.h"
#include "Schema/Address_tbl.h"

#include <QDebug>

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
  pvDatabase = QSqlDatabase::addDatabase("QSQLITE");
  QVERIFY(pvDatabase.isValid());
  QVERIFY(pvTempFile.open());
  pvTempFile.close();
  pvDatabase.setDatabaseName(pvTempFile.fileName());
  QVERIFY(pvDatabase.open());
  /*
   * Create schema
   */
  Mdt::Sql::Schema::Driver driver(pvDatabase);
  QVERIFY(driver.isValid());
  QVERIFY(driver.createSchema(Schema::TestSchema()));
}

void QueryTest::cleanupTestCase()
{
  pvDatabase.close();
}

/*
 * Tests
 */

void QueryTest::insertQueryTest()
{
  using Mdt::Sql::FieldName;

  InsertQueryTest query(pvDatabase);
  Schema::Client_tbl client;
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
  {
    QSqlQuery q(pvDatabase);
    QVERIFY(q.exec("DELETE FROM Client_tbl"));
    QVERIFY(q.exec("SELECT * FROM Client_tbl"));
    QVERIFY(!q.next());
  }
  // Insert and check
  QVERIFY(query.exec());
  {
    QSqlQuery q(pvDatabase);
    QVERIFY(q.exec("SELECT Id_PK, Name FROM Client_tbl"));
    QVERIFY(q.next());
    QCOMPARE(q.value(0), QVariant(1));
    QCOMPARE(q.value(1), QVariant("Name 1"));
  }
  // Cleanup
  {
    QSqlQuery q(pvDatabase);
    QVERIFY(q.exec("DELETE FROM Client_tbl"));
  }
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  QueryTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}

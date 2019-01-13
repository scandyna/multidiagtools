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
#include "SchemaDriverPostgreSqlTest.h"
#include "Mdt/CoreApplication.h"
#include "Mdt/Sql/Schema/Driver.h"
#include "Mdt/Sql/Schema/DriverPostgreSQL.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <QDebug>

using namespace Mdt::Sql::Schema;

void SchemaDriverPostgreSqlTest::initTestCase()
{
  /*
   * Some tests needs a connection
   * to the server, and all access to a database
   *
   * The encoding of the database must be UTF8 (important for some tests).
   * Make also shure that the CITEXT extension is loaded and enabled for the test database.
   */
  const QString host = "localhost";
  const QString user = "TestUser";
  const QString pwd = "TestPassword";
  const QString dbName = "testdb";

  // Get database instance
  mDatabase = QSqlDatabase::addDatabase("QPSQL");
  if(!mDatabase.isValid()){
    QSKIP("QPSQL driver is not available - Skip all tests");  // Will also skip all tests
  }
  // Connect to test database
  mDatabase.setHostName(host);
  mDatabase.setUserName(user);
  mDatabase.setPassword(pwd);
  mDatabase.setDatabaseName(dbName);
  if(!mDatabase.open()){
    qWarning() << "Could not open database. Make shure that test database is created with correct login as defined in initTestCase()";
    qWarning() << "Reported error: " << mDatabase.lastError().text();
  }
  /*
   * Because some tests can be executed without a open connection to the server,
   * we not fail here if database could not be open.
   */
}

void SchemaDriverPostgreSqlTest::cleanupTestCase()
{
}

void SchemaDriverPostgreSqlTest::sandbox()
{
  mDatabase.setHostName("localhost");
  mDatabase.setUserName("postgres");
  mDatabase.setPassword("12345678");
  mDatabase.setDatabaseName("sandbox");
  QVERIFY(mDatabase.open());
  
  qDebug() << "Tables: " << mDatabase.tables(QSql::AllTables);
}

/*
 * Tests
 */

void SchemaDriverPostgreSqlTest::driverInstanceTest()
{
  using Mdt::Sql::Schema::Driver;
  using Mdt::Sql::Schema::DriverType;

  auto db = mDatabase;
  /*
   * Create a SQLite driver
   */
  QVERIFY(db.isValid());
  Driver driver(db);
  QVERIFY(driver.isValid());
  QVERIFY(driver.type() == DriverType::PostgreSQL);
}

void SchemaDriverPostgreSqlTest::availableFieldTypeTest()
{
  DriverPostgreSQL driver(mDatabase);
  auto list = driver.getAvailableFieldTypeList();

  QCOMPARE(list.size(), 8);
  QVERIFY(list.at(0) == FieldType::Boolean);
  QVERIFY(list.at(1) == FieldType::Integer);
  QVERIFY(list.at(2) == FieldType::Float);
  QVERIFY(list.at(3) == FieldType::Double);
  QVERIFY(list.at(4) == FieldType::Varchar);
  QVERIFY(list.at(5) == FieldType::Date);
  QVERIFY(list.at(6) == FieldType::Time);
  QVERIFY(list.at(7) == FieldType::DateTime);
}

void SchemaDriverPostgreSqlTest::fieldTypeNameTest()
{
  DriverPostgreSQL driver(mDatabase);

  QCOMPARE(driver.fieldTypeName(FieldType::UnknownType), QString());
  QCOMPARE(driver.fieldTypeName(FieldType::Boolean), QString("BOOLEAN"));
  QCOMPARE(driver.fieldTypeName(FieldType::Smallint), QString("SMALLINT"));
  QCOMPARE(driver.fieldTypeName(FieldType::Integer), QString("INTEGER"));
  QCOMPARE(driver.fieldTypeName(FieldType::Bigint), QString("BIGINT"));
  QCOMPARE(driver.fieldTypeName(FieldType::Float), QString("REAL"));
  QCOMPARE(driver.fieldTypeName(FieldType::Double), QString("DOUBLE PRECISION"));
  QCOMPARE(driver.fieldTypeName(FieldType::Char), QString("CHARACTER"));
  QCOMPARE(driver.fieldTypeName(FieldType::Varchar), QString("VARCHAR"));
  QCOMPARE(driver.fieldTypeName(FieldType::Text), QString("TEXT"));
  QCOMPARE(driver.fieldTypeName(FieldType::Blob), QString("BLOB"));
  QCOMPARE(driver.fieldTypeName(FieldType::Date), QString("DATE"));
  QCOMPARE(driver.fieldTypeName(FieldType::Time), QString("TIME"));
  QCOMPARE(driver.fieldTypeName(FieldType::DateTime), QString("TIMESTAMP"));
}

void SchemaDriverPostgreSqlTest::fieldTypeFromStringTest()
{
  DriverPostgreSQL driver(mDatabase);

  QVERIFY(driver.fieldTypeFromString("BOOLEAN") == FieldType::Boolean);
  QVERIFY(driver.fieldTypeFromString("boolean") == FieldType::Boolean);
  QVERIFY(driver.fieldTypeFromString("BOOL") == FieldType::Boolean);
  QVERIFY(driver.fieldTypeFromString("bool") == FieldType::Boolean);
  QVERIFY(driver.fieldTypeFromString("SMALLINT") == FieldType::Smallint);
  QVERIFY(driver.fieldTypeFromString("smallint") == FieldType::Smallint);
  QVERIFY(driver.fieldTypeFromString("int2") == FieldType::Smallint);
  QVERIFY(driver.fieldTypeFromString("INTEGER") == FieldType::Integer);
  QVERIFY(driver.fieldTypeFromString("integer") == FieldType::Integer);
  QVERIFY(driver.fieldTypeFromString("Integer") == FieldType::Integer);
  QVERIFY(driver.fieldTypeFromString("int4") == FieldType::Integer);
  QVERIFY(driver.fieldTypeFromString("BIGINT") == FieldType::Bigint);
  QVERIFY(driver.fieldTypeFromString("bigint") == FieldType::Bigint);
  QVERIFY(driver.fieldTypeFromString("int8") == FieldType::Bigint);
  QVERIFY(driver.fieldTypeFromString("SMALLSERIAL") == FieldType::Smallint);
  QVERIFY(driver.fieldTypeFromString("smallserial") == FieldType::Smallint);
  QVERIFY(driver.fieldTypeFromString("SERIAL") == FieldType::Integer);
  QVERIFY(driver.fieldTypeFromString("serial") == FieldType::Integer);
  QVERIFY(driver.fieldTypeFromString("BIGSERIAL") == FieldType::Bigint);
  QVERIFY(driver.fieldTypeFromString("bigserial") == FieldType::Bigint);
  QVERIFY(driver.fieldTypeFromString("DECIMAL") == FieldType::Double);
  QVERIFY(driver.fieldTypeFromString("decimal") == FieldType::Double);
  QVERIFY(driver.fieldTypeFromString("NUMERIC") == FieldType::Double);
  QVERIFY(driver.fieldTypeFromString("numeric") == FieldType::Double);
  QVERIFY(driver.fieldTypeFromString("REAL") == FieldType::Float);
  QVERIFY(driver.fieldTypeFromString("real") == FieldType::Float);
  QVERIFY(driver.fieldTypeFromString("DOUBLE") == FieldType::Double);
  QVERIFY(driver.fieldTypeFromString("double") == FieldType::Double);
  QVERIFY(driver.fieldTypeFromString("DOUBLE PRECISION") == FieldType::Double);
  QVERIFY(driver.fieldTypeFromString("double precision") == FieldType::Double);
  QVERIFY(driver.fieldTypeFromString("CHARACTER") == FieldType::Char);
  QVERIFY(driver.fieldTypeFromString("character") == FieldType::Char);
  QVERIFY(driver.fieldTypeFromString("CHAR") == FieldType::Char);
  QVERIFY(driver.fieldTypeFromString("char") == FieldType::Char);
  QVERIFY(driver.fieldTypeFromString("CHAR(1)") == FieldType::Char);
  QVERIFY(driver.fieldTypeFromString("CHAR(2)") == FieldType::Char);
  QVERIFY(driver.fieldTypeFromString("CHAR(10)") == FieldType::Char);
  QVERIFY(driver.fieldTypeFromString("VARCHAR") == FieldType::Varchar);
  QVERIFY(driver.fieldTypeFromString("varchar") == FieldType::Varchar);
  QVERIFY(driver.fieldTypeFromString("VARCHAR(50)") == FieldType::Varchar);
  QVERIFY(driver.fieldTypeFromString("VARCHAR (50)") == FieldType::Varchar);
  QVERIFY(driver.fieldTypeFromString("CHARACTER VARYING") == FieldType::Varchar);
  QVERIFY(driver.fieldTypeFromString("character varying") == FieldType::Varchar);
  QVERIFY(driver.fieldTypeFromString("CHARACTER VARYING(35)") == FieldType::Varchar);
  QVERIFY(driver.fieldTypeFromString("TEXT") == FieldType::Text);
  QVERIFY(driver.fieldTypeFromString("DATE") == FieldType::Date);
  QVERIFY(driver.fieldTypeFromString("date") == FieldType::Date);
  QVERIFY(driver.fieldTypeFromString("TIME") == FieldType::Time);
  QVERIFY(driver.fieldTypeFromString("time") == FieldType::Time);
  QVERIFY(driver.fieldTypeFromString("TIMESTAMP") == FieldType::DateTime);
  QVERIFY(driver.fieldTypeFromString("timestamp") == FieldType::DateTime);
}

void SchemaDriverPostgreSqlTest::fieldLengthFromStringTest()
{
  DriverPostgreSQL driver(mDatabase);


  QFAIL("Not complete");
}


void SchemaDriverPostgreSqlTest::databaseDefaultCharsetTest()
{
  using Mdt::Sql::Schema::Charset;

  Mdt::Sql::Schema::DriverPostgreSQL driver(mDatabase);
  Charset cs;

  /*
   * Encoding was specified in initTestCase()
   */
  cs = driver.getDatabaseDefaultCharset();
  QCOMPARE(cs.charsetName(), QString("UTF8"));
}

void SchemaDriverPostgreSqlTest::collationDefinitionTest()
{
  using Mdt::Sql::Schema::Collation;

  Mdt::Sql::Schema::DriverPostgreSQL driver(mDatabase);
  Collation collation;

  /*
   * Null collation
   */
  QVERIFY(driver.getCollationDefinition(collation).isEmpty());
  /*
   * Collation with only case sensitivity set
   * Will simply be ignored, because PostgreSQL does not support it
   * as COLLATE option.
   */
  collation.clear();
  collation.setCaseSensitive(false);
  QVERIFY(driver.getCollationDefinition(collation).isEmpty());
  /*
   * Collation with locale defined
   */
  collation.clear();
  collation.setLanguage(QLocale::French);
  collation.setCountry(QLocale::Switzerland);
  QCOMPARE(driver.getCollationDefinition(collation), QString("COLLATE \"fr_CH\""));
  /*
   * Collation with case sensitivity and locale defined
   * (Case sensitivity is simply ignored here)
   */
  collation.clear();
  collation.setCaseSensitive(false);
  collation.setLanguage(QLocale::French);
  collation.setCountry(QLocale::Switzerland);
  QCOMPARE(driver.getCollationDefinition(collation), QString("COLLATE \"fr_CH\""));
}

void SchemaDriverPostgreSqlTest::fieldDefinitionTest()
{
  using Mdt::Sql::Schema::Collation;
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::Field;

  Mdt::Sql::Schema::DriverPostgreSQL driver(mDatabase);
  QString expectedSql;
  Field field;

  QFAIL("No implemented yet");
}

void SchemaDriverPostgreSqlTest::autoIncrementPrimaryKeyDefinitionTest()
{
  DriverPostgreSQL driver(mDatabase);
  QString expectedSql;
  AutoIncrementPrimaryKey pk;

  pk.setFieldName("Id_PK");
  pk.setFieldType(FieldType::Smallint);
  expectedSql = "\"Id_PK\" SMALLSERIAL";
  QCOMPARE(driver.getPrimaryKeyFieldDefinition(pk), expectedSql);

  pk.setFieldType(FieldType::Integer);
  expectedSql = "\"Id_PK\" SERIAL";
  QCOMPARE(driver.getPrimaryKeyFieldDefinition(pk), expectedSql);

  pk.setFieldType(FieldType::Bigint);
  expectedSql = "\"Id_PK\" BIGSERIAL";
  QCOMPARE(driver.getPrimaryKeyFieldDefinition(pk), expectedSql);
}

void SchemaDriverPostgreSqlTest::singleFieldPrimaryKeyDefinitionTest()
{
  using Mdt::Sql::Schema::PrimaryKey;
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::Field;

  Mdt::Sql::Schema::DriverPostgreSQL driver(mDatabase);
  QString expectedSql;
  PrimaryKey pk;
  Field Id_A_PK, Id_B_PK;

  /*
   * Setup fields
   */
  Id_A_PK.setName("Id_A_PK");
  Id_B_PK.setName("Id_B_PK");
  /*
   * Simple 1 field PK
   */
  // Setup primary key
  pk.clear();
  pk.addField(Id_A_PK);
  // Check
  expectedSql = "PRIMARY KEY (`Id_A_PK`)";
  QCOMPARE(driver.getPrimaryKeyDefinition(pk), expectedSql);
  /*
   * Simple 2 field PK
   */
  // Setup primary key
  pk.clear();
  pk.addField(Id_A_PK);
  pk.addField(Id_B_PK);
  // Check
  expectedSql = "PRIMARY KEY (`Id_A_PK`,`Id_B_PK`)";
  QCOMPARE(driver.getPrimaryKeyDefinition(pk), expectedSql);
}

void SchemaDriverPostgreSqlTest::primaryKeyDefinitionTest()
{

}

void SchemaDriverPostgreSqlTest::indexDefinitionTest()
{

}

void SchemaDriverPostgreSqlTest::createTableTest()
{
  if(!mDatabase.isOpen()){
    QSKIP("Not connected to server");
  }
  Mdt::Sql::Schema::Driver driver(mDatabase);
  QVERIFY(driver.isValid());

  QVERIFY(dropTable("Client_tbl"));

  QSqlQuery q(mDatabase);
  if(!q.exec("CREATE TABLE Client_tbl (Id_PK INTEGER NOT NULL PRIMARY Key)")){
    qDebug() << q.lastError().text();
  }
  if(!q.exec("SELECT * FROM Client_tbl;")){
    qDebug() << q.lastError().text();
  }

  QVERIFY(dropTable("Client_tbl"));
}

/*
 * Helper functions
 */

bool SchemaDriverPostgreSqlTest::dropTable(const QString& tableName)
{
  QSqlQuery query(mDatabase);

  if(!query.exec("DROP TABLE IF EXISTS " + tableName + "")){
    qWarning() << query.lastError().text();
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
  SchemaDriverPostgreSqlTest test;

  return QTest::qExec(&test, argc, argv);
}

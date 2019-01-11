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
#include "SchemaDriverMySqlTest.h"
#include "Mdt/CoreApplication.h"
#include "Mdt/Sql/Schema/Driver.h"
#include "Mdt/Sql/Schema/DriverMySql.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <QDebug>

void SchemaDriverMySqlTest::initTestCase()
{
  /*
   * Some tests needs a connection
   * to the server, and all access to a database
   */
  const QString host = "localhost";
  const QString user = "TestUser";
  const QString pwd = "TestPassword";
  const QString dbName = "testdb";

  // Get database instance
  pvDatabase = QSqlDatabase::addDatabase("QMYSQL");
  if(!pvDatabase.isValid()){
    QSKIP("QMYSQL driver is not available - Skip all tests");  // Will also skip all tests
  }
  // Connect to test database
  pvDatabase.setHostName(host);
  pvDatabase.setUserName(user);
  pvDatabase.setPassword(pwd);
  pvDatabase.setDatabaseName(dbName);
  if(!pvDatabase.open()){
    qWarning() << "Could not open database. Make shure that test database is created with correct login as defined in initTestCase()";
    qWarning() << "Reported error: " << pvDatabase.lastError().text();
  }
  /*
   * Because some tests can be executed without a open connection to the server,
   * we not fail here if database could not be open.
   */
  QSqlQuery query(pvDatabase);
  // Define database default charset (is important for some tests)
  QVERIFY(query.exec("ALTER DATABASE `testdb` CHARACTER SET 'utf8'"));
}

void SchemaDriverMySqlTest::cleanupTestCase()
{
  /// \todo DROP all tables, etc...
}

/*
 * Tests
 */

void SchemaDriverMySqlTest::driverInstanceTest()
{
  using Mdt::Sql::Schema::Driver;
  using Mdt::Sql::Schema::DriverType;

  auto db = pvDatabase;
  /*
   * Create a SQLite driver
   */
  QVERIFY(db.isValid());
  Driver driver(db);
  QVERIFY(driver.isValid());
  QVERIFY(driver.type() == DriverType::MySQL);
}

void SchemaDriverMySqlTest::availableFieldTypeTest()
{
  using Mdt::Sql::Schema::FieldType;

  Mdt::Sql::Schema::Driver driver(pvDatabase);
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

void SchemaDriverMySqlTest::fieldTypeMapTest()
{
  using Mdt::Sql::Schema::FieldType;

  Mdt::Sql::Schema::Driver driver(pvDatabase);
  QVERIFY(driver.isValid());

  QFAIL("No complete");
  /*
   * Check FieldType <-> QMetaType mapping
   */
//   // QMetaType -> FieldType
//   QVERIFY(driver.fieldTypeFromQMetaType(QMetaType::UnknownType) == FieldType::UnknownType);
//   QVERIFY(driver.fieldTypeFromQMetaType(QMetaType::QPolygon) == FieldType::UnknownType);
//   QVERIFY(driver.fieldTypeFromQMetaType(QMetaType::Int) == FieldType::Integer);
//   QVERIFY(driver.fieldTypeFromQMetaType(QMetaType::Bool) == FieldType::Boolean);
//   QVERIFY(driver.fieldTypeFromQMetaType(QMetaType::Double) == FieldType::Double);
//   QVERIFY(driver.fieldTypeFromQMetaType(QMetaType::Float) == FieldType::Float);
//   QVERIFY(driver.fieldTypeFromQMetaType(QMetaType::QDate) == FieldType::Date);
//   QVERIFY(driver.fieldTypeFromQMetaType(QMetaType::QTime) == FieldType::Time);
//   QVERIFY(driver.fieldTypeFromQMetaType(QMetaType::QDateTime) == FieldType::DateTime);
//   QVERIFY(driver.fieldTypeFromQMetaType(QMetaType::QString) == FieldType::Varchar);
//   // FieldType -> QMetaType
//   QVERIFY(driver.fieldTypeToQMetaType(FieldType::UnknownType) == QMetaType::UnknownType);
//   QVERIFY(driver.fieldTypeToQMetaType(FieldType::Boolean) == QMetaType::Bool);
//   QVERIFY(driver.fieldTypeToQMetaType(FieldType::Integer) == QMetaType::Int);
//   QVERIFY(driver.fieldTypeToQMetaType(FieldType::Float) == QMetaType::Float);
//   QVERIFY(driver.fieldTypeToQMetaType(FieldType::Double) == QMetaType::Double);
//   QVERIFY(driver.fieldTypeToQMetaType(FieldType::Varchar) == QMetaType::QString);
//   QVERIFY(driver.fieldTypeToQMetaType(FieldType::Date) == QMetaType::QDate);
//   QVERIFY(driver.fieldTypeToQMetaType(FieldType::Time) == QMetaType::QTime);
//   QVERIFY(driver.fieldTypeToQMetaType(FieldType::DateTime) == QMetaType::QDateTime);
}

void SchemaDriverMySqlTest::databaseDefaultCharsetTest()
{
  using Mdt::Sql::Schema::Charset;

  Mdt::Sql::Schema::DriverMySql driver(pvDatabase);
  Charset cs;

  /*
   * Encoding was specified in initTestCase()
   */
  cs = driver.getDatabaseDefaultCharset();
  QCOMPARE(cs.charsetName(), QString("utf8"));
}

void SchemaDriverMySqlTest::collationDefinitionTest()
{
  using Mdt::Sql::Schema::Collation;

  Mdt::Sql::Schema::DriverMySql driver(pvDatabase);
  Collation collation;

  /*
   * Check that we have utf8 as default charset
   */
  QCOMPARE(driver.getDatabaseDefaultCharset().charsetName(), QString("utf8"));
  /*
   * Null collation
   */
  QVERIFY(driver.getCollationDefinition(collation).isEmpty());
  /*
   * Collation with only case sensitivity set
   */
  collation.clear();
  collation.setCaseSensitive(true);
  QCOMPARE(driver.getCollationDefinition(collation), QString("COLLATE utf8_bin"));
  collation.clear();
  collation.setCaseSensitive(false);
  QCOMPARE(driver.getCollationDefinition(collation), QString("COLLATE utf8_general_ci"));
  /*
   * Collation with only locale (language and country) set
   * Note: specifying a locale hase only sense for case insensitivity
   */
  collation.clear();
  collation.setLanguage(QLocale::Swedish);
  collation.setCountry(QLocale::Sweden);
  QCOMPARE(driver.getCollationDefinition(collation), QString("COLLATE utf8_swedish_ci"));
  /*
   * Collation with case sensitivity and locale set
   */
  collation.clear();
  collation.setCaseSensitive(false);
  collation.setLanguage(QLocale::Swedish);
  collation.setCountry(QLocale::Sweden);
  QCOMPARE(driver.getCollationDefinition(collation), QString("COLLATE utf8_swedish_ci"));
}

void SchemaDriverMySqlTest::fieldDefinitionTest()
{
  using Mdt::Sql::Schema::Collation;
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::Field;

  Mdt::Sql::Schema::DriverMySql driver(pvDatabase);
  QString expectedSql;
  Field field;

  QFAIL("No implemented yet");
}

void SchemaDriverMySqlTest::autoIncrementPrimaryKeyDefinitionTest()
{
  using Mdt::Sql::Schema::AutoIncrementPrimaryKey;

  Mdt::Sql::Schema::DriverMySql driver(pvDatabase);
  QString expectedSql;
  AutoIncrementPrimaryKey pk;

  pk.setFieldName("Id_PK");

  QFAIL("No implemented yet");
}

void SchemaDriverMySqlTest::singleFieldPrimaryKeyDefinitionTest()
{
  using Mdt::Sql::Schema::PrimaryKey;
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::Field;

  Mdt::Sql::Schema::DriverMySql driver(pvDatabase);
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

void SchemaDriverMySqlTest::primaryKeyDefinitionTest()
{

}

void SchemaDriverMySqlTest::indexDefinitionTest()
{

}

void SchemaDriverMySqlTest::createTableTest()
{
  if(!pvDatabase.isOpen()){
    QSKIP("Not connected to server");
  }
  Mdt::Sql::Schema::Driver driver(pvDatabase);
  QVERIFY(driver.isValid());

  
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  SchemaDriverMySqlTest test;

  return QTest::qExec(&test, argc, argv);
}

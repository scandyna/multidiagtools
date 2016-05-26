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
#include "SchemaDriverTest.h"
#include "Mdt/Application.h"
#include "Mdt/Sql/Schema/Driver.h"
#include "Mdt/Sql/Schema/DriverSQLite.h"
#include <QSqlDatabase>
#include <QSqlQuery>


void SchemaDriverTest::initTestCase()
{
  QSqlDatabase::addDatabase("QSQLITE", "SQLITE_1");
  QSqlDatabase::addDatabase("QMYSQL", "MYSQL_1");
}

void SchemaDriverTest::cleanupTestCase()
{
  QSqlDatabase::removeDatabase("SQLITE_1");
  QSqlDatabase::removeDatabase("MYSQL_1");
}

/*
 * Tests
 */

void SchemaDriverTest::driverTypeMapTest()
{
  using Mdt::Sql::Schema::Driver;
  using Mdt::Sql::Schema::DriverType;

  /*
   * Check QSqlDriver's DbmsType <-> DriverType
   */
  QVERIFY(Driver::typeFromDbmsType(QSqlDriver::UnknownDbms) == DriverType::Unknown);
  QVERIFY(Driver::typeFromDbmsType(QSqlDriver::MySqlServer) == DriverType::MySQL);
  QVERIFY(Driver::typeFromDbmsType(QSqlDriver::SQLite) == DriverType::SQLite);
}

void SchemaDriverTest::sqliteInstanceTest()
{
  using Mdt::Sql::Schema::Driver;
  using Mdt::Sql::Schema::DriverType;

  QSqlDatabase db = QSqlDatabase::database("SQLITE_1", false);
  if(!db.isValid()){
    QSKIP("SQLite driver not available");
  }

  /*
   * Create a SQLite driver
   */
  QVERIFY(db.isValid());
  Driver driver(db);
  QVERIFY(driver.isValid());
  QVERIFY(driver.type() == DriverType::SQLite);

}

void SchemaDriverTest::mysqlInstanceTest()
{
  using Mdt::Sql::Schema::Driver;
  using Mdt::Sql::Schema::DriverType;

  QSqlDatabase db = QSqlDatabase::database("MYSQL_1", false);
  if(!db.isValid()){
    QSKIP("MySQL driver not available");
  }

  /*
   * Create a MySQL driver
   */
  QVERIFY(db.isValid());
  Driver driver(db);
  QVERIFY(driver.isValid());
  QVERIFY(driver.type() == DriverType::MySQL);

}

void SchemaDriverTest::fieldTypeNameSqliteTest()
{
  using Mdt::Sql::Schema::FieldType;

  QSqlDatabase db = QSqlDatabase::database("SQLITE_1", false);
  if(!db.isValid()){
    QSKIP("SQLite driver not available");
  }
  Mdt::Sql::Schema::DriverSQLite driver(db);

  QVERIFY(driver.fieldTypeName(FieldType::UnknownType).isEmpty());
  QCOMPARE(driver.fieldTypeName(FieldType::Boolean), QString("BOOLEAN"));
  QCOMPARE(driver.fieldTypeName(FieldType::Integer), QString("INTEGER"));
  QCOMPARE(driver.fieldTypeName(FieldType::Float), QString("FLOAT"));
  QCOMPARE(driver.fieldTypeName(FieldType::Double), QString("DOUBLE"));
  QCOMPARE(driver.fieldTypeName(FieldType::Varchar), QString("VARCHAR"));
  QCOMPARE(driver.fieldTypeName(FieldType::Date), QString("DATE"));
  QCOMPARE(driver.fieldTypeName(FieldType::Time), QString("TIME"));
  QCOMPARE(driver.fieldTypeName(FieldType::DateTime), QString("DATETIME"));
}

void SchemaDriverTest::fieldTypeInfoListSqliteTest()
{
  using Mdt::Sql::Schema::Driver;
  using Mdt::Sql::Schema::FieldType;

  QSqlDatabase db = QSqlDatabase::database("SQLITE_1", false);
  if(!db.isValid()){
    QSKIP("SQLite driver not available");
  }
  /*
   * Create a SQLite driver
   */
  QVERIFY(db.isValid());
  Driver driver(db);
  QVERIFY(driver.isValid());
  /*
   * Check avaliable field types
   */
  auto list = driver.getAvailableFieldTypeInfoList();
  QCOMPARE(list.size(), 8);
  QVERIFY(list.at(0).type() == FieldType::Boolean);
  QCOMPARE(list.at(0).name(), QString("BOOLEAN"));

  QFAIL("Test not completely implemented yet");
}

void SchemaDriverTest::fieldTypeMapSqliteTest()
{
  using Mdt::Sql::Schema::Driver;
  using Mdt::Sql::Schema::FieldType;

  QSqlDatabase db = QSqlDatabase::database("SQLITE_1", false);
  if(!db.isValid()){
    QSKIP("SQLite driver not available");
  }
  /*
   * Create a SQLite driver
   */
  QVERIFY(db.isValid());
  Driver driver(db);
  QVERIFY(driver.isValid());
  /*
   * Check FieldType <-> QMetaType mapping
   */
  // QMetaType -> FieldType
  QVERIFY(driver.fieldTypeFromQMetaType(QMetaType::UnknownType) == FieldType::UnknownType);
  QVERIFY(driver.fieldTypeFromQMetaType(QMetaType::QPolygon) == FieldType::UnknownType);
  QVERIFY(driver.fieldTypeFromQMetaType(QMetaType::Int) == FieldType::Integer);
  QVERIFY(driver.fieldTypeFromQMetaType(QMetaType::Bool) == FieldType::Boolean);
  QVERIFY(driver.fieldTypeFromQMetaType(QMetaType::Double) == FieldType::Double);
  QVERIFY(driver.fieldTypeFromQMetaType(QMetaType::Float) == FieldType::Float);
  QVERIFY(driver.fieldTypeFromQMetaType(QMetaType::QDate) == FieldType::Date);
  QVERIFY(driver.fieldTypeFromQMetaType(QMetaType::QTime) == FieldType::Time);
  QVERIFY(driver.fieldTypeFromQMetaType(QMetaType::QDateTime) == FieldType::DateTime);
  QVERIFY(driver.fieldTypeFromQMetaType(QMetaType::QString) == FieldType::Varchar);
  // FieldType -> QMetaType
  QVERIFY(driver.fieldTypeToQMetaType(FieldType::UnknownType) == QMetaType::UnknownType);
  QVERIFY(driver.fieldTypeToQMetaType(FieldType::Boolean) == QMetaType::Bool);
  QVERIFY(driver.fieldTypeToQMetaType(FieldType::Integer) == QMetaType::Int);
  QVERIFY(driver.fieldTypeToQMetaType(FieldType::Float) == QMetaType::Float);
  QVERIFY(driver.fieldTypeToQMetaType(FieldType::Double) == QMetaType::Double);
  QVERIFY(driver.fieldTypeToQMetaType(FieldType::Varchar) == QMetaType::QString);
  QVERIFY(driver.fieldTypeToQMetaType(FieldType::Date) == QMetaType::QDate);
  QVERIFY(driver.fieldTypeToQMetaType(FieldType::Time) == QMetaType::QTime);
  QVERIFY(driver.fieldTypeToQMetaType(FieldType::DateTime) == QMetaType::QDateTime);
}

void SchemaDriverTest::collationDefinitionSqliteTest()
{
  using Mdt::Sql::Schema::Collation;

  QSqlDatabase db = QSqlDatabase::database("SQLITE_1", false);
  if(!db.isValid()){
    QSKIP("SQLite driver not available");
  }
  Mdt::Sql::Schema::DriverSQLite driver(db);
  Collation collation;

  /*
   * Null collation
   */
  QVERIFY(driver.getCollationDefinition(collation).isEmpty());
  /*
   * Case sensitive collation
   */
  // Setup collation
  collation.setCaseSensitive(true);
  collation.setCountry(QLocale::Switzerland); // Must simply be ignored
  collation.setLanguage(QLocale::French);     // Must simply be ignored
  collation.setCharset("utf8");               // Must simply be ignored
  // Check
  QCOMPARE(driver.getCollationDefinition(collation), QString("COLLATE BINARY"));
  // Setup collation
  collation.clear();
  collation.setCaseSensitive(true);
  // Check
  QCOMPARE(driver.getCollationDefinition(collation), QString("COLLATE BINARY"));
  /*
   * Case insensitive collation
   */
  // Setup collation
  collation.clear();
  collation.setCaseSensitive(false);
  // Check
  QCOMPARE(driver.getCollationDefinition(collation), QString("COLLATE NOCASE"));
}

void SchemaDriverTest::fieldDefinitionSqliteTest()
{
  using Mdt::Sql::Schema::Collation;
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::Field;

  QSqlDatabase db = QSqlDatabase::database("SQLITE_1", false);
  if(!db.isValid()){
    QSKIP("SQLite driver not available");
  }
  Mdt::Sql::Schema::DriverSQLite driver(db);
  QString expectedSql;
  Field field;

  /*
   * Simple INTEGER field
   */
  field.clear();
  field.setName("Number");
  field.setType(FieldType::Integer);
  // Check
  expectedSql = "\"Number\" INTEGER DEFAULT NULL";
  QCOMPARE(driver.getFieldDefinition(field), expectedSql);
  /*
   * INTEGER field with NOT NULL constraint
   */
  field.clear();
  field.setName("Number");
  field.setType(FieldType::Integer);
  field.setRequired(true);
  // Check
  expectedSql = "\"Number\" INTEGER NOT NULL DEFAULT NULL";
  QCOMPARE(driver.getFieldDefinition(field), expectedSql);
  /*
   * INTEGER field with a default value
   */
  field.clear();
  field.setName("Number");
  field.setType(FieldType::Integer);
  field.setDefaultValue(5);
  // Check
  expectedSql = "\"Number\" INTEGER DEFAULT 5";
  QCOMPARE(driver.getFieldDefinition(field), expectedSql);
  /*
   * Simple VARCHAR field
   */
  field.clear();
  field.setName("Name");
  field.setType(FieldType::Varchar);
  field.setLength(50);
  // Check
  expectedSql = "\"Name\" VARCHAR(50) DEFAULT NULL";
  QCOMPARE(driver.getFieldDefinition(field), expectedSql);
  /*
   * VARCHAR field with collation set
   */
  field.clear();
  field.setName("Name");
  field.setType(FieldType::Varchar);
  field.setLength(100);
  field.setCaseSensitive(false);
  // Check
  expectedSql = "\"Name\" VARCHAR(100) DEFAULT NULL COLLATE NOCASE";
  QCOMPARE(driver.getFieldDefinition(field), expectedSql);
  /*
   * VARCHAR field with NOT NULL constraint
   */
  field.clear();
  field.setName("Name");
  field.setType(FieldType::Varchar);
  field.setLength(25);
  field.setRequired(true);
  // Check
  expectedSql = "\"Name\" VARCHAR(25) NOT NULL DEFAULT NULL";
  QCOMPARE(driver.getFieldDefinition(field), expectedSql);
  /*
   * VARCHAR field with NOT NULL constraint and a collation set
   */
  field.clear();
  field.setName("Name");
  field.setType(FieldType::Varchar);
  field.setLength(50);
  field.setRequired(true);
  field.setCaseSensitive(false);
  // Check
  expectedSql = "\"Name\" VARCHAR(50) NOT NULL DEFAULT NULL COLLATE NOCASE";
  QCOMPARE(driver.getFieldDefinition(field), expectedSql);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  SchemaDriverTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}

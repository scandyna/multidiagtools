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
#include "SchemaDriverSqliteTest.h"
#include "Mdt/Application.h"
#include "Mdt/Sql/Schema/Driver.h"
#include "Mdt/Sql/Schema/DriverSQLite.h"
#include <QSqlDatabase>
#include <QSqlQuery>

void SchemaDriverSqliteTest::initTestCase()
{
  // Get database instance
  pvDatabase = QSqlDatabase::addDatabase("QSQLITE");
  if(!pvDatabase.isValid()){
    QSKIP("QSQLITE driver is not available - Skip all tests");  // Will also skip all tests
  }
  // Create a database
  /// \todo to be done when needed..
}

void SchemaDriverSqliteTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void SchemaDriverSqliteTest::driverInstanceTest()
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
  QVERIFY(driver.type() == DriverType::SQLite);
}

void SchemaDriverSqliteTest::availableFieldTypeTest()
{
  using Mdt::Sql::Schema::FieldType;

  Mdt::Sql::Schema::Driver driver(pvDatabase);
  QVERIFY(driver.isValid());
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

void SchemaDriverSqliteTest::fieldTypeMapTest()
{
  using Mdt::Sql::Schema::FieldType;

  Mdt::Sql::Schema::Driver driver(pvDatabase);
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

void SchemaDriverSqliteTest::collationDefinitionTest()
{
  using Mdt::Sql::Schema::Collation;

  Mdt::Sql::Schema::DriverSQLite driver(pvDatabase);
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

void SchemaDriverSqliteTest::fieldDefinitionTest()
{
  using Mdt::Sql::Schema::Collation;
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::Field;

  Mdt::Sql::Schema::DriverSQLite driver(pvDatabase);
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
   * INTEGER field with UNIQUE constraint
   */
  field.clear();
  field.setName("Number");
  field.setType(FieldType::Integer);
  field.setUnique(true);
  // Check
  expectedSql = "\"Number\" INTEGER UNIQUE DEFAULT NULL";
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
   * INTEGER field with UNIQUE and NOT NULL constraints
   */
  field.clear();
  field.setName("Number");
  field.setType(FieldType::Integer);
  field.setRequired(true);
  field.setUnique(true);
  // Check
  expectedSql = "\"Number\" INTEGER UNIQUE NOT NULL DEFAULT NULL";
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

void SchemaDriverSqliteTest::autoIncrementPrimaryKeyDefinitionTest()
{
  using Mdt::Sql::Schema::AutoIncrementPrimaryKey;

  Mdt::Sql::Schema::DriverSQLite driver(pvDatabase);
  QString expectedSql;
  AutoIncrementPrimaryKey pk;

  pk.setFieldName("Id_PK");
  /*
   * Note: SQL statement must contain NOT NULL (see SQLite documentation about NULL and PK)
   */
  expectedSql = "\"Id_PK\" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT";
  QCOMPARE(driver.getPrimaryKeyFieldDefinition(pk), expectedSql);
}

void SchemaDriverSqliteTest::singleFieldPrimaryKeyDefinitionTest()
{
  using Mdt::Sql::Schema::SingleFieldPrimaryKey;
  using Mdt::Sql::Schema::FieldType;

  Mdt::Sql::Schema::DriverSQLite driver(pvDatabase);
  QString expectedSql;
  SingleFieldPrimaryKey pk;

  /*
   * Integer primary key
   * Note: SQL statement must contain NOT NULL (see SQLite documentation about NULL and PK)
   */
  // Setup primary key
  pk.clear();
  pk.setFieldType(FieldType::Integer);
  pk.setFieldName("Id_PK");
  // Check
  expectedSql = "\"Id_PK\" INTEGER NOT NULL PRIMARY KEY";
  QCOMPARE(driver.getPrimaryKeyFieldDefinition(pk), expectedSql);
  /*
   * Text primary key
   * Note: SQL statement must contain NOT NULL (see SQLite documentation about NULL and PK)
   */
  // Setup primary key
  pk.clear();
  pk.setFieldType(FieldType::Varchar);
  pk.setFieldLength(20);
  pk.setFieldName("Code_PK");
  // Check
  expectedSql = "\"Code_PK\" VARCHAR(20) NOT NULL PRIMARY KEY";
  QCOMPARE(driver.getPrimaryKeyFieldDefinition(pk), expectedSql);
  /*
   * Text primary key
   * We specifiy a collation
   * Note: SQL statement must contain NOT NULL (see SQLite documentation about NULL and PK)
   */
  // Setup primary key
  pk.clear();
  pk.setFieldType(FieldType::Varchar);
  pk.setFieldLength(20);
  pk.setFieldName("Code_PK");
  pk.setCaseSensitive(true);
  // Check
  expectedSql = "\"Code_PK\" VARCHAR(20) NOT NULL PRIMARY KEY COLLATE BINARY";
  QCOMPARE(driver.getPrimaryKeyFieldDefinition(pk), expectedSql);
}

void SchemaDriverSqliteTest::primaryKeyDefinitionTest()
{
  using Mdt::Sql::Schema::PrimaryKey;
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::Field;

  Mdt::Sql::Schema::DriverSQLite driver(pvDatabase);
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
  expectedSql = "PRIMARY KEY (\"Id_A_PK\")";
  QCOMPARE(driver.getPrimaryKeyDefinition(pk), expectedSql);
  /*
   * Simple 2 field PK
   */
  // Setup primary key
  pk.clear();
  pk.addField(Id_A_PK);
  pk.addField(Id_B_PK);
  // Check
  expectedSql = "PRIMARY KEY (\"Id_A_PK\",\"Id_B_PK\")";
  QCOMPARE(driver.getPrimaryKeyDefinition(pk), expectedSql);
}

void SchemaDriverSqliteTest::indexDefinitionTest()
{
  using Mdt::Sql::Schema::Field;
  using Mdt::Sql::Schema::Index;
  using Mdt::Sql::Schema::Table;

  Mdt::Sql::Schema::DriverSQLite driver(pvDatabase);
  QString expectedSql;
  Field Id_A, Id_B;
  Index index;

  /*
   * Setup fields
   */
  Id_A.setName("Id_A");
  Id_B.setName("Id_B");
  /*
   * Setup a table
   */
  Table Client_tbl;
  Client_tbl.setTableName("Client_tbl");
  /*
   * Non unique index with 1 field
   */
  // Setup index
  index.clear();
  index.setTable(Client_tbl);
  index.addField(Id_A);
  index.generateName();
  // Check
  expectedSql = "CREATE INDEX \"Client_tbl_Id_A_index\" ON \"Client_tbl\" (\"Id_A\")";
  QCOMPARE(driver.getSqlToCreateIndex(index), expectedSql);
  expectedSql = "DROP INDEX IF EXISTS \"Client_tbl_Id_A_index\"";
  QCOMPARE(driver.getSqlToDropIndex(index), expectedSql);
  /*
   * Non unique index with 2 fields
   */
  // Setup index
  index.clear();
  index.setTable(Client_tbl);
  index.addField(Id_A);
  index.addField(Id_B);
  index.generateName();
  // Check
  expectedSql = "CREATE INDEX \"Client_tbl_Id_A_Id_B_index\" ON \"Client_tbl\" (\"Id_A\",\"Id_B\")";
  QCOMPARE(driver.getSqlToCreateIndex(index), expectedSql);
  expectedSql = "DROP INDEX IF EXISTS \"Client_tbl_Id_A_Id_B_index\"";
  QCOMPARE(driver.getSqlToDropIndex(index), expectedSql);
  /*
   * Unique index with 1 field
   */
  // Setup index
  index.clear();
  index.setUnique(true);
  index.setTable(Client_tbl);
  index.addField(Id_A);
  index.generateName();
  // Check
  expectedSql = "CREATE UNIQUE INDEX \"Client_tbl_Id_A_index\" ON \"Client_tbl\" (\"Id_A\")";
  QCOMPARE(driver.getSqlToCreateIndex(index), expectedSql);
  expectedSql = "DROP INDEX IF EXISTS \"Client_tbl_Id_A_index\"";
  QCOMPARE(driver.getSqlToDropIndex(index), expectedSql);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  SchemaDriverSqliteTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}

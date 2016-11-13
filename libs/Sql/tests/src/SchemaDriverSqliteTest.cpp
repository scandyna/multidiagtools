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
#include "Schema/Client_tbl.h"
#include "Schema/Address_tbl.h"
#include "Schema/ClientAddressView.h"
#include "Schema/TestSchema.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

void SchemaDriverSqliteTest::initTestCase()
{
  // Get database instance
  mDatabase = QSqlDatabase::addDatabase("QSQLITE");
  if(!mDatabase.isValid()){
    QSKIP("QSQLITE driver is not available - Skip all tests");  // Will also skip all tests
  }
  // Create a database
  QVERIFY(pvTempFile.open());
  pvTempFile.close();
  mDatabase.setDatabaseName(pvTempFile.fileName());
  QVERIFY(mDatabase.open());
  QSqlQuery query(mDatabase);
  // Specify encoding (is important for some tests)
  QVERIFY(query.exec("PRAGMA encoding = \"UTF-8\""));
}

void SchemaDriverSqliteTest::cleanupTestCase()
{
  mDatabase.close();
}

/*
 * Tests
 */

void SchemaDriverSqliteTest::driverInstanceTest()
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
  QVERIFY(driver.type() == DriverType::SQLite);
}

void SchemaDriverSqliteTest::availableFieldTypeTest()
{
  using Mdt::Sql::Schema::FieldType;

  Mdt::Sql::Schema::Driver driver(mDatabase);
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

  Mdt::Sql::Schema::Driver driver(mDatabase);
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
  // QVariant::Type -> FieldType
  QVERIFY(driver.fieldTypeFromQVariantType(QVariant::Int) == FieldType::Integer);
  QVERIFY(driver.fieldTypeFromQVariantType(QVariant::Bool) == FieldType::Boolean);
  QVERIFY(driver.fieldTypeFromQVariantType(QVariant::String) == FieldType::Varchar);
  // FieldType -> QVariant::Type
  QVERIFY(driver.fieldTypeToQVariantType(FieldType::Integer) == QVariant::Int);
  QVERIFY(driver.fieldTypeToQVariantType(FieldType::Boolean) == QVariant::Bool);
  QVERIFY(driver.fieldTypeToQVariantType(FieldType::Varchar) == QVariant::String);
}

void SchemaDriverSqliteTest::fieldTypeFromStringTest()
{
  using Mdt::Sql::Schema::FieldType;

  Mdt::Sql::Schema::DriverSQLite driver(mDatabase);

  QVERIFY(driver.fieldTypeFromString("BOOLEAN") == FieldType::Boolean);
  QVERIFY(driver.fieldTypeFromString("boolean") == FieldType::Boolean);
  QVERIFY(driver.fieldTypeFromString("BOOL") == FieldType::Boolean);
  QVERIFY(driver.fieldTypeFromString("bool") == FieldType::Boolean);
  QVERIFY(driver.fieldTypeFromString("INTEGER") == FieldType::Integer);
  QVERIFY(driver.fieldTypeFromString("integer") == FieldType::Integer);
  QVERIFY(driver.fieldTypeFromString("Integer") == FieldType::Integer);
  QVERIFY(driver.fieldTypeFromString("INT") == FieldType::Integer);
  QVERIFY(driver.fieldTypeFromString("int") == FieldType::Integer);
  QVERIFY(driver.fieldTypeFromString("FLOAT") == FieldType::Float);
  QVERIFY(driver.fieldTypeFromString("float") == FieldType::Float);
  QVERIFY(driver.fieldTypeFromString("DOUBLE") == FieldType::Double);
  QVERIFY(driver.fieldTypeFromString("double") == FieldType::Double);
  QVERIFY(driver.fieldTypeFromString("VARCHAR") == FieldType::Varchar);
  QVERIFY(driver.fieldTypeFromString("varchar") == FieldType::Varchar);
  QVERIFY(driver.fieldTypeFromString("VARCHAR(50)") == FieldType::Varchar);
  QVERIFY(driver.fieldTypeFromString("VARCHAR (50)") == FieldType::Varchar);
  QVERIFY(driver.fieldTypeFromString("DATE") == FieldType::Date);
  QVERIFY(driver.fieldTypeFromString("date") == FieldType::Date);
  QVERIFY(driver.fieldTypeFromString("TIME") == FieldType::Time);
  QVERIFY(driver.fieldTypeFromString("time") == FieldType::Time);
  QVERIFY(driver.fieldTypeFromString("DATETIME") == FieldType::DateTime);
  QVERIFY(driver.fieldTypeFromString("datetime") == FieldType::DateTime);
  QVERIFY(driver.fieldTypeFromString("Unkown") == FieldType::UnknownType);
}

void SchemaDriverSqliteTest::fieldLengthFromStringTest()
{
  Mdt::Sql::Schema::DriverSQLite driver(mDatabase);

  QCOMPARE(driver.fieldLengthFromString("INTEGER"), -1);
  QCOMPARE(driver.fieldLengthFromString("VARCHAR"), -1);
  QCOMPARE(driver.fieldLengthFromString("VARCHAR(50)"), 50);
  QCOMPARE(driver.fieldLengthFromString("VARCHAR (50)"), 50);
  QCOMPARE(driver.fieldLengthFromString("varcher(50)"), 50);
  QCOMPARE(driver.fieldLengthFromString("VARCHAR( 50 )"), 50);
  QCOMPARE(driver.fieldLengthFromString("VARCHAR ( 50 )"), 50);
  // Currently not supported..
  QCOMPARE(driver.fieldLengthFromString("DOUBLE(2,3)"), -2);
}

void SchemaDriverSqliteTest::databaseDefaultCharsetTest()
{
  using Mdt::Sql::Schema::Charset;

  Mdt::Sql::Schema::DriverSQLite driver(mDatabase);
  Charset cs;

  /*
   * Encoding was specified in initTestCase()
   */
  cs = driver.getDatabaseDefaultCharset();
  QCOMPARE(cs.charsetName(), QString("UTF-8"));
}

void SchemaDriverSqliteTest::collationDefinitionTest()
{
  using Mdt::Sql::Schema::Collation;

  Mdt::Sql::Schema::DriverSQLite driver(mDatabase);
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
//   collation.setCharset("utf8");               // Must simply be ignored
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

  Mdt::Sql::Schema::DriverSQLite driver(mDatabase);
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
   * VARCHAR field with a default value
   */
  field.clear();
  field.setName("Name");
  field.setType(FieldType::Varchar);
  field.setLength(150);
  field.setDefaultValue("Default name");
  // Check
  expectedSql = "\"Name\" VARCHAR(150) DEFAULT \"Default name\"";
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

  Mdt::Sql::Schema::DriverSQLite driver(mDatabase);
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

  Mdt::Sql::Schema::DriverSQLite driver(mDatabase);
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

  Mdt::Sql::Schema::DriverSQLite driver(mDatabase);
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

  Mdt::Sql::Schema::DriverSQLite driver(mDatabase);
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

void SchemaDriverSqliteTest::foreignKeyDefinitionTest()
{
  using Mdt::Sql::Schema::Field;
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::AutoIncrementPrimaryKey;
  using Mdt::Sql::Schema::PrimaryKey;
  using Mdt::Sql::Schema::Table;
  using Mdt::Sql::Schema::ForeignKey;
  using Mdt::Sql::Schema::ParentTableFieldName;
  using Mdt::Sql::Schema::ChildTableFieldName;

  Mdt::Sql::Schema::DriverSQLite driver(mDatabase);
  QString expectedSql;
  ForeignKey fk;

  /*
   * Setup fields
   */
  // Id_PK
  AutoIncrementPrimaryKey Id_PK;
  Id_PK.setFieldName("Id_PK");
  // Id_A_PK
  Field Id_A_PK;
  Id_A_PK.setName("Id_A_PK");
  Id_A_PK.setType(FieldType::Integer);
  Id_A_PK.setRequired(true);
  // Id_B_PK
  Field Id_B_PK;
  Id_B_PK.setName("Id_B_PK");
  Id_B_PK.setType(FieldType::Integer);
  Id_B_PK.setRequired(true);
  // Client_Id_FK
  Field Client_Id_FK;
  Client_Id_FK.setName("Client_Id_FK");
  Client_Id_FK.setType(FieldType::Integer);
  Client_Id_FK.setRequired(true);
  // Id_A_FK
  Field Id_A_FK;
  Id_A_FK.setName("Id_A_FK");
  Id_A_FK.setType(FieldType::Integer);
  Id_A_FK.setRequired(true);
  // Id_B_PK
  Field Id_B_FK;
  Id_B_FK.setName("Id_B_FK");
  Id_B_FK.setType(FieldType::Integer);
  Id_B_FK.setRequired(true);
  /*
   * Setup Tables
   */
  // Client_tbl
  Table Client_tbl;
  Client_tbl.setTableName("Client_tbl");
  // Address_tbl
  Table Address_tbl;
  Address_tbl.setTableName("Address_tbl");
  /*
   * Simple 1 field FK
   */
  // Setup FK
  fk.clear();
  fk.setParentTable(Client_tbl);
  fk.addKeyFields(ParentTableFieldName(Id_PK), ChildTableFieldName(Client_Id_FK));
  // Check
  expectedSql  = "  FOREIGN KEY (\"Client_Id_FK\")\n";
  expectedSql += "   REFERENCES \"Client_tbl\" (\"Id_PK\")\n";
  expectedSql += "   ON DELETE NO ACTION\n";
  expectedSql += "   ON UPDATE NO ACTION";
  QCOMPARE(driver.getForeignKeyDefinition(fk), expectedSql);
  // Set on delete and update actions
  fk.setOnDeleteAction(ForeignKey::Restrict);
  fk.setOnUpdateAction(ForeignKey::Cascade);
  expectedSql  = "  FOREIGN KEY (\"Client_Id_FK\")\n";
  expectedSql += "   REFERENCES \"Client_tbl\" (\"Id_PK\")\n";
  expectedSql += "   ON DELETE RESTRICT\n";
  expectedSql += "   ON UPDATE CASCADE";
  QCOMPARE(driver.getForeignKeyDefinition(fk), expectedSql);
  /*
   * Simple 2 fields FK
   */
  // Setup FK
  fk.clear();
  fk.setParentTable(Client_tbl);
  fk.addKeyFields(ParentTableFieldName(Id_A_PK), ChildTableFieldName(Id_A_FK));
  fk.addKeyFields(ParentTableFieldName(Id_B_PK), ChildTableFieldName(Id_B_FK));
  // Check
  expectedSql  = "  FOREIGN KEY (\"Id_A_FK\",\"Id_B_FK\")\n";
  expectedSql += "   REFERENCES \"Client_tbl\" (\"Id_A_PK\",\"Id_B_PK\")\n";
  expectedSql += "   ON DELETE NO ACTION\n";
  expectedSql += "   ON UPDATE NO ACTION";
  QCOMPARE(driver.getForeignKeyDefinition(fk), expectedSql);
  // Set on delete and update actions
  fk.setOnDeleteAction(ForeignKey::Restrict);
  fk.setOnUpdateAction(ForeignKey::Cascade);
  expectedSql  = "  FOREIGN KEY (\"Id_A_FK\",\"Id_B_FK\")\n";
  expectedSql += "   REFERENCES \"Client_tbl\" (\"Id_A_PK\",\"Id_B_PK\")\n";
  expectedSql += "   ON DELETE RESTRICT\n";
  expectedSql += "   ON UPDATE CASCADE";
  QCOMPARE(driver.getForeignKeyDefinition(fk), expectedSql);
}

void SchemaDriverSqliteTest::tableDefinitionTest()
{
  using Mdt::Sql::Schema::Field;
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::AutoIncrementPrimaryKey;
  using Mdt::Sql::Schema::SingleFieldPrimaryKey;
  using Mdt::Sql::Schema::PrimaryKey;
  using Mdt::Sql::Schema::Table;
  using Mdt::Sql::Schema::ParentTableFieldName;
  using Mdt::Sql::Schema::ChildTableFieldName;
  using Mdt::Sql::Schema::ForeignKey;

  Mdt::Sql::Schema::DriverSQLite driver(mDatabase);
  QString expectedSql;
  Table table;

  /*
   * Setup fields, primary keys, foreeign keys
   */
  // Id_PK
  AutoIncrementPrimaryKey Id_PK;
  Id_PK.setFieldName("Id_PK");
  // Code_PK
  SingleFieldPrimaryKey Code_PK;
  Code_PK.setFieldName("Code_PK");
  Code_PK.setFieldType(FieldType::Varchar);
  Code_PK.setFieldLength(20);
  // Id_A
  Field Id_A;
  Id_A.setName("Id_A");
  Id_A.setType(FieldType::Integer);
  Id_A.setRequired(true);
  // Id_B
  Field Id_B;
  Id_B.setName("Id_B");
  Id_B.setType(FieldType::Integer);
  Id_B.setRequired(true);
  // Id_AB_PK primary key
  PrimaryKey Id_AB_PK;
  Id_AB_PK.addField(Id_A);
  Id_AB_PK.addField(Id_B);
  // Name
  Field Name;
  Name.setName("Name");
  Name.setType(FieldType::Varchar);
  Name.setLength(50);
  // Remarks
  Field Remarks;
  Remarks.setName("Remarks");
  Remarks.setType(FieldType::Varchar);
  Remarks.setLength(100);
  // Connector_Id_FK
  Field Connector_Id_FK;
  Connector_Id_FK.setName("Connector_Id_FK");
  Connector_Id_FK.setType(FieldType::Integer);
  Connector_Id_FK.setRequired(true);
  // Init Connector_tbl
  Table Connector_tbl;
  Connector_tbl.setTableName("Connector_tbl");
  Connector_tbl.setPrimaryKey(Id_PK);
  // fk_Connector_Id_FK
  ForeignKey fk_Connector_Id_FK;
  fk_Connector_Id_FK.setParentTable(Connector_tbl);
  fk_Connector_Id_FK.addKeyFields(ParentTableFieldName(Id_PK), ChildTableFieldName(Connector_Id_FK));
  fk_Connector_Id_FK.setOnDeleteAction(ForeignKey::Restrict);
  fk_Connector_Id_FK.setOnUpdateAction(ForeignKey::Cascade);
  // Init Type_tbl
  Table Type_tbl;
  Type_tbl.setTableName("Type_tbl");
  Type_tbl.setPrimaryKey(Id_PK);
  // Type_Id_FK
  Field Type_Id_FK;
  Type_Id_FK.setName("Type_Id_FK");
  Type_Id_FK.setType(FieldType::Integer);
  Type_Id_FK.setRequired(true);
  // fk_Type_Id_FK
  ForeignKey fk_Type_Id_FK;
  fk_Type_Id_FK.setParentTable(Type_tbl);
  fk_Type_Id_FK.addKeyFields(ParentTableFieldName(Id_PK), ChildTableFieldName(Type_Id_FK));
  fk_Type_Id_FK.setOnDeleteAction(ForeignKey::Restrict);
  fk_Type_Id_FK.setOnUpdateAction(ForeignKey::Cascade);

  /*
   * Check SQL to create table:
   *  - Auto increment primary key
   *  - 0 other field
   */
  // Setup table
  table.clear();
  table.setTableName("Client_tbl");
  table.setPrimaryKey(Id_PK);
  // Check
  expectedSql  = "CREATE TABLE \"Client_tbl\" (\n";
  expectedSql += "  \"Id_PK\" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT\n";
  expectedSql += ");\n";
  QCOMPARE(driver.getSqlToCreateTable(table), expectedSql);
  /*
   * Check SQL to create table:
   *  - Auto increment primary key
   *  - 1 other field
   */
  // Setup table
  table.clear();
  table.setTableName("Client_tbl");
  table.setPrimaryKey(Id_PK);
  table.addField(Name);
  // Check
  expectedSql  = "CREATE TABLE \"Client_tbl\" (\n";
  expectedSql += "  \"Id_PK\" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\n";
  expectedSql += "  \"Name\" VARCHAR(50) DEFAULT NULL\n";
  expectedSql += ");\n";
  QCOMPARE(driver.getSqlToCreateTable(table), expectedSql);
  /*
   * Check SQL to create table:
   *  - Auto increment primary key
   *  - 2 other fields
   */
  // Setup table
  table.clear();
  table.setTableName("Client_tbl");
  table.setPrimaryKey(Id_PK);
  table.addField(Name);
  table.addField(Remarks);
  // Check
  expectedSql  = "CREATE TABLE \"Client_tbl\" (\n";
  expectedSql += "  \"Id_PK\" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\n";
  expectedSql += "  \"Name\" VARCHAR(50) DEFAULT NULL,\n";
  expectedSql += "  \"Remarks\" VARCHAR(100) DEFAULT NULL\n";
  expectedSql += ");\n";
  QCOMPARE(driver.getSqlToCreateTable(table), expectedSql);
  /*
   * Check SQL to create table:
   *  - Single field primary key
   *  - 0 other field
   */
  // Setup table
  table.clear();
  table.setTableName("Client_tbl");
  table.setPrimaryKey(Code_PK);
  // Check
  expectedSql  = "CREATE TABLE \"Client_tbl\" (\n";
  expectedSql += "  \"Code_PK\" VARCHAR(20) NOT NULL PRIMARY KEY\n";
  expectedSql += ");\n";
  QCOMPARE(driver.getSqlToCreateTable(table), expectedSql);
  /*
   * Check SQL to create table:
   *  - Single field primary key
   *  - 1 other field
   */
  // Setup table
  table.clear();
  table.setTableName("Client_tbl");
  table.setPrimaryKey(Code_PK);
  table.addField(Name);
  // Check
  expectedSql  = "CREATE TABLE \"Client_tbl\" (\n";
  expectedSql += "  \"Code_PK\" VARCHAR(20) NOT NULL PRIMARY KEY,\n";
  expectedSql += "  \"Name\" VARCHAR(50) DEFAULT NULL\n";
  expectedSql += ");\n";
  QCOMPARE(driver.getSqlToCreateTable(table), expectedSql);
  /*
   * Check SQL to create table:
   *  - Only 1 field (No primary key)
   */
  // Setup table
  table.clear();
  table.setTableName("Client_tbl");
  table.addField(Name);
  // Check
  expectedSql  = "CREATE TABLE \"Client_tbl\" (\n";
  expectedSql += "  \"Name\" VARCHAR(50) DEFAULT NULL\n";
  expectedSql += ");\n";
  QCOMPARE(driver.getSqlToCreateTable(table), expectedSql);
  /*
   * Check SQL to create table:
   *  - Composed primary key
   *  - 0 other field
   */
  // Setup table
  table.clear();
  table.setTableName("Client_tbl");
  table.addField(Id_A);
  table.addField(Id_B);
  table.setPrimaryKey(Id_AB_PK);
  // Check
  expectedSql  = "CREATE TABLE \"Client_tbl\" (\n";
  expectedSql += "  \"Id_A\" INTEGER NOT NULL DEFAULT NULL,\n";
  expectedSql += "  \"Id_B\" INTEGER NOT NULL DEFAULT NULL,\n";
  expectedSql += "  PRIMARY KEY (\"Id_A\",\"Id_B\")\n";
  expectedSql += ");\n";
  QCOMPARE(driver.getSqlToCreateTable(table), expectedSql);
  /*
   * Check SQL to create table:
   *  - Composed primary key
   *  - 1 other field
   */
  // Setup table
  table.clear();
  table.setTableName("Client_tbl");
  table.addField(Id_A);
  table.addField(Id_B);
  table.addField(Name);
  table.setPrimaryKey(Id_AB_PK);
  // Check
  expectedSql  = "CREATE TABLE \"Client_tbl\" (\n";
  expectedSql += "  \"Id_A\" INTEGER NOT NULL DEFAULT NULL,\n";
  expectedSql += "  \"Id_B\" INTEGER NOT NULL DEFAULT NULL,\n";
  expectedSql += "  \"Name\" VARCHAR(50) DEFAULT NULL,\n";
  expectedSql += "  PRIMARY KEY (\"Id_A\",\"Id_B\")\n";
  expectedSql += ");\n";
  QCOMPARE(driver.getSqlToCreateTable(table), expectedSql);
  /*
   * Check SQL to create table:
   *  - Auto increment primary key
   *  - 1 other field
   *  - 1 foreign key
   */
  // Setup table
  table.clear();
  table.setTableName("Contact_tbl");
  table.setPrimaryKey(Id_PK);
  table.addField(Connector_Id_FK);
  table.addForeignKey(fk_Connector_Id_FK);
  // Check
  expectedSql  = "CREATE TABLE \"Contact_tbl\" (\n";
  expectedSql += "  \"Id_PK\" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\n";
  expectedSql += "  \"Connector_Id_FK\" INTEGER NOT NULL DEFAULT NULL,\n";
  expectedSql += "  FOREIGN KEY (\"Connector_Id_FK\")\n";
  expectedSql += "   REFERENCES \"Connector_tbl\" (\"Id_PK\")\n";
  expectedSql += "   ON DELETE RESTRICT\n";
  expectedSql += "   ON UPDATE CASCADE\n";
  expectedSql += ");\n";
  QCOMPARE(driver.getSqlToCreateTable(table), expectedSql);
  /*
   * Check SQL to create table:
   *  - Composed primary key
   *  - 1 other field
   *  - 1 foreign key
   */
  // Setup table
  table.clear();
  table.setTableName("Contact_tbl");
  table.addField(Id_A);
  table.addField(Id_B);
  table.addField(Connector_Id_FK);
  table.setPrimaryKey(Id_AB_PK);
  table.addForeignKey(fk_Connector_Id_FK);
  // Check
  expectedSql  = "CREATE TABLE \"Contact_tbl\" (\n";
  expectedSql += "  \"Id_A\" INTEGER NOT NULL DEFAULT NULL,\n";
  expectedSql += "  \"Id_B\" INTEGER NOT NULL DEFAULT NULL,\n";
  expectedSql += "  \"Connector_Id_FK\" INTEGER NOT NULL DEFAULT NULL,\n";
  expectedSql += "  PRIMARY KEY (\"Id_A\",\"Id_B\"),\n";
  expectedSql += "  FOREIGN KEY (\"Connector_Id_FK\")\n";
  expectedSql += "   REFERENCES \"Connector_tbl\" (\"Id_PK\")\n";
  expectedSql += "   ON DELETE RESTRICT\n";
  expectedSql += "   ON UPDATE CASCADE\n";
  expectedSql += ");\n";
  QCOMPARE(driver.getSqlToCreateTable(table), expectedSql);
  /*
   * Check SQL to create table:
   *  - Auto increment primary key
   *  - 2 other field
   *  - 2 foreign key
   */
  // Setup table
  table.clear();
  table.setTableName("Contact_tbl");
  table.setPrimaryKey(Id_PK);
  table.addField(Connector_Id_FK);
  table.addField(Type_Id_FK);
  table.addForeignKey(fk_Connector_Id_FK);
  table.addForeignKey(fk_Type_Id_FK);
  // Check
  expectedSql  = "CREATE TABLE \"Contact_tbl\" (\n";
  expectedSql += "  \"Id_PK\" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\n";
  expectedSql += "  \"Connector_Id_FK\" INTEGER NOT NULL DEFAULT NULL,\n";
  expectedSql += "  \"Type_Id_FK\" INTEGER NOT NULL DEFAULT NULL,\n";
  expectedSql += "  FOREIGN KEY (\"Connector_Id_FK\")\n";
  expectedSql += "   REFERENCES \"Connector_tbl\" (\"Id_PK\")\n";
  expectedSql += "   ON DELETE RESTRICT\n";
  expectedSql += "   ON UPDATE CASCADE,\n";
  expectedSql += "  FOREIGN KEY (\"Type_Id_FK\")\n";
  expectedSql += "   REFERENCES \"Type_tbl\" (\"Id_PK\")\n";
  expectedSql += "   ON DELETE RESTRICT\n";
  expectedSql += "   ON UPDATE CASCADE\n";
  expectedSql += ");\n";
  QCOMPARE(driver.getSqlToCreateTable(table), expectedSql);

  /*
   * Check SQL to drop table
   */
  table.clear();
  table.setTableName("Client_tbl");
  expectedSql = "DROP TABLE IF EXISTS \"Client_tbl\";\n";
  QCOMPARE(driver.getSqlToDropTable(table), expectedSql);
}

void SchemaDriverSqliteTest::simpleCreateAndDropTableTest()
{
  using Mdt::Sql::Schema::Table;
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::ForeignKey;

  Mdt::Sql::Schema::Driver driver(mDatabase);
  QVERIFY(driver.isValid());
  Schema::Client_tbl client_tbl;

  /*
   * Here we simply check that we can create and drop a table.
   * More checks are done in next tests
   */
  // Check versions that takes a Table type
  QVERIFY(!mDatabase.tables().contains(client_tbl.tableName()));
  QVERIFY(driver.createTable(client_tbl.toTable()));
  QVERIFY(mDatabase.tables().contains(client_tbl.tableName()));
  QVERIFY(driver.dropTable(client_tbl.toTable()));
  QVERIFY(!mDatabase.tables().contains(client_tbl.tableName()));
  // Check versions that takes a TableTemplate type
  QVERIFY(driver.createTable(client_tbl));
  QVERIFY(mDatabase.tables().contains(client_tbl.tableName()));
  QVERIFY(driver.dropTable(client_tbl));
  QVERIFY(!mDatabase.tables().contains(client_tbl.tableName()));
}

void SchemaDriverSqliteTest::reverseFieldListTest()
{
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::Field;
  using Mdt::Sql::Schema::FieldList;
  using Mdt::Sql::Schema::Table;
  using Mdt::Sql::Schema::AutoIncrementPrimaryKey;
  using Mdt::Sql::Schema::PrimaryKeyContainer;
  using Mdt::Sql::Schema::CaseSensitivity;

  Mdt::Sql::Schema::DriverSQLite driver(mDatabase);
  Field field;
  FieldList fieldList;

  /*
   * Setup fields
   */
  // Id_PK
  AutoIncrementPrimaryKey Id_PK;
  Id_PK.setFieldName("Id_PK");
  // Name
  Field Name;
  Name.setName("Name");
  Name.setType(FieldType::Varchar);
  Name.setLength(150);
  Name.setDefaultValue("Default name");
  Name.setRequired(true);
  Name.setUnique(true);
  Name.setCaseSensitive(false);
  // Remarks
  Field Remarks;
  Remarks.setName("Remarks");
  Remarks.setType(FieldType::Varchar);
  Remarks.setLength(200);
  /*
   * Setup table
   */
  Table Connector_tbl;
  Connector_tbl.setTableName("Connector_tbl");
  Connector_tbl.setPrimaryKey(Id_PK);
  Connector_tbl.addField(Name);
  Connector_tbl.addField(Remarks);
  /*
   * Create table in database
   */
  QVERIFY(driver.createTable(Connector_tbl));
  /*
   * Check
   *
   * Note: for primary key, the unique flag is not checked here.
   *       The unique flag is important for other fields in Table,
   *       but implicit for primary keys, and returning true or false
   *       is a problem that must be solved in Table, not in reversing
   *       from database.
   *
   * Note: it seems that SQLite provides no way to get collation back
   *       from database schema, except for fields that are indexed.
   *       For now, we also not check collation reversing for fields
   *       that are not indexed.
   */
  auto ret = driver.getTableFieldListFromDatabase("Connector_tbl");
  QVERIFY(ret);
  fieldList = ret.value();
  QCOMPARE(fieldList.size(), 3);
  // Id_PK
  field = fieldList.at(0);
  QCOMPARE(field.name(), QString("Id_PK"));
  QVERIFY(field.type() == FieldType::Integer);
  QCOMPARE(field.length(), -1);
  QVERIFY(field.defaultValue().isNull());
  QVERIFY(field.isRequired());
  QVERIFY(field.collation().isNull());
  QVERIFY(Connector_tbl.primaryKeyType() == PrimaryKeyContainer::AutoIncrementPrimaryKeyType);
  QCOMPARE(Connector_tbl.autoIncrementPrimaryKey().fieldName(), QString("Id_PK"));
  // Name
  field = fieldList.at(1);
  QCOMPARE(field.name(), QString("Name"));
  QVERIFY(field.type() == FieldType::Varchar);
  QCOMPARE(field.length(), 150);
  QCOMPARE(field.defaultValue(), QVariant("Default name"));
  QVERIFY(field.isRequired());
  QVERIFY(field.isUnique());
  QVERIFY(field.collation().caseSensitivity() == CaseSensitivity::CaseInsensitive);
  // Remarks
  field = fieldList.at(2);
  QCOMPARE(field.name(), QString("Remarks"));
  QVERIFY(field.type() == FieldType::Varchar);
  QCOMPARE(field.length(), 200);
  QVERIFY(field.defaultValue().isNull());
  QVERIFY(!field.isRequired());
  QVERIFY(!field.isUnique());
  QVERIFY(field.collation().isNull());
}

void SchemaDriverSqliteTest::reverseIndexListTest()
{
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::Field;
  using Mdt::Sql::Schema::FieldList;
  using Mdt::Sql::Schema::Table;
  using Mdt::Sql::Schema::AutoIncrementPrimaryKey;
  using Mdt::Sql::Schema::Index;
  using Mdt::Sql::Schema::IndexList;

  Mdt::Sql::Schema::DriverSQLite driver(mDatabase);
  Field field;
  FieldList fieldList;
  IndexList indexList;

  /*
   * Setup fields
   */
  // Id_PK
  AutoIncrementPrimaryKey Id_PK;
  Id_PK.setFieldName("Id_PK");
  // Name - Will generate a index for the unique constraint (on most databases)
  Field Name;
  Name.setName("Name");
  Name.setType(FieldType::Varchar);
  Name.setLength(150);
  Name.setRequired(true);
  Name.setUnique(true);
  // A
  Field A;
  A.setName("A");
  A.setType(FieldType::Varchar);
  A.setLength(50);
  A.setRequired(true);
  // B
  Field B;
  B.setName("B");
  B.setType(FieldType::Varchar);
  B.setLength(50);
  B.setRequired(true);
  /*
   * Setup table
   */
  Table Connector_tbl;
  Connector_tbl.setTableName("Connector_tbl");
  Connector_tbl.setPrimaryKey(Id_PK);
  Connector_tbl.addField(Name);
  Connector_tbl.addField(A);
  Connector_tbl.addField(B);
  /*
   * Setup indexes
   */
  Index A_B_idx;
  A_B_idx.setName("A_B_idx");
  A_B_idx.setTable(Connector_tbl);
  A_B_idx.addField(A);
  A_B_idx.addField(B);
  Connector_tbl.addIndex(A_B_idx);

//   QVERIFY(driver.dropTable(Connector_tbl));
  /*
   * Create table in database
   */
  QVERIFY(driver.createTable(Connector_tbl));
  /*
   * Get index list and check
   */
  auto ret = driver.getTableIndexListFromDatabase(Connector_tbl.tableName());
  QVERIFY(ret);
  indexList = ret.value();
  // Check
  QCOMPARE(indexList.size(), 1);
  QCOMPARE(indexList.at(0).name(), A_B_idx.name());
  QCOMPARE(indexList.at(0).tableName(), A_B_idx.tableName());
  QVERIFY(!indexList.at(0).isUnique());
  QCOMPARE(indexList.at(0).fieldCount(), 2);
  QCOMPARE(indexList.at(0).fieldName(0), A.name());
  QCOMPARE(indexList.at(0).fieldName(1), B.name());
  /*
   * Cleanup
   */
  QVERIFY(driver.dropTable(Connector_tbl));
  ret = driver.getTableIndexListFromDatabase(Connector_tbl.tableName());
  QVERIFY(ret);
  indexList = ret.value();
  QCOMPARE(indexList.size(), 0);
}

void SchemaDriverSqliteTest::reversePrimaryKeyTest()
{
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::Field;
  using Mdt::Sql::Schema::Table;
  using Mdt::Sql::Schema::AutoIncrementPrimaryKey;
  using Mdt::Sql::Schema::SingleFieldPrimaryKey;
  using Mdt::Sql::Schema::PrimaryKey;
  using Mdt::Sql::Schema::PrimaryKeyContainer;

  Mdt::Sql::Schema::DriverSQLite driver(mDatabase);
  Table table;
  Mdt::Expected<PrimaryKeyContainer> ret;
  PrimaryKeyContainer pk;

  /*
   * Setup primary keys with field definitions
   */
  // Id_PK
  AutoIncrementPrimaryKey Id_PK;
  Id_PK.setFieldName("Id_PK");
  // Code_PK
  SingleFieldPrimaryKey Code_PK;
  Code_PK.setFieldName("Code_PK");
  Code_PK.setFieldType(FieldType::Varchar); // Must not be Integer, else it will become a auto increment PK in SQLite
  Code_PK.setFieldLength(50);
  /*
   * Setup fields
   */
  // Id_A
  Field Id_A;
  Id_A.setName("Id_A");
  Id_A.setType(FieldType::Integer);
  Id_A.setRequired(true);
  // Id_B
  Field Id_B;
  Id_B.setName("Id_B");
  Id_B.setType(FieldType::Integer);
  Id_B.setRequired(true);
  // Name
  Field Name;
  Name.setName("Name");
  Name.setType(FieldType::Varchar);
  Name.setLength(50);
  Name.setUnique(true);
  /*
   * Setup primary keys that only refers to existing fields
   */
  // Id_AB_PK
  PrimaryKey Id_AB_PK;
  Id_AB_PK.addField(Id_A);
  Id_AB_PK.addField(Id_B);

  /*
   * Check with a auto increment primary key
   */
  // Setup and create table
  table.clear();
  table.setTableName("Connector_tbl");
  table.setPrimaryKey(Id_PK);
  table.addField(Name);
  QVERIFY(driver.dropTable(table));
  QVERIFY(driver.createTable(table));
  // Get primary key from database
  ret = driver.getTablePrimaryKeyFromDatabase(table.tableName());
  QVERIFY(ret);
  pk = ret.value();
  // Check
  QVERIFY(pk.primaryKeyType() == PrimaryKeyContainer::AutoIncrementPrimaryKeyType);
  QCOMPARE(pk.autoIncrementPrimaryKey().fieldName(), QString("Id_PK"));
  // Drop table
  QVERIFY(driver.dropTable(table));
  /*
   * Check with a single field primary key
   *  -> Primary key defined as COLUMN constraint
   */
  // Setup and create table
  table.clear();
  table.setTableName("Connector_tbl");
  table.setPrimaryKey(Code_PK);
  table.addField(Name);
  QVERIFY(driver.createTable(table));
  // Get primary key from database
  ret = driver.getTablePrimaryKeyFromDatabase(table.tableName());
  QVERIFY(ret);
  pk = ret.value();
  // Check
  QVERIFY(pk.primaryKeyType() == PrimaryKeyContainer::SingleFieldPrimaryKeyType);
  QCOMPARE(pk.singleFieldPrimaryKey().fieldName(), QString("Code_PK"));
  QVERIFY(pk.singleFieldPrimaryKey().fieldType() == FieldType::Varchar);
  QCOMPARE(pk.singleFieldPrimaryKey().fieldLength(), 50);
  /// \todo Collation once defined
  // Drop table
  QVERIFY(driver.dropTable(table));
  /*
   * Check with a multiple fields primary key
   *  -> Primary key defined as TABLE constraint
   */
  // Setup and create table
  table.clear();
  table.setTableName("Connector_tbl");
  table.addField(Id_A);
  table.addField(Id_B);
  table.addField(Name);
  table.setPrimaryKey(Id_AB_PK);
  QVERIFY(driver.createTable(table));
  // Get primary key from database
  ret = driver.getTablePrimaryKeyFromDatabase(table.tableName());
  QVERIFY(ret);
  pk = ret.value();
  // Check
  QVERIFY(pk.primaryKeyType() == PrimaryKeyContainer::PrimaryKeyType);
  QCOMPARE(pk.primaryKey().fieldCount(), 2);
  QCOMPARE(pk.primaryKey().fieldNameList().at(0), QString("Id_A"));
  QCOMPARE(pk.primaryKey().fieldNameList().at(1), QString("Id_B"));
  // Drop table
  QVERIFY(driver.dropTable(table));
}

void SchemaDriverSqliteTest::reverseForeignKeyTest()
{
  using Mdt::Sql::Schema::FieldType;
  using Mdt::Sql::Schema::Field;
  using Mdt::Sql::Schema::Table;
  using Mdt::Sql::Schema::AutoIncrementPrimaryKey;
  using Mdt::Sql::Schema::SingleFieldPrimaryKey;
  using Mdt::Sql::Schema::PrimaryKey;
  using Mdt::Sql::Schema::ParentTableFieldName;
  using Mdt::Sql::Schema::ChildTableFieldName;
  using Mdt::Sql::Schema::ForeignKey;
  using Mdt::Sql::Schema::ForeignKeyList;

  Mdt::Sql::Schema::DriverSQLite driver(mDatabase);
  Table table;
  ForeignKey fk;
  Mdt::Expected<ForeignKeyList> ret;
  ForeignKeyList fkList;

  /*
   * Setup fields
   */
  // Id_A
  Field Id_A;
  Id_A.setName("Id_A");
  Id_A.setType(FieldType::Integer);
  Id_A.setRequired(true);
  // Id_B
  Field Id_B;
  Id_B.setName("Id_B");
  Id_B.setType(FieldType::Integer);
  Id_B.setRequired(true);
  // Id_A_FK
  Field Id_A_FK;
  Id_A_FK.setName("Id_A_FK");
  Id_A_FK.setType(FieldType::Integer);
  Id_A_FK.setRequired(true);
  // Id_B_FK
  Field Id_B_FK;
  Id_B_FK.setName("Id_B_FK");
  Id_B_FK.setType(FieldType::Integer);
  Id_B_FK.setRequired(true);
  // Name
  Field Name;
  Name.setName("Name");
  Name.setType(FieldType::Varchar);
  Name.setLength(50);
  Name.setUnique(true);
  // Code_FK
  Field Code_FK;
  Code_FK.setName("Code_FK");
  Code_FK.setType(FieldType::Varchar);
  Code_FK.setLength(50);
  Code_FK.setRequired(true);
  /*
   * Setup primary keys
   */
  // Id_PK
  AutoIncrementPrimaryKey Id_PK;
  Id_PK.setFieldName("Id_PK");
  // Code_PK
  SingleFieldPrimaryKey Code_PK;
  Code_PK.setFieldName("Code_PK");
  Code_PK.setFieldType(FieldType::Varchar);
  Code_PK.setFieldLength(50);
  // Id_AB_PK
  PrimaryKey Id_AB_PK;
  Id_AB_PK.addField(Id_A);
  Id_AB_PK.addField(Id_B);
  /*
   * Setup tables that will be reused
   */
  // Type_tbl
  Table Type_tbl;
  Type_tbl.setTableName("Type_tbl");
  Type_tbl.setPrimaryKey(Code_PK);
  Type_tbl.addField(Name);
  // Connector_tbl
  Table Connector_tbl;
  Connector_tbl.setTableName("Connector_tbl");
  Connector_tbl.addField(Id_A);
  Connector_tbl.addField(Id_B);
  Connector_tbl.addField(Name);
  Connector_tbl.setPrimaryKey(Id_AB_PK);
  /*
   * Cleanup and create tables that will be reused
   */
  QVERIFY(driver.dropTable(Type_tbl));
  QVERIFY(driver.dropTable(Connector_tbl));
  QVERIFY(driver.createTable(Type_tbl));
  QVERIFY(driver.createTable(Connector_tbl));

  /*
   * Table with 1 single column foreign key
   */
  // Setup and create table
  table.clear();
  table.setTableName("Contact_tbl");
  table.setPrimaryKey(Id_PK);
  table.addField(Code_FK);
  fk.clear();
  fk.setParentTable(Type_tbl);
  fk.setChildTable(table);
  fk.setOnDeleteAction(ForeignKey::Restrict);
  fk.setOnUpdateAction(ForeignKey::Cascade);
  fk.addKeyFields(ParentTableFieldName(Code_PK), ChildTableFieldName(Code_FK));
  table.addForeignKey(fk);
  QVERIFY(driver.dropTable(table));
  QVERIFY(driver.createTable(table));
  // Check
  ret = driver.getTableForeignKeyListFromDatabase(table.tableName());
  QVERIFY(ret);
  fkList = ret.value();
  QCOMPARE(fkList.size(), 1);
  fk = fkList.at(0);
  QCOMPARE(fk.parentTableName(), QString("Type_tbl"));
  QCOMPARE(fk.childTableName(), QString("Contact_tbl"));
  QVERIFY(fk.onDeleteAction() == ForeignKey::Restrict);
  QVERIFY(fk.onUpdateAction() == ForeignKey::Cascade);
  QCOMPARE(fk.parentTableFieldNameList().size(), 1);
  QCOMPARE(fk.parentTableFieldNameList().at(0), QString("Code_PK"));
  QCOMPARE(fk.childTableFieldNameList().size(), 1);
  QCOMPARE(fk.childTableFieldNameList().at(0), QString("Code_FK"));
  // Drop table
  QVERIFY(driver.dropTable(table));
  /*
   * Table with 1 multi column foreign key
   */
  // Setup and create table
  table.clear();
  table.setTableName("Contact_tbl");
  table.setPrimaryKey(Id_PK);
  table.addField(Id_A_FK);
  table.addField(Id_B_FK);
  fk.clear();
  fk.setParentTable(Connector_tbl);
  fk.setChildTable(table);
  fk.setOnDeleteAction(ForeignKey::Restrict);
  fk.setOnUpdateAction(ForeignKey::Cascade);
  fk.addKeyFields(ParentTableFieldName(Id_A), ChildTableFieldName(Id_A_FK));
  fk.addKeyFields(ParentTableFieldName(Id_B), ChildTableFieldName(Id_B_FK));
  table.addForeignKey(fk);
  QVERIFY(driver.createTable(table));
  // Check
  ret = driver.getTableForeignKeyListFromDatabase(table.tableName());
  QVERIFY(ret);
  fkList = ret.value();
  QCOMPARE(fkList.size(), 1);
  fk = fkList.at(0);
  QCOMPARE(fk.parentTableName(), QString("Connector_tbl"));
  QCOMPARE(fk.childTableName(), QString("Contact_tbl"));
  QVERIFY(fk.onDeleteAction() == ForeignKey::Restrict);
  QVERIFY(fk.onUpdateAction() == ForeignKey::Cascade);
  QCOMPARE(fk.parentTableFieldNameList().size(), 2);
  QCOMPARE(fk.parentTableFieldNameList().at(0), QString("Id_A"));
  QCOMPARE(fk.parentTableFieldNameList().at(1), QString("Id_B"));
  QCOMPARE(fk.childTableFieldNameList().size(), 2);
  QCOMPARE(fk.childTableFieldNameList().at(0), QString("Id_A_FK"));
  QCOMPARE(fk.childTableFieldNameList().at(1), QString("Id_B_FK"));
  // Drop table
  QVERIFY(driver.dropTable(table));
  /*
   * Table with 1 single column and 1 multi column foreign keys
   */
  // Setup and create table
  table.clear();
  table.setTableName("Contact_tbl");
  table.setPrimaryKey(Id_PK);
  table.addField(Code_FK);
  table.addField(Id_A_FK);
  table.addField(Id_B_FK);
  fk.clear();
  fk.setParentTable(Type_tbl);
  fk.setChildTable(table);
  fk.setOnDeleteAction(ForeignKey::Restrict);
  fk.setOnUpdateAction(ForeignKey::Cascade);
  fk.addKeyFields(ParentTableFieldName(Code_PK), ChildTableFieldName(Code_FK));
  table.addForeignKey(fk);
  fk.clear();
  fk.setParentTable(Connector_tbl);
  fk.setChildTable(table);
  fk.setOnDeleteAction(ForeignKey::Restrict);
  fk.setOnUpdateAction(ForeignKey::Cascade);
  fk.addKeyFields(ParentTableFieldName(Id_A), ChildTableFieldName(Id_A_FK));
  fk.addKeyFields(ParentTableFieldName(Id_B), ChildTableFieldName(Id_B_FK));
  table.addForeignKey(fk);
  QVERIFY(driver.createTable(table));
  // Check
  ret = driver.getTableForeignKeyListFromDatabase(table.tableName());
  QVERIFY(ret);
  fkList = ret.value();
  QCOMPARE(fkList.size(), 2);
  // It seems that SQLite returns FK list in revert order of declaration
  fk = fkList.at(1);
  QCOMPARE(fk.parentTableName(), QString("Type_tbl"));
  QCOMPARE(fk.childTableName(), QString("Contact_tbl"));
  QVERIFY(fk.onDeleteAction() == ForeignKey::Restrict);
  QVERIFY(fk.onUpdateAction() == ForeignKey::Cascade);
  QCOMPARE(fk.parentTableFieldNameList().size(), 1);
  QCOMPARE(fk.parentTableFieldNameList().at(0), QString("Code_PK"));
  QCOMPARE(fk.childTableFieldNameList().size(), 1);
  QCOMPARE(fk.childTableFieldNameList().at(0), QString("Code_FK"));
  fk = fkList.at(0);
  QCOMPARE(fk.parentTableName(), QString("Connector_tbl"));
  QCOMPARE(fk.childTableName(), QString("Contact_tbl"));
  QVERIFY(fk.onDeleteAction() == ForeignKey::Restrict);
  QVERIFY(fk.onUpdateAction() == ForeignKey::Cascade);
  QCOMPARE(fk.parentTableFieldNameList().size(), 2);
  QCOMPARE(fk.parentTableFieldNameList().at(0), QString("Id_A"));
  QCOMPARE(fk.parentTableFieldNameList().at(1), QString("Id_B"));
  QCOMPARE(fk.childTableFieldNameList().size(), 2);
  QCOMPARE(fk.childTableFieldNameList().at(0), QString("Id_A_FK"));
  QCOMPARE(fk.childTableFieldNameList().at(1), QString("Id_B_FK"));
  // Drop table
  QVERIFY(driver.dropTable(table));
}

void SchemaDriverSqliteTest::simpleCreateAndDropViewTest()
{
  Mdt::Sql::Schema::Driver driver(mDatabase);
  QVERIFY(driver.isValid());
  Schema::ClientAdrressView view;
  Schema::Client_tbl client;
  Schema::Address_tbl address;

  // Create tables
  QVERIFY(driver.createTable(client));
  QVERIFY(driver.createTable(address));
  // Check version that takes a View
  QVERIFY(!mDatabase.tables(QSql::AllTables).contains(view.name()));
  QVERIFY(driver.createView(view.toView()));
  QVERIFY(mDatabase.tables(QSql::AllTables).contains(view.name()));
  QVERIFY(driver.dropView(view.toView()));
  QVERIFY(!mDatabase.tables(QSql::AllTables).contains(view.name()));
  // Check version that takes a ViewTemplate
  QVERIFY(driver.createView(view));
  QVERIFY(mDatabase.tables(QSql::AllTables).contains(view.name()));
  QVERIFY(driver.dropView(view));
  QVERIFY(!mDatabase.tables(QSql::AllTables).contains(view.name()));
  // Cleanup
  QVERIFY(driver.dropTable(client));
  QVERIFY(driver.dropTable(address));
}

void SchemaDriverSqliteTest::triggerDefinitionTest()
{
  using Mdt::Sql::Schema::Trigger;

  Mdt::Sql::Schema::DriverSQLite driver(mDatabase);
  QString expectedSql;
  Trigger trigger;

  /*
   * Simple trigger
   */
  // Setup trigger
  trigger.setTemporary(true);
  trigger.setName("TestTrigger");
  trigger.setEvent(Trigger::AfterInsert);
  trigger.setTable(Schema::Client_tbl());
  trigger.setScript(" UPDATE Client_tbl SET Name = 'Some name';");
  // Check SQL for DROP
  expectedSql = "DROP TRIGGER IF EXISTS \"TestTrigger\";";
  QCOMPARE(driver.getSqlToDropTrigger(trigger), expectedSql);
  // Check SQL for CREATE
  expectedSql = "CREATE TEMPORARY TRIGGER \"TestTrigger\" AFTER INSERT ON \"Client_tbl\"\n" \
                "FOR EACH ROW\n" \
                "BEGIN\n" \
                " UPDATE Client_tbl SET Name = 'Some name';\n" \
                "END;";
  QCOMPARE(driver.getSqlToCreateTrigger(trigger), expectedSql);
}

void SchemaDriverSqliteTest::simpleTriggerCreateDropTest()
{
  using Mdt::Sql::Schema::Trigger;

  Mdt::Sql::Schema::DriverSQLite driver(mDatabase);
  Trigger trigger;
  Schema::Client_tbl client;
  QSqlQuery query(mDatabase);

  /*
   * Setup trigger
   */
  trigger.setName("TestTrigger");
  trigger.setEvent(Trigger::AfterInsert);
  trigger.setTable(client);
  trigger.setScript(" UPDATE Client_tbl SET FieldA = 'Some A';");
  /*
   * Create table en trigger
   */
  QVERIFY(driver.createTable(client.toTable()));
  QVERIFY(driver.createTrigger(trigger));
  /*
   * Insert some data
   */
  QVERIFY(query.exec("INSERT INTO Client_tbl (Name) VALUES ('Name 1')"));
  QVERIFY(query.exec("SELECT Name, FieldA FROM Client_tbl"));
  QVERIFY(query.next());
  QCOMPARE(query.value(0), QVariant("Name 1"));
  QCOMPARE(query.value(1), QVariant("Some A"));
  QVERIFY(query.exec("DELETE FROM Client_tbl"));
  /*
   * Drop trigger and check
   */
  QVERIFY(driver.dropTrigger(trigger));
  QVERIFY(query.exec("INSERT INTO Client_tbl (Name) VALUES ('Name 1')"));
  QVERIFY(query.exec("SELECT Name, FieldA FROM Client_tbl"));
  QVERIFY(query.next());
  QCOMPARE(query.value(0), QVariant("Name 1"));
  QVERIFY(query.value(1).isNull());
  /*
   * Cleanup
   */
  query.clear();
  QVERIFY(driver.dropTable(client.toTable()));
}

void SchemaDriverSqliteTest::tablePopulationTest()
{
  using Mdt::Sql::Schema::TablePopulation;

  Mdt::Sql::Schema::DriverSQLite driver(mDatabase);
  Schema::Client_tbl client;
  QSqlQuery query(mDatabase);
  TablePopulation tp;

  /*
   * Setup table population
   */
  tp.setTable(client);
  tp.addField(client.Id_PK());
  tp.addField(client.Name());
  tp << 1 << "Name 1";
  tp.commitCurrentRow();
  tp << 2 << "Name 2";
  tp.commitCurrentRow();
  /*
   * Create Client_tbl
   */
  QVERIFY(driver.dropTable(client.toTable()));
  QVERIFY(driver.createTable(client.toTable()));
  /*
   * Check
   */
  QVERIFY(query.exec("SELECT Id_PK, Name FROM Client_tbl"));
  QVERIFY(!query.next());
  QVERIFY(driver.populateTable(tp));
  QVERIFY(query.exec("SELECT Id_PK, Name FROM Client_tbl"));
  QVERIFY(query.next());
  QCOMPARE(query.value(0), QVariant(1));
  QCOMPARE(query.value(1), QVariant("Name 1"));
  QVERIFY(query.next());
  QCOMPARE(query.value(0), QVariant(2));
  QCOMPARE(query.value(1), QVariant("Name 2"));
  /*
   * Cleanup
   */
  query.clear();
  QVERIFY(driver.dropTable(client.toTable()));
}

void SchemaDriverSqliteTest::simpleCreateAndDropSchemaTest()
{
  Mdt::Sql::Schema::Driver driver(mDatabase);
  QVERIFY(driver.isValid());
  Schema::TestSchema schema;

  // Check with version that takes a Schema
  QVERIFY(!mDatabase.tables(QSql::AllTables).contains("Client_tbl"));
  QVERIFY(!mDatabase.tables(QSql::AllTables).contains("Address_tbl"));
  QVERIFY(!mDatabase.tables(QSql::AllTables).contains("ClientAddress_view"));
  QVERIFY(driver.createSchema(schema.toSchema()));
  QVERIFY(mDatabase.tables(QSql::AllTables).contains("Client_tbl"));
  QVERIFY(mDatabase.tables(QSql::AllTables).contains("Address_tbl"));
  QVERIFY(mDatabase.tables(QSql::AllTables).contains("ClientAddress_view"));
  QVERIFY(driver.dropSchema(schema.toSchema()));
  QVERIFY(!mDatabase.tables(QSql::AllTables).contains("Client_tbl"));
  QVERIFY(!mDatabase.tables(QSql::AllTables).contains("Address_tbl"));
  QVERIFY(!mDatabase.tables(QSql::AllTables).contains("ClientAddress_view"));
  // Check with version that takes a SchemaTemplate
  QVERIFY(driver.createSchema(schema));
  QVERIFY(mDatabase.tables(QSql::AllTables).contains("Client_tbl"));
  QVERIFY(mDatabase.tables(QSql::AllTables).contains("Address_tbl"));
  QVERIFY(mDatabase.tables(QSql::AllTables).contains("ClientAddress_view"));
  QVERIFY(driver.dropSchema(schema));
  QVERIFY(!mDatabase.tables(QSql::AllTables).contains("Client_tbl"));
  QVERIFY(!mDatabase.tables(QSql::AllTables).contains("Address_tbl"));
  QVERIFY(!mDatabase.tables(QSql::AllTables).contains("ClientAddress_view"));
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

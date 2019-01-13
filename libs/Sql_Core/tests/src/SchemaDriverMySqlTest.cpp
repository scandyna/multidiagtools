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
#include "Mdt/Sql/Schema/Driver.h"
#include "Mdt/Sql/Schema/DriverMySql.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <QDebug>

using namespace Mdt::Sql::Schema;

void SchemaDriverMySqlTest::initTestCase()
{
  /*
   * Some tests needs a connection
   * to the server, and all access to a database
   */
  const QString host = "localhost";
  const QString user = "TestUser";
  const QString pwd = "TestPassword";
  const QString dbName = "test";

  // Get database instance
  mDatabase = QSqlDatabase::addDatabase("QMYSQL");
  if(!mDatabase.isValid()){
    QSKIP("QMYSQL driver is not available - Skip all tests");  // Will also skip all tests
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
  if(mDatabase.isOpen()){
    QSqlQuery query(mDatabase);
    // Define database default charset (is important for some tests)
    QVERIFY(query.exec("ALTER DATABASE `" + dbName + "` CHARACTER SET 'utf8'"));
  }
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

  auto db = mDatabase;
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
  DriverMySql driver(mDatabase);
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

void SchemaDriverMySqlTest::fieldTypeNameTest()
{
  DriverMySql driver(mDatabase);

  QCOMPARE(driver.fieldTypeName(FieldType::UnknownType), QString());
  QCOMPARE(driver.fieldTypeName(FieldType::Boolean), QString("BOOLEAN"));
  QCOMPARE(driver.fieldTypeName(FieldType::Smallint), QString("SMALLINT"));
  QCOMPARE(driver.fieldTypeName(FieldType::Integer), QString("INT"));
  QCOMPARE(driver.fieldTypeName(FieldType::Bigint), QString("BIGINT"));
  QCOMPARE(driver.fieldTypeName(FieldType::Float), QString("FLOAT"));
  QCOMPARE(driver.fieldTypeName(FieldType::Double), QString("DOUBLE"));
  QCOMPARE(driver.fieldTypeName(FieldType::Char), QString("CHARACTER"));
  QCOMPARE(driver.fieldTypeName(FieldType::Varchar), QString("VARCHAR"));
  QCOMPARE(driver.fieldTypeName(FieldType::Text), QString("TEXT"));
  QCOMPARE(driver.fieldTypeName(FieldType::Blob), QString("BLOB"));
  QCOMPARE(driver.fieldTypeName(FieldType::Date), QString("DATE"));
  QCOMPARE(driver.fieldTypeName(FieldType::Time), QString("TIME"));
  QCOMPARE(driver.fieldTypeName(FieldType::DateTime), QString("DATETIME"));
}

void SchemaDriverMySqlTest::fieldTypeFromStringTest()
{
  DriverMySql driver(mDatabase);

  QVERIFY(driver.fieldTypeFromString("BOOLEAN") == FieldType::Boolean);
  QVERIFY(driver.fieldTypeFromString("boolean") == FieldType::Boolean);
  QVERIFY(driver.fieldTypeFromString("BOOL") == FieldType::Boolean);
  QVERIFY(driver.fieldTypeFromString("bool") == FieldType::Boolean);
  QVERIFY(driver.fieldTypeFromString("BIT") == FieldType::Boolean);
  QVERIFY(driver.fieldTypeFromString("BIT(1)") == FieldType::Boolean);
  QVERIFY(driver.fieldTypeFromString("BIT(2)") == FieldType::Smallint);
  QVERIFY(driver.fieldTypeFromString("BIT(16)") == FieldType::Smallint);
  QVERIFY(driver.fieldTypeFromString("BIT(17)") == FieldType::Integer);
  QVERIFY(driver.fieldTypeFromString("BIT(32)") == FieldType::Integer);
  QVERIFY(driver.fieldTypeFromString("BIT(33)") == FieldType::Bigint);
  QVERIFY(driver.fieldTypeFromString("BIT(64)") == FieldType::Bigint);
  QVERIFY(driver.fieldTypeFromString("bit") == FieldType::Boolean);
  QVERIFY(driver.fieldTypeFromString("TINYINT") == FieldType::Smallint);
  QVERIFY(driver.fieldTypeFromString("tinyint") == FieldType::Smallint);
  QVERIFY(driver.fieldTypeFromString("SMALLINT") == FieldType::Smallint);
  QVERIFY(driver.fieldTypeFromString("smallint") == FieldType::Smallint);
  QVERIFY(driver.fieldTypeFromString("MEDIUMINT") == FieldType::Integer);
  QVERIFY(driver.fieldTypeFromString("mediumint") == FieldType::Integer);
  QVERIFY(driver.fieldTypeFromString("INTEGER") == FieldType::Integer);
  QVERIFY(driver.fieldTypeFromString("integer") == FieldType::Integer);
  QVERIFY(driver.fieldTypeFromString("Integer") == FieldType::Integer);
  QVERIFY(driver.fieldTypeFromString("INT") == FieldType::Integer);
  QVERIFY(driver.fieldTypeFromString("int") == FieldType::Integer);
  QVERIFY(driver.fieldTypeFromString("BIGINT") == FieldType::Bigint);
  QVERIFY(driver.fieldTypeFromString("bigint") == FieldType::Bigint);
  QVERIFY(driver.fieldTypeFromString("SERIAL") == FieldType::Bigint);
  QVERIFY(driver.fieldTypeFromString("serial") == FieldType::Bigint);
  QVERIFY(driver.fieldTypeFromString("DECIMAL") == FieldType::Double);
  QVERIFY(driver.fieldTypeFromString("decimal") == FieldType::Double);
  QVERIFY(driver.fieldTypeFromString("FLOAT") == FieldType::Float);
  QVERIFY(driver.fieldTypeFromString("float") == FieldType::Float);
  QVERIFY(driver.fieldTypeFromString("DOUBLE") == FieldType::Double);
  QVERIFY(driver.fieldTypeFromString("double") == FieldType::Double);
  QVERIFY(driver.fieldTypeFromString("DOUBLE PRECISION") == FieldType::Double);
  QVERIFY(driver.fieldTypeFromString("double precision") == FieldType::Double);
  QVERIFY(driver.fieldTypeFromString("CHAR") == FieldType::Char);
  QVERIFY(driver.fieldTypeFromString("char") == FieldType::Char);
  QVERIFY(driver.fieldTypeFromString("CHAR(1)") == FieldType::Char);
  QVERIFY(driver.fieldTypeFromString("CHAR(2)") == FieldType::Char);
  QVERIFY(driver.fieldTypeFromString("CHAR(10)") == FieldType::Char);
  QVERIFY(driver.fieldTypeFromString("VARCHAR") == FieldType::Varchar);
  QVERIFY(driver.fieldTypeFromString("varchar") == FieldType::Varchar);
  QVERIFY(driver.fieldTypeFromString("VARCHAR(50)") == FieldType::Varchar);
  QVERIFY(driver.fieldTypeFromString("VARCHAR (50)") == FieldType::Varchar);
  QVERIFY(driver.fieldTypeFromString("TINYTEXT") == FieldType::Text);
  QVERIFY(driver.fieldTypeFromString("TEXT") == FieldType::Text);
  QVERIFY(driver.fieldTypeFromString("MEDIUMTEXT") == FieldType::Text);
  QVERIFY(driver.fieldTypeFromString("LONGTEXT") == FieldType::Text);
  QVERIFY(driver.fieldTypeFromString("BINARY") == FieldType::Blob);
  QVERIFY(driver.fieldTypeFromString("TINYBLOB") == FieldType::Blob);
  QVERIFY(driver.fieldTypeFromString("BLOB") == FieldType::Blob);
  QVERIFY(driver.fieldTypeFromString("MEDIUMBLOB") == FieldType::Blob);
  QVERIFY(driver.fieldTypeFromString("LONGBLOB") == FieldType::Blob);
  QVERIFY(driver.fieldTypeFromString("DATE") == FieldType::Date);
  QVERIFY(driver.fieldTypeFromString("date") == FieldType::Date);
  QVERIFY(driver.fieldTypeFromString("TIME") == FieldType::Time);
  QVERIFY(driver.fieldTypeFromString("time") == FieldType::Time);
  QVERIFY(driver.fieldTypeFromString("DATETIME") == FieldType::DateTime);
  QVERIFY(driver.fieldTypeFromString("datetime") == FieldType::DateTime);
  QVERIFY(driver.fieldTypeFromString("TIMESTAMP") == FieldType::DateTime);
  QVERIFY(driver.fieldTypeFromString("timestamp") == FieldType::DateTime);
  QVERIFY(driver.fieldTypeFromString("Unkown") == FieldType::UnknownType);
  QVERIFY(driver.fieldTypeFromString("YEAR") == FieldType::Integer);
  QVERIFY(driver.fieldTypeFromString("year") == FieldType::Integer);
}

void SchemaDriverMySqlTest::fieldLengthFromStringTest()
{
  DriverMySql driver(mDatabase);

  QCOMPARE(driver.fieldLengthFromString("INTEGER"), -1);
  QCOMPARE(driver.fieldLengthFromString("VARCHAR"), -1);
  QCOMPARE(driver.fieldLengthFromString("VARCHAR(50)"), 50);
  QCOMPARE(driver.fieldLengthFromString("VARCHAR (50)"), 50);
  QCOMPARE(driver.fieldLengthFromString("varcher(50)"), 50);
  QCOMPARE(driver.fieldLengthFromString("VARCHAR( 50 )"), 50);
  QCOMPARE(driver.fieldLengthFromString("VARCHAR ( 50 )"), 50);
  // Currently not supported..
  QCOMPARE(driver.fieldLengthFromString("DOUBLE(2,3)"), -2);

  QFAIL("Not complete");
}

void SchemaDriverMySqlTest::databaseDefaultCharsetTest()
{
  DriverMySql driver(mDatabase);
  Charset cs;

  /*
   * Encoding was specified in initTestCase()
   */
  cs = driver.getDatabaseDefaultCharset();
  QCOMPARE(cs.charsetName(), QString("utf8"));
}

void SchemaDriverMySqlTest::collationDefinitionTest()
{
  DriverMySql driver(mDatabase);
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
  DriverMySql driver(mDatabase);
  QString expectedSql;
  Field field;

  /*
   * Simple Integer field
   */
  field.clear();
  field.setName("Number");
  field.setType(FieldType::Integer);
  // Check
  expectedSql = "`Number` INT DEFAULT NULL";
  QCOMPARE(driver.getFieldDefinition(field), expectedSql);
  /*
   * Unsigned Integer field
   */
  field.clear();
  field.setName("Number");
  field.setType(FieldType::Integer);
  field.setUnsigned(true);
  // Check
  expectedSql = "`Number` INT UNSIGNED DEFAULT NULL";
  QCOMPARE(driver.getFieldDefinition(field), expectedSql);
  /*
   * Integer field with UNIQUE constraint
   */
  field.clear();
  field.setName("Number");
  field.setType(FieldType::Integer);
  field.setUnique(true);
  // Check
  expectedSql = "`Number` INT UNIQUE DEFAULT NULL";
  QCOMPARE(driver.getFieldDefinition(field), expectedSql);
  /*
   * Integer field with NOT NULL constraint
   */
  field.clear();
  field.setName("Number");
  field.setType(FieldType::Integer);
  field.setRequired(true);
  // Check
  expectedSql = "`Number` INT NOT NULL DEFAULT NULL";
  QCOMPARE(driver.getFieldDefinition(field), expectedSql);
  /*
   * Integer field with UNIQUE and NOT NULL constraints
   */
  field.clear();
  field.setName("Number");
  field.setType(FieldType::Integer);
  field.setRequired(true);
  field.setUnique(true);
  // Check
  expectedSql = "`Number` INT UNIQUE NOT NULL DEFAULT NULL";
  QCOMPARE(driver.getFieldDefinition(field), expectedSql);
  /*
   * Integer field with a default value
   */
  field.clear();
  field.setName("Number");
  field.setType(FieldType::Integer);
  field.setDefaultValue(5);
  // Check
  expectedSql = "`Number` INT DEFAULT 5";
  QCOMPARE(driver.getFieldDefinition(field), expectedSql);
  /*
   * Simple VARCHAR field
   */
  field.clear();
  field.setName("Name");
  field.setType(FieldType::Varchar);
  field.setLength(50);
  // Check
  expectedSql = "`Name` VARCHAR(50) DEFAULT NULL";
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
  expectedSql = "`Name` VARCHAR(150) DEFAULT \\Default name\\";
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
  expectedSql = "`Name` VARCHAR(100) DEFAULT NULL COLLATE NOCASE";
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
  expectedSql = "`Name` VARCHAR(25) NOT NULL DEFAULT NULL";
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
  expectedSql = "`Name` VARCHAR(50) NOT NULL DEFAULT NULL COLLATE NOCASE";
  QCOMPARE(driver.getFieldDefinition(field), expectedSql);

  QFAIL("No complete/correct");
}

void SchemaDriverMySqlTest::autoIncrementPrimaryKeyDefinitionTest()
{
  using Mdt::Sql::Schema::AutoIncrementPrimaryKey;

  Mdt::Sql::Schema::DriverMySql driver(mDatabase);
  QString expectedSql;
  AutoIncrementPrimaryKey pk;

  pk.setFieldName("Id_PK");

  QFAIL("No implemented yet");
}

void SchemaDriverMySqlTest::singleFieldPrimaryKeyDefinitionTest()
{
  DriverMySql driver(mDatabase);
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

  QFAIL("Not complete");
}

void SchemaDriverMySqlTest::indexDefinitionTest()
{

  QFAIL("Not complete");
}

void SchemaDriverMySqlTest::createAndDropTableTest()
{
  if(!mDatabase.isOpen()){
    QSKIP("Not connected to server");
  }
  Mdt::Sql::Schema::Driver driver(mDatabase);
  QVERIFY(driver.isValid());

  Table table;
  table.setTableName("TestTable");
  table.setAutoIncrementPrimaryKey("Id_PK");

  Field field;
  field.setName("f_A");
  field.setType(FieldType::Integer);
  table.addField(field);

  field.clear();
  field.setName("f_B");
  field.setType(FieldType::Integer);
  field.setUnsigned(true);
  table.addField(field);

  QVERIFY(!mDatabase.tables().contains(table.tableName()));
  QVERIFY(driver.createTable(table));
  QVERIFY(mDatabase.tables().contains(table.tableName()));
  QVERIFY(driver.dropTable(table));
  QVERIFY(!mDatabase.tables().contains(table.tableName()));
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

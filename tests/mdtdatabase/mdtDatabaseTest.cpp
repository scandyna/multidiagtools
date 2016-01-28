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
#include "mdtDatabaseTest.h"
#include "mdtApplication.h"

#include "mdtSqlDatabaseManager.h"

#include "mdtSqlConnectionNameWidget.h"
#include "mdtSqlDatabaseBasicInfoWidget.h"
#include "mdtSqlDatabaseSqlite.h"
#include "mdtSqlDatabaseDialogSqlite.h"
#include "mdtSqlDatabaseSchemaThread.h"

#include "mdtSqlTableSetupWidget.h"

#include "mdtSqlField.h"
#include "mdtSqlIndexBase.h"
#include "mdtSqlIndex.h"
#include "mdtSqlPrimaryKey.h"
#include "mdtSqlForeignKey.h"
#include "mdtSqlTableSchema.h"
#include "mdtSqlTableSchemaModel.h"
#include "mdtSqlViewSchema.h"
#include "mdtSqlTriggerSchema.h"
#include "mdtSqlTablePopulationSchema.h"
#include "mdtSqlDatabaseSchema.h"
#include "mdtSqlDatabaseSchemaModel.h"
#include "mdtSqlDatabaseSchemaDialog.h"

#include "mdtSqlFieldSetupWidget.h"
#include "mdtSqlFieldSetupDialog.h"

#include "mdtSqlRecord.h"
#include "mdtSqlRelation.h"
#include "mdtSqlRelationInfo.h"
#include "mdtSqlTransaction.h"
#include "mdtSqlDriverType.h"
#include "mdtSqlFieldType.h"
#include "mdtSqlWhereOperator.h"
#include <QTemporaryFile>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QTableView>
#include <QTreeView>
#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDateTimeEdit>
#include <QAbstractButton>
#include <QComboBox>
#include <QString>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QFile>
#include <QVariant>
#include <QTimer>
#include <QAbstractButton>
#include <QModelIndex>
#include <QList>
#include <limits>
#include <memory>

#include <QTableView>
#include <QItemSelectionModel>

#include <QSignalSpy>

#include <QDebug>

void mdtDatabaseTest::initTestCase()
{
  createTestDatabase();
  clearTestDatabaseData();
}

void mdtDatabaseTest::whereOperatorTest()
{
  mdtSqlWhereOperator op;

  // Initial state
  QVERIFY(op == mdtSqlWhereOperator::Null);
  QVERIFY(op.toString().isEmpty());
  /*
   * Affectation
   */
  mdtSqlWhereOperator op1 = mdtSqlWhereOperator::And;
  QVERIFY(op1.value == mdtSqlWhereOperator::And);
  auto op2 = op1;
  QVERIFY(op2.value == mdtSqlWhereOperator::And);
  /*
   * Comprison operators
   */
  QVERIFY(op1 == op2);
  QVERIFY(op1.value == op2);
  QVERIFY(op1 == op2.value);
  QVERIFY(op1.value == op2.value);
  QVERIFY(!(op1 != op2));
  QVERIFY(!(op1.value != op2));
  QVERIFY(!(op1 != op2.value));
  QVERIFY(!(op1.value != op2.value));
  /*
   * String representation
   */
  QVERIFY(mdtSqlWhereOperatorString(mdtSqlWhereOperator::Null).isEmpty());
  QCOMPARE(mdtSqlWhereOperatorString(mdtSqlWhereOperator::And), QString("AND"));
  QCOMPARE(mdtSqlWhereOperatorString(mdtSqlWhereOperator::Or), QString("OR"));
  // Check object function
  op = mdtSqlWhereOperator::Null;
  QVERIFY(op.toString().isEmpty());
  op = mdtSqlWhereOperator::And;
  QCOMPARE(op.toString(), QString("AND"));
  op = mdtSqlWhereOperator::Or;
  QCOMPARE(op.toString(), QString("OR"));
}

void mdtDatabaseTest::sqlDriverTypeTest()
{
  mdtSqlDriverType dt;

  // Static functions
  QCOMPARE(mdtSqlDriverType::nameFromType(mdtSqlDriverType::Unknown), QString(""));
  QCOMPARE(mdtSqlDriverType::nameFromType(mdtSqlDriverType::SQLite), QString("QSQLITE"));
  QCOMPARE(mdtSqlDriverType::nameFromType(mdtSqlDriverType::MariaDB), QString("QMYSQL"));
  QCOMPARE(mdtSqlDriverType::nameFromType(mdtSqlDriverType::MySQL), QString("QMYSQL"));
  QVERIFY(mdtSqlDriverType::typeFromName("QSQLITE") == mdtSqlDriverType::SQLite);
  QVERIFY(mdtSqlDriverType::typeFromName("QMYSQL") == mdtSqlDriverType::MySQL);
  QVERIFY(mdtSqlDriverType::typeFromName("") == mdtSqlDriverType::Unknown);
  QVERIFY(mdtSqlDriverType::typeFromName("FAKENAME") == mdtSqlDriverType::Unknown);
  // Initial state
  QVERIFY(dt.type() == mdtSqlDriverType::Unknown);
  QVERIFY(dt.isNull());
  // Set from type
  dt.setType(mdtSqlDriverType::SQLite);
  QVERIFY(!dt.isNull());
  QVERIFY(dt.type() == mdtSqlDriverType::SQLite);
  QCOMPARE(dt.name(), QString("QSQLITE"));
  dt.setType(mdtSqlDriverType::Unknown);
  QVERIFY(dt.isNull());
  // Set from name
  QVERIFY(dt.setType("QSQLITE"));
  QVERIFY(!dt.isNull());
  QVERIFY(dt.type() == mdtSqlDriverType::SQLite);
  QCOMPARE(dt.name(), QString("QSQLITE"));
  QVERIFY(dt.setType("QMYSQL"));
  QVERIFY(!dt.isNull());
  QVERIFY(dt.type() == mdtSqlDriverType::MySQL);
  QCOMPARE(dt.name(), QString("QMYSQL"));
  QVERIFY(!dt.setType(""));
  QVERIFY(dt.isNull());
  QVERIFY(dt.type() == mdtSqlDriverType::Unknown);
  QCOMPARE(dt.name(), QString(""));
  QVERIFY(!dt.setType("FAKENAME"));
  QVERIFY(dt.isNull());
  QVERIFY(dt.type() == mdtSqlDriverType::Unknown);
  QCOMPARE(dt.name(), QString(""));
  // Set a valid type before clear test
  dt.setType(mdtSqlDriverType::MariaDB);
  QVERIFY(!dt.isNull());
  QVERIFY(dt.type() == mdtSqlDriverType::MariaDB);
  // Clear
  dt.clear();
  QVERIFY(dt.isNull());
  QVERIFY(dt.type() == mdtSqlDriverType::Unknown);
  QCOMPARE(dt.name(), QString(""));
}

void mdtDatabaseTest::sqlFieldTypeTest()
{
  mdtSqlFieldType ft;
  QList<mdtSqlFieldType> fieldTypeList;

  /*
   * Object checks
   */
  // Initial state
  QVERIFY(ft.type() == mdtSqlFieldType::UnknownType);
  QCOMPARE(ft.length(), -1);
  QVERIFY(ft.isNull());
  // Set type including type name
  ft.setType(mdtSqlFieldType::Integer, mdtSqlDriverType::SQLite);
  QVERIFY(!ft.isNull());
  QVERIFY(ft.type() == mdtSqlFieldType::Integer);
  QCOMPARE(ft.name(), QString("INTEGER"));
  // Set type without type name
  ft.setType(mdtSqlFieldType::Integer);
  QVERIFY(!ft.isNull());
  QVERIFY(ft.type() == mdtSqlFieldType::Integer);
  QVERIFY(ft.name().isEmpty());
  // Set a valid type before clear test
  ft.setType(mdtSqlFieldType::Varchar, mdtSqlDriverType::SQLite);
  ft.setLength(50);
  QVERIFY(!ft.isNull());
  QVERIFY(ft.type() == mdtSqlFieldType::Varchar);
  QCOMPARE(ft.name(), QString("VARCHAR"));
  QCOMPARE(ft.length(), 50);
  // Clear
  ft.clear();
  QVERIFY(ft.type() == mdtSqlFieldType::UnknownType);
  QVERIFY(ft.name().isEmpty());
  QCOMPARE(ft.length(), -1);
  QVERIFY(ft.isNull());
  /*
   * Check field type mapping - SQLite
   */
  // QVariant -> mdtSqlFieldType
  QVERIFY(mdtSqlFieldType::fromQVariantType(QVariant::Invalid, mdtSqlDriverType::SQLite) == mdtSqlFieldType::UnknownType);
  QVERIFY(mdtSqlFieldType::fromQVariantType(QVariant::Int, mdtSqlDriverType::SQLite) == mdtSqlFieldType::Integer);
  QVERIFY(mdtSqlFieldType::fromQVariantType(QVariant::Bool, mdtSqlDriverType::SQLite) == mdtSqlFieldType::Boolean);
  QVERIFY(mdtSqlFieldType::fromQVariantType(QVariant::Polygon, mdtSqlDriverType::SQLite) == mdtSqlFieldType::UnknownType);
  // mdtSqlFieldType -> QVariant
  QVERIFY(mdtSqlFieldType::toQVariantType(mdtSqlFieldType::UnknownType) == QVariant::Invalid);
  QVERIFY(mdtSqlFieldType::toQVariantType(mdtSqlFieldType::Boolean) == QVariant::Bool);
  QVERIFY(mdtSqlFieldType::toQVariantType(mdtSqlFieldType::Integer) == QVariant::Int);
  QVERIFY(mdtSqlFieldType::toQVariantType(mdtSqlFieldType::Varchar) == QVariant::String);
  QVERIFY(mdtSqlFieldType::toQVariantType(mdtSqlFieldType::Date) == QVariant::Date);
  QVERIFY(mdtSqlFieldType::toQVariantType(mdtSqlFieldType::Time) == QVariant::Time);
  QVERIFY(mdtSqlFieldType::toQVariantType(mdtSqlFieldType::DateTime) == QVariant::DateTime);
  /*
   * Check getting field type name - SQLite
   */
  QCOMPARE(mdtSqlFieldType::nameFromType(mdtSqlFieldType::Boolean, mdtSqlDriverType::SQLite) , QString("BOOLEAN"));
  QCOMPARE(mdtSqlFieldType::nameFromType(mdtSqlFieldType::Integer, mdtSqlDriverType::SQLite) , QString("INTEGER"));
  QCOMPARE(mdtSqlFieldType::nameFromType(mdtSqlFieldType::Float, mdtSqlDriverType::SQLite) , QString("FLOAT"));
  QCOMPARE(mdtSqlFieldType::nameFromType(mdtSqlFieldType::Double, mdtSqlDriverType::SQLite) , QString("DOUBLE"));
  QCOMPARE(mdtSqlFieldType::nameFromType(mdtSqlFieldType::Varchar, mdtSqlDriverType::SQLite) , QString("VARCHAR"));
  QCOMPARE(mdtSqlFieldType::nameFromType(mdtSqlFieldType::Date, mdtSqlDriverType::SQLite) , QString("DATE"));
  ///QCOMPARE(mdtSqlFieldType::nameFromType(mdtSqlFieldType::Time, mdtSqlDriverType::SQLite) , QString("??"));
  /*
   * Check getting field type from name - SQLite
   */
  ft = mdtSqlFieldType::typeFromName("BOOLEAN", mdtSqlDriverType::SQLite);
  QVERIFY(ft.type() == mdtSqlFieldType::Boolean);
  QCOMPARE(ft.name(), QString("BOOLEAN"));
  QCOMPARE(ft.length(), -1);
  ft = mdtSqlFieldType::typeFromName("BOOL", mdtSqlDriverType::SQLite);
  QVERIFY(ft.type() == mdtSqlFieldType::Boolean);
  QCOMPARE(ft.name(), QString("BOOL"));
  QCOMPARE(ft.length(), -1);
  ft = mdtSqlFieldType::typeFromName("bool", mdtSqlDriverType::SQLite);
  QVERIFY(ft.type() == mdtSqlFieldType::Boolean);
  QCOMPARE(ft.name(), QString("bool"));
  QCOMPARE(ft.length(), -1);
  ft = mdtSqlFieldType::typeFromName("INTEGER", mdtSqlDriverType::SQLite);
  QVERIFY(ft.type() == mdtSqlFieldType::Integer);
  QCOMPARE(ft.name(), QString("INTEGER"));
  QCOMPARE(ft.length(), -1);
  ft = mdtSqlFieldType::typeFromName("INT", mdtSqlDriverType::SQLite);
  QVERIFY(ft.type() == mdtSqlFieldType::Integer);
  QCOMPARE(ft.name(), QString("INT"));
  QCOMPARE(ft.length(), -1);
  ft = mdtSqlFieldType::typeFromName("FLOAT", mdtSqlDriverType::SQLite);
  QVERIFY(ft.type() == mdtSqlFieldType::Float);
  QCOMPARE(ft.name(), QString("FLOAT"));
  QCOMPARE(ft.length(), -1);
  ft = mdtSqlFieldType::typeFromName("DOUBLE", mdtSqlDriverType::SQLite);
  QVERIFY(ft.type() == mdtSqlFieldType::Double);
  QCOMPARE(ft.name(), QString("DOUBLE"));
  QCOMPARE(ft.length(), -1);
  ft = mdtSqlFieldType::typeFromName("VARCHAR(50)", mdtSqlDriverType::SQLite);
  QVERIFY(ft.type() == mdtSqlFieldType::Varchar);
  QCOMPARE(ft.name(), QString("VARCHAR"));
  QCOMPARE(ft.length(), 50);
  ft = mdtSqlFieldType::typeFromName("VARCHAR (50)", mdtSqlDriverType::SQLite);
  QVERIFY(ft.type() == mdtSqlFieldType::Varchar);
  QCOMPARE(ft.name(), QString("VARCHAR"));
  QCOMPARE(ft.length(), 50);
  ft = mdtSqlFieldType::typeFromName("DATE", mdtSqlDriverType::SQLite);
  QVERIFY(ft.type() == mdtSqlFieldType::Date);
  QCOMPARE(ft.name(), QString("DATE"));
  QCOMPARE(ft.length(), -1);
  ft = mdtSqlFieldType::typeFromName("TIME", mdtSqlDriverType::SQLite);
  QVERIFY(ft.type() == mdtSqlFieldType::Time);
  QCOMPARE(ft.name(), QString("TIME"));
  QCOMPARE(ft.length(), -1);
  ft = mdtSqlFieldType::typeFromName("DATETIME", mdtSqlDriverType::SQLite);
  QVERIFY(ft.type() == mdtSqlFieldType::DateTime);
  QCOMPARE(ft.name(), QString("DATETIME"));
  QCOMPARE(ft.length(), -1);

  // Static functions - SQLite
  /// \todo To be done
  // Available field type list - SQLite
  fieldTypeList = mdtSqlFieldType::availableFieldTypeList(mdtSqlDriverType::SQLite);
  QCOMPARE(fieldTypeList.size(), 7);
  QVERIFY(fieldTypeList.at(0).type() == mdtSqlFieldType::Boolean);
  QCOMPARE(fieldTypeList.at(0).name(), QString("BOOLEAN"));
  QVERIFY(fieldTypeList.at(1).type() == mdtSqlFieldType::Integer);
  QCOMPARE(fieldTypeList.at(1).name(), QString("INTEGER"));
  QVERIFY(fieldTypeList.at(2).type() == mdtSqlFieldType::Float);
  QCOMPARE(fieldTypeList.at(2).name(), QString("FLOAT"));
  QVERIFY(fieldTypeList.at(3).type() == mdtSqlFieldType::Double);
  QCOMPARE(fieldTypeList.at(3).name(), QString("DOUBLE"));
  QVERIFY(fieldTypeList.at(4).type() == mdtSqlFieldType::Varchar);
  QCOMPARE(fieldTypeList.at(4).name(), QString("VARCHAR"));
  QVERIFY(fieldTypeList.at(5).type() == mdtSqlFieldType::Date);
  QCOMPARE(fieldTypeList.at(5).name(), QString("DATE"));
  QVERIFY(fieldTypeList.at(6).type() == mdtSqlFieldType::Time);
  QCOMPARE(fieldTypeList.at(6).name(), QString("TIME"));
  // Static functions - MySQL
  /// \todo To be done
  // Available field type list - MariaDB/MySQL
  /// \todo To be done
  // Initial state
}

void mdtDatabaseTest::sqlFieldTypeCompareTest()
{
  mdtSqlFieldType ftA, ftB;
  mdtSqlDriverType::Type driverType = mdtSqlDriverType::SQLite;

  /*
   * Initially, ftA and ftB are UnknownType,
   * so they are not equal.
   */
  QVERIFY(ftA != ftB);
  /*
   * Check by setting only type
   */
  ftA.setType(mdtSqlFieldType::Integer, driverType);
  ftB.setType(mdtSqlFieldType::Integer, driverType);
  QVERIFY(ftA == ftB);
  ftA.setType(mdtSqlFieldType::Integer, driverType);
  ftB.setType(mdtSqlFieldType::Boolean, driverType);
  QVERIFY(ftA != ftB);
  /*
   * Set same type, but vary length
   */
  ftA.setType(mdtSqlFieldType::Integer, driverType);
  ftB.setType(mdtSqlFieldType::Integer, driverType);
  QVERIFY(ftA == ftB);
  ftA.setLength(-1);
  ftB.setLength(-1);
  QVERIFY(ftA == ftB);
  ftA.setLength(50);
  ftB.setLength(-1);
  QVERIFY(ftA != ftB);
  ftA.setLength(50);
  ftB.setLength(50);
  QVERIFY(ftA == ftB);
  ftA.setLength(-1);
  ftB.setLength(50);
  QVERIFY(ftA != ftB);

}

void mdtDatabaseTest::sqlCollationTest()
{
  mdtSqlCollation collation;
  QString expectedSql;

  /*
   * Initial state
   */
  QVERIFY(collation.isCaseSensitive());
  QVERIFY(collation.country() == QLocale::AnyCountry);
  QVERIFY(collation.language() == QLocale::AnyLanguage);
  QVERIFY(collation.isNull());
  /*
   * Simple set/get
   */
  collation.setCaseSensitive(false);
  collation.setCountry(QLocale::Switzerland);
  collation.setLanguage(QLocale::French);
  collation.setCharset("utf8");
  QVERIFY(!collation.isCaseSensitive());
  QVERIFY(collation.country() == QLocale::Switzerland);
  QVERIFY(collation.language() == QLocale::French);
  QCOMPARE(collation.charset(), QString("utf8"));
  QVERIFY(!collation.isNull());
  /*
   * Clear
   */
  collation.clear();
  QVERIFY(collation.isCaseSensitive());
  QVERIFY(collation.country() == QLocale::AnyCountry);
  QVERIFY(collation.language() == QLocale::AnyLanguage);
  QVERIFY(collation.charset().isEmpty());
  QVERIFY(collation.isNull());
  /*
   * SQL for SQLite
   */
  // Null collation
  collation.clear();
  expectedSql = "";
  QCOMPARE(collation.getSql(mdtSqlDriverType::SQLite), expectedSql);
  // Setup collation
  collation.setCaseSensitive(false);
  collation.setCountry(QLocale::Switzerland); // Must simply be ignored
  collation.setLanguage(QLocale::French);     // Must simply be ignored
  collation.setCharset("utf8");               // Must simply be ignored
  // Check
  expectedSql = "NOCASE";
  QCOMPARE(collation.getSql(mdtSqlDriverType::SQLite), expectedSql);
  // Set a case sensitive collation
  collation.clear();
  collation.setCaseSensitive(true);
  expectedSql = "BINARY";
  QCOMPARE(collation.getSql(mdtSqlDriverType::SQLite), expectedSql);
}

void mdtDatabaseTest::sqlFieldTest()
{
  mdtSqlField field;
  mdtSqlFieldType ft;

  /*
   * Initial state
   */
  QVERIFY(field.type() == mdtSqlFieldType::UnknownType);
  QVERIFY(!field.isAutoValue());
  QVERIFY(!field.isRequired());
  QCOMPARE(field.length(), -1);
  QVERIFY(field.isNull());
  /*
   * Simple set/get check
   */
  // Setup a auto increment PK
  field.setType(mdtSqlFieldType::Integer);
  QVERIFY(field.isNull());
  field.setName("Id_PK");
  QVERIFY(!field.isNull());
  field.setAutoValue(true);
  field.setRequired(true);
  // Check
  QVERIFY(field.type() == mdtSqlFieldType::Integer);
  QCOMPARE(field.typeName(mdtSqlDriverType::Unknown), QString("INTEGER"));
  QCOMPARE(field.name(), QString("Id_PK"));
  QVERIFY(field.isAutoValue());
  QVERIFY(field.isRequired());
  /*
   * Clear
   */
  field.clear();
  QVERIFY(field.type() == mdtSqlFieldType::UnknownType);
  QVERIFY(field.name().isEmpty());
  QVERIFY(!field.isAutoValue());
  QVERIFY(!field.isRequired());
  QVERIFY(field.defaultValue().isNull());
  QCOMPARE(field.length(), -1);
  QVERIFY(field.collation().isNull());
  QVERIFY(field.isNull());
  /*
   * Simple set/get check
   */
  // Setup a text field
  field.setName("Name");
  QVERIFY(field.isNull());
  field.setType(mdtSqlFieldType::Varchar);
  QVERIFY(!field.isNull());
  field.setLength(50);
  field.setDefaultValue("Empty");
  field.setCaseSensitive(false);
  // Check
  QVERIFY(field.type() == mdtSqlFieldType::Varchar);
  QCOMPARE(field.name(), QString("Name"));
  QVERIFY(!field.isAutoValue());
  QVERIFY(!field.isRequired());
  QCOMPARE(field.length(), 50);
  QCOMPARE(field.defaultValue(), QVariant("Empty"));
  QVERIFY(!field.collation().isCaseSensitive());
  // Check getting field type object with its type name
  ft = field.getFieldType();
  QVERIFY(ft.type() == mdtSqlFieldType::Varchar);
  QCOMPARE(ft.length(), 50);
  QVERIFY(ft.name().isEmpty());
  // Check getting field type object with its type name
  ft = field.getFieldType(mdtSqlDriverType::SQLite);
  QVERIFY(ft.type() == mdtSqlFieldType::Varchar);
  QCOMPARE(ft.length(), 50);
  QCOMPARE(ft.name(), QString("VARCHAR"));
  /*
   * Clear
   */
  field.clear();
  QVERIFY(field.type() == mdtSqlFieldType::UnknownType);
  QVERIFY(field.name().isEmpty());
  QVERIFY(!field.isAutoValue());
  QVERIFY(!field.isRequired());
  QVERIFY(field.defaultValue().isNull());
  QCOMPARE(field.length(), -1);
  QVERIFY(field.collation().isNull());
  QVERIFY(field.isNull());
  /*
   * Check setting up from a QSqlField
   */
  // Setup QSqlField
  QSqlField qtField;
  qtField.setType(QVariant::Int);
  qtField.setName("Id_PK");
  qtField.setAutoValue(true);
  qtField.setRequired(true);
  qtField.setDefaultValue(2);
  qtField.setLength(5);
  // Setup field and check
  field.setupFromQSqlField(qtField, mdtSqlDriverType::SQLite);
  QVERIFY(field.type() == mdtSqlFieldType::Integer);
  QCOMPARE(field.name(), QString("Id_PK"));
  QVERIFY(field.isAutoValue());
  QVERIFY(field.isRequired());
  QCOMPARE(field.length(), 5);
  QCOMPARE(field.defaultValue(), QVariant(2));
  /*
   * Check converting to QSqlField
   */
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  field.setDefaultValue("Empty");
  field.setCaseSensitive(false);
  qtField = field.toQSqlField();
  QCOMPARE(qtField.name(), QString("Name"));
  QVERIFY(qtField.type() == QVariant::String);
  QCOMPARE(qtField.length(), 50);
  QCOMPARE(qtField.defaultValue(), QVariant("Empty"));
}

void mdtDatabaseTest::sqlFieldSQLiteTest()
{
  mdtSqlField field;
  QSqlDatabase db = pvDatabase;
  QString expectedSql;

  QCOMPARE(db.driverName(), QString("QSQLITE"));
  /*
   * Auto increment PK
   * Note: SQL statement must contain NOT NULL (see SQLite documentation about NULL and PK)
   */
  // Setup field
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  // Check
  expectedSql = "\"Id_PK\" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT";
  QCOMPARE(field.getSql(db, true), expectedSql);
  /*
   * Not auto increment PK
   * Note: SQL statement must contain NOT NULL (see SQLite documentation about NULL and PK)
   */
  // Setup field
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  // Check
  expectedSql = "\"Id_PK\" INTEGER NOT NULL PRIMARY KEY";
  QCOMPARE(field.getSql(db, true), expectedSql);
  /*
   * Simple INTEGER field
   */
  field.clear();
  field.setName("Number");
  field.setType(mdtSqlFieldType::Integer);
  // Check
  expectedSql = "\"Number\" INTEGER DEFAULT NULL";
  QCOMPARE(field.getSql(db, false), expectedSql);
  /*
   * INTEGER field with NOT NULL constraint
   */
  field.clear();
  field.setName("Number");
  field.setType(mdtSqlFieldType::Integer);
  field.setRequired(true);
  // Check
  expectedSql = "\"Number\" INTEGER NOT NULL DEFAULT NULL";
  QCOMPARE(field.getSql(db, false), expectedSql);
  /*
   * INTEGER field with a default value
   */
  field.clear();
  field.setName("Number");
  field.setType(mdtSqlFieldType::Integer);
  field.setDefaultValue(5);
  // Check
  expectedSql = "\"Number\" INTEGER DEFAULT 5";
  QCOMPARE(field.getSql(db, false), expectedSql);
  /*
   * Simple VARCHAR field
   */
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  // Check
  expectedSql = "\"Name\" VARCHAR(50) DEFAULT NULL";
  QCOMPARE(field.getSql(db, false), expectedSql);
  /*
   * VARCHAR field with collation set
   */
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(100);
  field.setCaseSensitive(false);
  // Check
  expectedSql = "\"Name\" VARCHAR(100) DEFAULT NULL COLLATE NOCASE";
  QCOMPARE(field.getSql(db, false), expectedSql);
  /*
   * VARCHAR field with NOT NULL constraint
   */
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(25);
  field.setRequired(true);
  // Check
  expectedSql = "\"Name\" VARCHAR(25) NOT NULL DEFAULT NULL";
  QCOMPARE(field.getSql(db, false), expectedSql);
  /*
   * VARCHAR field with NOT NULL constraint and a collation set
   */
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  field.setRequired(true);
  field.setCaseSensitive(false);
  // Check
  expectedSql = "\"Name\" VARCHAR(50) NOT NULL DEFAULT NULL COLLATE NOCASE";
  QCOMPARE(field.getSql(db, false), expectedSql);

}

void mdtDatabaseTest::sqlFieldMySQLTest()
{
  mdtSqlField field;
  QString expectedSql;
  QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", "sqlFieldMySQLTest");

  QCOMPARE(db.driverName(), QString("QMYSQL"));
  /*
   * Auto increment PK
   */
  // Setup field
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  field.setRequired(true);
  // Check
  expectedSql = "`Id_PK` INTEGER NOT NULL AUTO_INCREMENT PRIMARY KEY";
  QCOMPARE(field.getSql(db, true), expectedSql);
  /*
   * Simple INTEGER field
   */
  field.clear();
  field.setName("Number");
  field.setType(mdtSqlFieldType::Integer);
  // Check
  expectedSql = "`Number` INTEGER DEFAULT NULL";
  QCOMPARE(field.getSql(db, false), expectedSql);
  /*
   * INTEGER field with NOT NULL constraint
   */
  field.clear();
  field.setName("Number");
  field.setType(mdtSqlFieldType::Integer);
  field.setRequired(true);
  // Check
  expectedSql = "`Number` INTEGER NOT NULL DEFAULT NULL";
  QCOMPARE(field.getSql(db, false), expectedSql);
  /*
   * INTEGER field with a default value
   */
  field.clear();
  field.setName("Number");
  field.setType(mdtSqlFieldType::Integer);
  field.setDefaultValue(5);
  // Check
  expectedSql = "`Number` INTEGER DEFAULT 5";
  QCOMPARE(field.getSql(db, false), expectedSql);
  /*
   * Simple VARCHAR field
   */
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  // Check
  expectedSql = "`Name` VARCHAR(50) DEFAULT NULL";
  QCOMPARE(field.getSql(db, false), expectedSql);
  /*
   * VARCHAR field with collation set
   */
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(100);
  field.setCaseSensitive(false);
  // Check
  /// \todo Once MySQL collation is clear, fix
  expectedSql = "`Name` VARCHAR(100) DEFAULT NULL COLLATE NOCASE";
  QCOMPARE(field.getSql(db, false), expectedSql);
  /*
   * VARCHAR field with NOT NULL constraint
   */
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(25);
  field.setRequired(true);
  // Check
  expectedSql = "`Name` VARCHAR(25) NOT NULL DEFAULT NULL";
  QCOMPARE(field.getSql(db, false), expectedSql);
  /*
   * VARCHAR field with NOT NULL constraint and a collation set
   */
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  field.setRequired(true);
  field.setCaseSensitive(false);
  // Check
  /// \todo Once MySQL collation is clear, fix
  expectedSql = "`Name` VARCHAR(50) NOT NULL DEFAULT NULL COLLATE NOCASE";
  QCOMPARE(field.getSql(db, false), expectedSql);

  // Free resources
  db.close();
  db = QSqlDatabase();
  QSqlDatabase::removeDatabase(db.connectionName());
}

void mdtDatabaseTest::sqlIndexBaseTest()
{
  mdtSqlIndexBase index;
//   mdtSqlField field;
  QSqlField qtField;
  QSqlIndex qtIndex;

  /*
   * Initial state
   */
  QVERIFY(index.isNull());
  QVERIFY(!index.contains("Id_PK"));
  /*
   * Simple set/get
   */
  index.setName("IDX");
  QVERIFY(index.isNull());
  index.addField("Id_PK");
  QVERIFY(!index.isNull());
  QCOMPARE(index.fieldCount(), 1);
  QCOMPARE(index.fieldName(0), QString("Id_PK"));
  QVERIFY(index.contains("Id_PK"));
  /*
   * Clear
   */
  index.clear();
  QVERIFY(index.name().isEmpty());
  QCOMPARE(index.fieldCount(), 0);
  QVERIFY(index.isNull());
  QVERIFY(!index.contains("Id_PK"));
  /*
   * Setup from QSqlIndex
   */
  qtIndex.clear();
  qtIndex.setName("index_1");
  // Setup Id_A_PK
  qtField = QSqlField();
  qtField.setName("Id_A_PK");
  qtField.setType(QVariant::Int);
  qtField.setRequired(true);
  qtIndex.append(qtField);
  // Setup Id_B_PK
  qtField = QSqlField();
  qtField.setName("Id_B_PK");
  qtField.setType(QVariant::Int);
  qtField.setRequired(true);
  qtIndex.append(qtField);
  // Setup index
  index.setupFromQSqlIndex(qtIndex);
  // Check
  QVERIFY(!index.isNull());
  QCOMPARE(index.name(), qtIndex.name());
  QCOMPARE(index.fieldCount(), qtIndex.count());
  QCOMPARE(index.fieldName(0), qtIndex.field(0).name());
  QCOMPARE(index.fieldName(1), qtIndex.field(1).name());
}

void mdtDatabaseTest::sqlIndexTest()
{
  mdtSqlIndex index;
  QSqlDatabase db = pvDatabase;
  QString expectedSql;

  QCOMPARE(db.driverName(), QString("QSQLITE"));
  /*
   * Initial state
   */
  QVERIFY(!index.isUnique());
  QVERIFY(index.isNull());
  /*
   * Simple set/get
   */
  index.clear();
  index.setTableName("Client_tbl");
  index.setUnique(true);
  index.addField("Id_A");
  // Generate name
  index.generateName();
  // Check
  QVERIFY(!index.isNull());
  QCOMPARE(index.name(), QString("Client_tbl_Id_A_index"));
  QCOMPARE(index.tableName(), QString("Client_tbl"));
  /*
   * Clear
   */
  index.clear();
  QVERIFY(index.name().isEmpty());
  QVERIFY(index.tableName().isEmpty());
  QVERIFY(!index.isUnique());
  QCOMPARE(index.fieldCount(), 0);
  QVERIFY(index.isNull());
  /*
   * SQL generation: non unique with 1 field
   */
  // Setup index
  index.clear();
  index.addField("Id_A");
  index.setTableName("Client_tbl");
  index.generateName();
  // Generate SQL and check
  expectedSql = "DROP INDEX IF EXISTS \"Client_tbl_Id_A_index\"";
  QCOMPARE(index.getSqlForDrop(db, ""), expectedSql);
  expectedSql = "CREATE INDEX \"Client_tbl_Id_A_index\" ON \"Client_tbl\" (\"Id_A\")";
  QCOMPARE(index.getSqlForCreate(db, ""), expectedSql);
  /*
   * SQL generation: non unique with 1 field - database name specified
   */
  // Setup index
  index.clear();
  index.addField("Id_A");
  index.setTableName("Client_tbl");
  index.generateName();
  // Generate SQL and check
  expectedSql = "DROP INDEX IF EXISTS \"sandbox\".\"Client_tbl_Id_A_index\"";
  QCOMPARE(index.getSqlForDrop(db, "sandbox"), expectedSql);
  expectedSql = "CREATE INDEX \"sandbox\".\"Client_tbl_Id_A_index\" ON \"Client_tbl\" (\"Id_A\")";
  QCOMPARE(index.getSqlForCreate(db, "sandbox"), expectedSql);
  /*
   * SQL generation: unique with 2 fields
   */
  // Setup index
  index.clear();
  index.addField("Id_A");
  index.addField("Id_B");
  index.setTableName("Client_tbl");
  index.setUnique(true);
  index.generateName();
  // Generate SQL and check
  expectedSql = "DROP INDEX IF EXISTS \"Client_tbl_Id_A_Id_B_index\"";
  QCOMPARE(index.getSqlForDrop(db, ""), expectedSql);
  expectedSql = "CREATE UNIQUE INDEX \"Client_tbl_Id_A_Id_B_index\" ON \"Client_tbl\" (\"Id_A\",\"Id_B\")";
  QCOMPARE(index.getSqlForCreate(db, ""), expectedSql);
}

void mdtDatabaseTest::sqlPrimaryKeySqliteTest()
{
  mdtSqlPrimaryKey pk;
  QString expectedSql;
  QSqlDatabase db = pvDatabase;

  QCOMPARE(db.driverName(), QString("QSQLITE"));
  /*
   * Simple 1 field PK
   */
  // Setup PK
  pk.clear();
  pk.setName("PK_1");   // Must be ignored
  pk.addField("Code_PK");
  // Check
  expectedSql = "PRIMARY KEY (\"Code_PK\")";
  QCOMPARE(pk.getSql(db), expectedSql);
  /*
   * Simple 2 fields PK
   */
  pk.clear();
  pk.setName("PK_2");   // Must be ignored
  pk.addField("Id_A_PK");
  pk.addField("Id_B_PK");
  // Check
  expectedSql = "PRIMARY KEY (\"Id_A_PK\",\"Id_B_PK\")";
  QCOMPARE(pk.getSql(db), expectedSql);
}

void mdtDatabaseTest::sqlForeignKeyTest()
{
  mdtSqlForeignKey fk;

  /*
   * Initial state
   */
  QVERIFY(!fk.createParentIndex());
  QVERIFY(!fk.createChildIndex());
  QVERIFY(fk.onDeleteAction() == mdtSqlForeignKey::NoAction);
  QVERIFY(fk.onUpdateAction() == mdtSqlForeignKey::NoAction);
  /*
   * Simple set/get
   */
  // Set
  fk.setParentTableName("Client_tbl");
  fk.setChildTableName("Address_tbl");
  fk.addKeyFields("Id_PK", "Client_Id_FK");
  fk.setCreateChildIndex(true);
  fk.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  // Check
  QVERIFY(!fk.createParentIndex());
  QVERIFY(fk.createChildIndex());
  QVERIFY(fk.onDeleteAction() == mdtSqlForeignKey::Restrict);
  QVERIFY(fk.onUpdateAction() == mdtSqlForeignKey::Cascade);
  QCOMPARE(fk.parentTableName(), QString("Client_tbl"));
  QCOMPARE(fk.childTableName(), QString("Address_tbl"));
  /*
   * Clear
   */
  fk.clear();
  QVERIFY(!fk.createParentIndex());
  QVERIFY(!fk.createChildIndex());
  QVERIFY(fk.onDeleteAction() == mdtSqlForeignKey::NoAction);
  QVERIFY(fk.onUpdateAction() == mdtSqlForeignKey::NoAction);
  QVERIFY(fk.parentTableName().isEmpty());
  QVERIFY(fk.childTableName().isEmpty());
  /*
   * Check setting Action from action name
   */
  fk.clear();
  // Check setting on delete action
  fk.setOnDeleteAction("RESTRICT");
  QVERIFY(fk.onDeleteAction() == mdtSqlForeignKey::Restrict);
  fk.setOnDeleteAction(" RESTRICT");
  QVERIFY(fk.onDeleteAction() == mdtSqlForeignKey::Restrict);
  fk.setOnDeleteAction("restrict");
  QVERIFY(fk.onDeleteAction() == mdtSqlForeignKey::Restrict);
  fk.setOnDeleteAction("NO ACTION");
  QVERIFY(fk.onDeleteAction() == mdtSqlForeignKey::NoAction);
  fk.setOnDeleteAction("CASCADE");
  QVERIFY(fk.onDeleteAction() == mdtSqlForeignKey::Cascade);
  fk.setOnDeleteAction("SET NULL");
  QVERIFY(fk.onDeleteAction() == mdtSqlForeignKey::SetNull);
  fk.setOnDeleteAction("SET DEFAULT");
  QVERIFY(fk.onDeleteAction() == mdtSqlForeignKey::SetDefault);
  // Check setting on update action
  fk.setOnUpdateAction("RESTRICT");
  QVERIFY(fk.onUpdateAction() == mdtSqlForeignKey::Restrict);
  fk.setOnUpdateAction(" RESTRICT");
  QVERIFY(fk.onUpdateAction() == mdtSqlForeignKey::Restrict);
  fk.setOnUpdateAction("restrict");
  QVERIFY(fk.onUpdateAction() == mdtSqlForeignKey::Restrict);
  fk.setOnUpdateAction("NO ACTION");
  QVERIFY(fk.onUpdateAction() == mdtSqlForeignKey::NoAction);
  fk.setOnUpdateAction("CASCADE");
  QVERIFY(fk.onUpdateAction() == mdtSqlForeignKey::Cascade);
  fk.setOnUpdateAction("SET NULL");
  QVERIFY(fk.onUpdateAction() == mdtSqlForeignKey::SetNull);
  fk.setOnUpdateAction("SET DEFAULT");
  QVERIFY(fk.onUpdateAction() == mdtSqlForeignKey::SetDefault);
}

void mdtDatabaseTest::sqlForeignKeySqliteTest()
{
  mdtSqlForeignKey fk;
  QString expectedSql;
  QSqlDatabase db = pvDatabase;

  QCOMPARE(db.driverName(), QString("QSQLITE"));
  /*
   * Simple 1 field FK
   */
  fk.clear();
  fk.setParentTableName("Client_tbl");
  fk.setChildTableName("Address_tbl");
  fk.addKeyFields("Id_PK", "Client_Id_FK");
  fk.setCreateParentIndex(true);
  fk.setCreateChildIndex(true);
  fk.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  // Check SQL for FK
  expectedSql  = "  FOREIGN KEY (\"Client_Id_FK\")\n";
  expectedSql += "   REFERENCES \"Client_tbl\" (\"Id_PK\")\n";
  expectedSql += "   ON DELETE RESTRICT\n";
  expectedSql += "   ON UPDATE CASCADE";
  QCOMPARE(fk.getSqlForForeignKey(db), expectedSql);
  // Check SQL to drop parent table index
  expectedSql = "DROP INDEX IF EXISTS \"Client_tbl_Id_PK_index\"";
  QCOMPARE(fk.getSqlForDropParentTableIndex(db, ""), expectedSql);
  // Check SQL to create parent table index
  expectedSql = "CREATE INDEX \"Client_tbl_Id_PK_index\" ON \"Client_tbl\" (\"Id_PK\")";
  QCOMPARE(fk.getSqlForCreateParentTableIndex(db, ""), expectedSql);
  // Check SQL to drop child table index
  expectedSql = "DROP INDEX IF EXISTS \"Address_tbl_Client_Id_FK_index\"";
  QCOMPARE(fk.getSqlForDropChildTableIndex(db, ""), expectedSql);
  // Check SQL to create child table index
  expectedSql = "CREATE INDEX \"Address_tbl_Client_Id_FK_index\" ON \"Address_tbl\" (\"Client_Id_FK\")";
  QCOMPARE(fk.getSqlForCreateChildTableIndex(db, ""), expectedSql);
  /*
   * Note: by looking at SQLite, MySQL and PgSQL documentation,
   *       it seems that REFERENCES only accepts table name (not databaseName.tableName)
   */
}

void mdtDatabaseTest::sqlTableSchemaTest()
{
  mdtSqlTableSchema ts;
  mdtSqlField field;
  mdtSqlForeignKey fk;

  /*
   * Initial state
   */
  QVERIFY(!ts.isTemporary());
  QCOMPARE(ts.fieldCount(), 0);
  /*
   * Simple set/get
   */
  // Set
  ts.setTemporary(true);
  ts.setTableName("Client_tbl");
  // Add Id_PK field
  field.clear();
  field.setName("Id_PK");
  field.setAutoValue(true);
  ts.addField(field, true);
  // Add Name field
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  ts.addField(field, false);
  // Check
  QVERIFY(ts.isTemporary());
  QCOMPARE(ts.tableName(), QString("Client_tbl"));
  QCOMPARE(ts.fieldCount(), 2);
  QCOMPARE(ts.fieldName(0), QString("Id_PK"));
  QCOMPARE(ts.getFieldNameList().size(), 2);
  QCOMPARE(ts.getFieldNameList().at(0), QString("Id_PK"));
  QCOMPARE(ts.fieldIndex("Id_PK"), 0);
  QCOMPARE(ts.field(0).name(), QString("Id_PK"));
  QCOMPARE(ts.field("Id_PK").name(), QString("Id_PK"));
  QCOMPARE(ts.fieldName(1), QString("Name"));
  QCOMPARE(ts.getFieldNameList().size(), 2);
  QCOMPARE(ts.getFieldNameList().at(1), QString("Name"));
  QCOMPARE(ts.fieldIndex("Id_PK"), 0);
  QCOMPARE(ts.fieldIndex("Name"), 1);
  QCOMPARE(ts.field(1).name(), QString("Name"));
  QCOMPARE(ts.field("Name").name(), QString("Name"));
  // Check is PK
  QVERIFY(ts.isFieldPartOfPrimaryKey(0));
  QVERIFY(!ts.isFieldPartOfPrimaryKey(1));
  /*
   * Setup Address_tbl
   */
  ts.clear();
  ts.setTableName("Address_tbl");
  // Add Id_PK field
  field.clear();
  field.setName("Id_PK");
  field.setAutoValue(true);
  ts.addField(field, true);
  // Add Street field
  field.clear();
  field.setName("Street");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  ts.addField(field, false);
  // Add Client_Id_FK field
  field.clear();
  field.setName("Client_Id_FK");
  field.setType(mdtSqlFieldType::Integer);
  field.setRequired(true);
  ts.addField(field, false);
  fk.clear();
  fk.setParentTableName("Client_tbl");
  fk.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk.setCreateChildIndex(true);
  fk.addKeyFields("Id_PK", field);
  ts.addForeignKey(fk);
  // Check getting foreign key
  fk.clear();
  fk = ts.foreignKeyReferencing("Client_tbl");
  QCOMPARE(fk.parentTableName(), QString("Client_tbl"));
  QCOMPARE(fk.parentTableFields().size(), 1);
  QCOMPARE(fk.parentTableFields().at(0), QString("Id_PK"));
  QCOMPARE(fk.childTableName(), QString("Address_tbl"));
  QCOMPARE(fk.childTableFields().size(), 1);
  QCOMPARE(fk.childTableFields().at(0), QString("Client_Id_FK"));
  QVERIFY(fk.onDeleteAction() == mdtSqlForeignKey::Restrict);
  QVERIFY(fk.onUpdateAction() == mdtSqlForeignKey::Cascade);
  

  /*
   * Clear
   */
  ts.clear();
  QVERIFY(!ts.isTemporary());
  QVERIFY(ts.tableName().isEmpty());
  QCOMPARE(ts.fieldCount(), 0);
  QCOMPARE(ts.getFieldNameList().size(), 0);
  QCOMPARE(ts.fieldIndex("Id_PK"), -1);
}

void mdtDatabaseTest::sqlTableSchemaGetSqlMySqlTest()
{
  mdtSqlTableSchema ts;
  mdtSqlField field;
  mdtSqlIndex index;
  mdtSqlForeignKey fk;
  QString expectedSql;
  QStringList sqlList;
  QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", "sqlTableSchemaGetSqlMySqlTest");

  QCOMPARE(db.driverName(), QString("QMYSQL"));
  /*
   * Empty table schema
   */
  QVERIFY(ts.getSqlForCreateTable(db).isEmpty());
  QVERIFY(ts.getSqlForDropTable(db).isEmpty());
  
  /// \todo Table COLLATE is missing now

  /*
   * Simple Table:
   *  - Database name is specified
   *  - Storage engine is specified
   *  - Collation: nothing is specified
   */
  ts.clear();
  ts.setDatabaseName("sandbox");
  ts.setStorageEngineName("InnoDB");
  ts.setTableName("Client_tbl", "UTF8");
  // Id_PK field
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  ts.addField(field, true);
  // Name field
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  ts.addField(field, false);
  // Check SQL statements to create the table
  sqlList = ts.getSqlForCreateTable(db);
  QCOMPARE(sqlList.size(), 1);
  expectedSql  = "CREATE TABLE `sandbox`.`Client_tbl` (\n";
  expectedSql += "  `Id_PK` INTEGER NOT NULL AUTO_INCREMENT PRIMARY KEY,\n";
  expectedSql += "  `Name` VARCHAR(50) DEFAULT NULL\n";
  expectedSql += ") ENGINE=InnoDB;\n";
  QCOMPARE(sqlList.at(0), expectedSql);
  // Check SQL statements to drop the table
  sqlList = ts.getSqlForDropTable(db);
  QCOMPARE(sqlList.size(), 1);
  expectedSql = "DROP TABLE IF EXISTS `sandbox`.`Client_tbl`;\n";
  QCOMPARE(sqlList.at(0), expectedSql);
  /*
   * Simple Table:
   *  - Database name is not specified
   *  - Storage engine is specified
   *  - Collation: nothing is specified
   */
  ts.clear();
  ts.setStorageEngineName("InnoDB");
  ts.setTableName("Client_tbl", "UTF8");
  // Id_PK field
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  ts.addField(field, true);
  // Name field
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  ts.addField(field, false);
  // Check SQL statements to create the table
  sqlList = ts.getSqlForCreateTable(db);
  QCOMPARE(sqlList.size(), 1);
  expectedSql  = "CREATE TABLE `Client_tbl` (\n";
  expectedSql += "  `Id_PK` INTEGER NOT NULL AUTO_INCREMENT PRIMARY KEY,\n";
  expectedSql += "  `Name` VARCHAR(50) DEFAULT NULL\n";
  expectedSql += ") ENGINE=InnoDB;\n";
  QCOMPARE(sqlList.at(0), expectedSql);
  // Check SQL statements to drop the table
  sqlList = ts.getSqlForDropTable(db);
  QCOMPARE(sqlList.size(), 1);
  expectedSql = "DROP TABLE IF EXISTS `Client_tbl`;\n";
  QCOMPARE(sqlList.at(0), expectedSql);
  /*
   * Table with a multi-field PK
   * Engine is not specified
   */
  ts.clear();
  ts.setTableName("Client_tbl", "UTF8");
  field.clear();
  field.setName("Id_A_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setRequired(true);
  ts.addField(field, true);
  field.clear();
  field.setName("Id_B_PK");
  field.setRequired(true);
  field.setType(mdtSqlFieldType::Integer);
  ts.addField(field, true);
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  ts.addField(field, false);
  // Check SQL statements to create the table
  sqlList = ts.getSqlForCreateTable(db);
  QCOMPARE(sqlList.size(), 1);
  expectedSql  = "CREATE TABLE `Client_tbl` (\n";
  expectedSql += "  `Id_A_PK` INTEGER NOT NULL DEFAULT NULL,\n";
  expectedSql += "  `Id_B_PK` INTEGER NOT NULL DEFAULT NULL,\n";
  expectedSql += "  `Name` VARCHAR(50) DEFAULT NULL,\n";
  expectedSql += "  PRIMARY KEY (`Id_A_PK`,`Id_B_PK`)\n";
  expectedSql += ");\n";
  QCOMPARE(sqlList.at(0), expectedSql);
  // Check SQL statements to drop the table
  sqlList = ts.getSqlForDropTable(db);
  QCOMPARE(sqlList.size(), 1);
  expectedSql = "DROP TABLE IF EXISTS `Client_tbl`;\n";
  QCOMPARE(sqlList.at(0), expectedSql);
  /*
   * Simple Maria DB/MySQL Table:
   *  - Storage engine is specified
   *  - Charset is specified
   */
  /// \todo Adapt and re-enable once COLLATION is defined
//   st.clear();
//   st.setDriverName("QMYSQL");
//   st.setDatabaseName("sandbox");
//   st.setStorageEngineName("InnoDB");
//   st.setTableName("Client_tbl", "UTF8");
//   field.clear();
//   field.setName("Id_PK");
//   field.setType(mdtSqlFieldType::Integer);
//   field.setAutoValue(true);
//   st.addField(field, true);
//   field.clear();
//   field.setName("Name");
//   field.setType(mdtSqlFieldType::Varchar);
//   field.setLength(50);
//   st.addField(field, false);
//   expectedSql  = "CREATE TABLE `sandbox`.`Client_tbl` (\n";
//   expectedSql += "  `Id_PK` INT NOT NULL AUTO_INCREMENT,\n";
//   expectedSql += "  `Name` VARCHAR(50) DEFAULT NULL,\n";
//   expectedSql += "  PRIMARY KEY (`Id_PK`)\n";
//   expectedSql += ") ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;\n";
//   QCOMPARE(st.sqlForCreateTable(), expectedSql);
//   expectedSql = "DROP TABLE IF EXISTS `sandbox`.`Client_tbl`;\n";
//   QCOMPARE(st.sqlForDropTable(), expectedSql);

  /*
   * Table with a single field foreign key - No index explicitly created
   */
  ts.clear();
  ts.setTableName("Address_tbl", "UTF8");
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  ts.addField(field, true);
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  ts.addField(field, false);
  field.clear();
  field.setName("Client_Id_FK");
  field.setType(mdtSqlFieldType::Integer);
  ts.addField(field, false);
  fk.clear();
  fk.setParentTableName("Client_tbl");
  fk.addKeyFields("Id_PK", "Client_Id_FK");
  fk.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  ts.addForeignKey(fk);
  // Check SQL statements to create the table
  sqlList = ts.getSqlForCreateTable(db);
  QCOMPARE(sqlList.size(), 1);
  expectedSql  = "CREATE TABLE `Address_tbl` (\n";
  expectedSql += "  `Id_PK` INTEGER NOT NULL AUTO_INCREMENT PRIMARY KEY,\n";
  expectedSql += "  `Name` VARCHAR(50) DEFAULT NULL,\n";
  expectedSql += "  `Client_Id_FK` INTEGER DEFAULT NULL,\n";
  expectedSql += "  FOREIGN KEY (`Client_Id_FK`)\n";
  expectedSql += "   REFERENCES `Client_tbl` (`Id_PK`)\n";
  expectedSql += "   ON DELETE RESTRICT\n";
  expectedSql += "   ON UPDATE CASCADE\n";
  expectedSql += ");\n";
  QCOMPARE(sqlList.at(0), expectedSql);
  // Check SQL statements to drop the table
  sqlList = ts.getSqlForDropTable(db);
  QCOMPARE(sqlList.size(), 1);
  expectedSql = "DROP TABLE IF EXISTS `Address_tbl`;\n";
  QCOMPARE(sqlList.at(0), expectedSql);

  // Free resources
  db.close();
  db = QSqlDatabase();
  QSqlDatabase::removeDatabase(db.connectionName());
}

void mdtDatabaseTest::sqlTableSchemaGetSqlSqliteTest()
{
  mdtSqlTableSchema ts;
  mdtSqlField field;
  mdtSqlIndex index;
  mdtSqlForeignKey fk;
  QString expectedSql;
  QStringList sqlList;
  QSqlDatabase db = pvDatabase;

  QCOMPARE(db.driverName(), QString("QSQLITE"));
  /*
   * Empty table schema
   */
  QVERIFY(ts.getSqlForCreateTable(db).isEmpty());
  QVERIFY(ts.getSqlForDropTable(db).isEmpty());
  
  /// \todo Table COLLATE is missing now
  
  /*
   * Simple Table:
   *  - Database name is specified
   *  - Storage engine is specified (must be ignored)
   *  - Collation: nothing is specified
   */
  ts.clear();
  ts.setDatabaseName("sandbox");
  ts.setStorageEngineName("InnoDB");
  ts.setTableName("Client_tbl", "UTF8");
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  ts.addField(field, true);
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  ts.addField(field, false);
  // Check SQL statements to create the table
  sqlList = ts.getSqlForCreateTable(db);
  QCOMPARE(sqlList.size(), 1);
  expectedSql  = "CREATE TABLE \"sandbox\".\"Client_tbl\" (\n";
  expectedSql += "  \"Id_PK\" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\n";
  expectedSql += "  \"Name\" VARCHAR(50) DEFAULT NULL\n";
  expectedSql += ");\n";
  QCOMPARE(sqlList.at(0), expectedSql);
  // Check SQL statements to drop the table
  sqlList = ts.getSqlForDropTable(db);
  QCOMPARE(sqlList.size(), 1);
  expectedSql = "DROP TABLE IF EXISTS \"sandbox\".\"Client_tbl\";\n";
  QCOMPARE(sqlList.at(0), expectedSql);
  /*
   * Simple Sqlite Table:
   *  - Storage engine is specified (must be ignored)
   *  - Collation: field Name , not case sensitive
   */
  ts.clear();
  ts.setStorageEngineName("InnoDB");
  ts.setTableName("Client_tbl", "UTF8");
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  ts.addField(field, true);
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  field.setCaseSensitive(false);
  ts.addField(field, false);
  // Check SQL statements to create the table
  sqlList = ts.getSqlForCreateTable(db);
  QCOMPARE(sqlList.size(), 1);
  expectedSql  = "CREATE TABLE \"Client_tbl\" (\n";
  expectedSql += "  \"Id_PK\" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\n";
  expectedSql += "  \"Name\" VARCHAR(50) DEFAULT NULL COLLATE NOCASE\n";
  expectedSql += ");\n";
  QCOMPARE(sqlList.at(0), expectedSql);
  // Check SQL statements to drop the table
  sqlList = ts.getSqlForDropTable(db);
  QCOMPARE(sqlList.size(), 1);
  expectedSql = "DROP TABLE IF EXISTS \"Client_tbl\";\n";
  QCOMPARE(sqlList.at(0), expectedSql);

  /*
   * Simple Sqlite temporary Table:
   *  - Charset is specified
   */
  ts.clear();
  ts.setTableName("Client_tbl", "UTF8");
  ts.setTemporary(true);
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  ts.addField(field, true);
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  field.setCaseSensitive(false);
  ts.addField(field, false);
  // Check SQL statements to create the table
  sqlList = ts.getSqlForCreateTable(db);
  QCOMPARE(sqlList.size(), 1);
  expectedSql  = "CREATE TEMPORARY TABLE \"Client_tbl\" (\n";
  expectedSql += "  \"Id_PK\" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\n";
  expectedSql += "  \"Name\" VARCHAR(50) DEFAULT NULL COLLATE NOCASE\n";
  expectedSql += ");\n";
  QCOMPARE(sqlList.at(0), expectedSql);
  // Check SQL statements to drop the table
  sqlList = ts.getSqlForDropTable(db);
  QCOMPARE(sqlList.size(), 1);
  expectedSql = "DROP TABLE IF EXISTS \"Client_tbl\";\n";
  QCOMPARE(sqlList.at(0), expectedSql);
  /*
   * Simple 1 field table
   * (Added because a bug caused exactly 2 fields table to be generated wrong)
   */
  ts.clear();
  ts.setTableName("Client_tbl", "UTF8");
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  ts.addField(field, true);
  // Check SQL statements to create the table
  sqlList = ts.getSqlForCreateTable(db);
  QCOMPARE(sqlList.size(), 1);
  expectedSql  = "CREATE TABLE \"Client_tbl\" (\n";
  expectedSql += "  \"Id_PK\" INTEGER NOT NULL PRIMARY KEY\n";
  expectedSql += ");\n";
  QCOMPARE(sqlList.at(0), expectedSql);
  /*
   * Simple 2 field table
   * (Added because a bug caused exactly 2 fields table to be generated wrong)
   */
  ts.clear();
  ts.setTableName("Client_tbl", "UTF8");
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  ts.addField(field, true);
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  ts.addField(field, false);
  // Check SQL statements to create the table
  sqlList = ts.getSqlForCreateTable(db);
  QCOMPARE(sqlList.size(), 1);
  expectedSql  = "CREATE TABLE \"Client_tbl\" (\n";
  expectedSql += "  \"Id_PK\" INTEGER NOT NULL PRIMARY KEY,\n";
  expectedSql += "  \"Name\" VARCHAR(50) DEFAULT NULL\n";
  expectedSql += ");\n";
  QCOMPARE(sqlList.at(0), expectedSql);
  /*
   * Simple 3 field table
   * (Added because a bug caused exactly 2 fields table to be generated wrong)
   */
  ts.clear();
  ts.setTableName("Client_tbl", "UTF8");
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  ts.addField(field, true);
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  ts.addField(field, false);
  field.clear();
  field.setName("Remarks");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(100);
  ts.addField(field, false);
  // Check SQL statements to create the table
  sqlList = ts.getSqlForCreateTable(db);
  QCOMPARE(sqlList.size(), 1);
  expectedSql  = "CREATE TABLE \"Client_tbl\" (\n";
  expectedSql += "  \"Id_PK\" INTEGER NOT NULL PRIMARY KEY,\n";
  expectedSql += "  \"Name\" VARCHAR(50) DEFAULT NULL,\n";
  expectedSql += "  \"Remarks\" VARCHAR(100) DEFAULT NULL\n";
  expectedSql += ");\n";
  QCOMPARE(sqlList.at(0), expectedSql);
  /*
   * Simple Sqlite Table:
   *  - Collation: nothing is specified
   *  - A NON UNIQUE index is added
   *  - A UNIQUE index is added
   */
  ts.clear();
  ts.setTableName("Client_tbl", "UTF8");
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  ts.addField(field, true);
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  ts.addField(field, false);
  field.clear();
  field.setName("NameU");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  ts.addField(field, false);
  index.clear();
  index.addField("Name");
  ts.addIndex(index);
  index.clear();
  index.setUnique(true);
  index.addField("NameU");
  ts.addIndex(index);
  // Check SQL statements to create the table
  sqlList = ts.getSqlForCreateTable(db);
  QCOMPARE(sqlList.size(), 3);
  expectedSql  = "CREATE TABLE \"Client_tbl\" (\n";
  expectedSql += "  \"Id_PK\" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\n";
  expectedSql += "  \"Name\" VARCHAR(50) DEFAULT NULL,\n";
  expectedSql += "  \"NameU\" VARCHAR(50) DEFAULT NULL\n";
  expectedSql += ");\n";
  QCOMPARE(sqlList.at(0), expectedSql);
  expectedSql = "CREATE INDEX \"Client_tbl_Name_index\" ON \"Client_tbl\" (\"Name\");\n";
  QCOMPARE(sqlList.at(1), expectedSql);
  expectedSql = "CREATE UNIQUE INDEX \"Client_tbl_NameU_index\" ON \"Client_tbl\" (\"NameU\");\n";
  QCOMPARE(sqlList.at(2), expectedSql);
  // Check SQL statements to drop the table
  sqlList = ts.getSqlForDropTable(db);
  QCOMPARE(sqlList.size(), 3);
  expectedSql = "DROP TABLE IF EXISTS \"Client_tbl\";\n";
  QCOMPARE(sqlList.at(0), expectedSql);
  expectedSql = "DROP INDEX IF EXISTS \"Client_tbl_Name_index\";\n";
  QCOMPARE(sqlList.at(1), expectedSql);
  expectedSql = "DROP INDEX IF EXISTS \"Client_tbl_NameU_index\";\n";
  QCOMPARE(sqlList.at(2), expectedSql);
  /*
   * Table with a multi-field PK
   */
  ts.clear();
  ts.setTableName("Client_tbl", "UTF8");
  field.clear();
  field.setName("Id_A_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setRequired(true);
  ts.addField(field, true);
  field.clear();
  field.setName("Id_B_PK");
  field.setRequired(true);
  field.setType(mdtSqlFieldType::Integer);
  ts.addField(field, true);
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  ts.addField(field, false);
  // Check SQL statements to create the table
  sqlList = ts.getSqlForCreateTable(db);
  QCOMPARE(sqlList.size(), 1);
  expectedSql  = "CREATE TABLE \"Client_tbl\" (\n";
  expectedSql += "  \"Id_A_PK\" INTEGER NOT NULL DEFAULT NULL,\n";
  expectedSql += "  \"Id_B_PK\" INTEGER NOT NULL DEFAULT NULL,\n";
  expectedSql += "  \"Name\" VARCHAR(50) DEFAULT NULL,\n";
  expectedSql += "  PRIMARY KEY (\"Id_A_PK\",\"Id_B_PK\")\n";
  expectedSql += ");\n";
  QCOMPARE(sqlList.at(0), expectedSql);
  // Check SQL statements to drop the table
  sqlList = ts.getSqlForDropTable(db);
  QCOMPARE(sqlList.size(), 1);
  expectedSql = "DROP TABLE IF EXISTS \"Client_tbl\";\n";
  QCOMPARE(sqlList.at(0), expectedSql);
  /*
   * Table without any primary key
   */
  ts.clear();
  ts.setTableName("Client_tbl", "UTF8");
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  ts.addField(field, false);
  // Check SQL statements to create the table
  sqlList = ts.getSqlForCreateTable(db);
  QCOMPARE(sqlList.size(), 1);
  expectedSql  = "CREATE TABLE \"Client_tbl\" (\n";
  expectedSql += "  \"Name\" VARCHAR(50) DEFAULT NULL\n";
  expectedSql += ");\n";
  QCOMPARE(sqlList.at(0), expectedSql);
  // Check SQL statements to drop the table
  sqlList = ts.getSqlForDropTable(db);
  QCOMPARE(sqlList.size(), 1);
  expectedSql = "DROP TABLE IF EXISTS \"Client_tbl\";\n";
  QCOMPARE(sqlList.at(0), expectedSql);
  /*
   * Table with a single field foreign key - No index explicitly created
   */
  ts.clear();
  ts.setTableName("Address_tbl", "UTF8");
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  ts.addField(field, true);
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  ts.addField(field, false);
  field.clear();
  field.setName("Client_Id_FK");
  field.setType(mdtSqlFieldType::Integer);
  ts.addField(field, false);
  fk.clear();
  fk.setParentTableName("Client_tbl");
  fk.addKeyFields("Id_PK", "Client_Id_FK");
  fk.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  ts.addForeignKey(fk);
  // Check SQL statements to create the table
  sqlList = ts.getSqlForCreateTable(db);
  QCOMPARE(sqlList.size(), 1);
  expectedSql  = "CREATE TABLE \"Address_tbl\" (\n";
  expectedSql += "  \"Id_PK\" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\n";
  expectedSql += "  \"Name\" VARCHAR(50) DEFAULT NULL,\n";
  expectedSql += "  \"Client_Id_FK\" INTEGER DEFAULT NULL,\n";
  expectedSql += "  FOREIGN KEY (\"Client_Id_FK\")\n";
  expectedSql += "   REFERENCES \"Client_tbl\" (\"Id_PK\")\n";
  expectedSql += "   ON DELETE RESTRICT\n";
  expectedSql += "   ON UPDATE CASCADE\n";
  expectedSql += ");\n";
  QCOMPARE(sqlList.at(0), expectedSql);
  // Check SQL statements to drop the table
  sqlList = ts.getSqlForDropTable(db);
  QCOMPARE(sqlList.size(), 1);
  expectedSql = "DROP TABLE IF EXISTS \"Address_tbl\";\n";
  QCOMPARE(sqlList.at(0), expectedSql);
  /*
   * Table with a 2 fields foreign key - No index explicitly created
   */
  ts.clear();
  ts.setTableName("Link_tbl", "UTF8");
  // Id_PK field
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  ts.addField(field, true);
  // CNN_A_Id_FK and CNN_B_Id_FK foreign key common setup
  fk.clear();
  fk.setParentTableName("CNN_tbl");
  fk.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  // CNN_A_Id_FK field
  field.clear();
  field.setName("CNN_A_Id_FK");
  field.setType(mdtSqlFieldType::Integer);
  field.setRequired(true);
  ts.addField(field, false);
  fk.addKeyFields("Id_A_PK", field);
  // CNN_B_Id_FK field
  field.clear();
  field.setName("CNN_B_Id_FK");
  field.setType(mdtSqlFieldType::Integer);
  field.setRequired(true);
  ts.addField(field, false);
  fk.addKeyFields("Id_B_PK", field);
  ts.addForeignKey(fk);
  // Check SQL statements to create the table
  sqlList = ts.getSqlForCreateTable(db);
  QCOMPARE(sqlList.size(), 1);
  expectedSql  = "CREATE TABLE \"Link_tbl\" (\n";
  expectedSql += "  \"Id_PK\" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\n";
  expectedSql += "  \"CNN_A_Id_FK\" INTEGER NOT NULL DEFAULT NULL,\n";
  expectedSql += "  \"CNN_B_Id_FK\" INTEGER NOT NULL DEFAULT NULL,\n";
  expectedSql += "  FOREIGN KEY (\"CNN_A_Id_FK\",\"CNN_B_Id_FK\")\n";
  expectedSql += "   REFERENCES \"CNN_tbl\" (\"Id_A_PK\",\"Id_B_PK\")\n";
  expectedSql += "   ON DELETE RESTRICT\n";
  expectedSql += "   ON UPDATE CASCADE\n";
  expectedSql += ");\n";
  QCOMPARE(sqlList.at(0), expectedSql);
  // Check SQL statements to drop the table
  sqlList = ts.getSqlForDropTable(db);
  QCOMPARE(sqlList.size(), 1);
  expectedSql = "DROP TABLE IF EXISTS \"Link_tbl\";\n";
  QCOMPARE(sqlList.at(0), expectedSql);
  /*
   * Table with 2 (1 field) foreign keys - Indexes explicitly created
   */
  ts.clear();
  ts.setTableName("Link_tbl", "UTF8");
  // Id_PK field
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  ts.addField(field, true);
  // UCNN_Id_FK field
  field.clear();
  field.setName("UCNN_Id_FK");
  field.setType(mdtSqlFieldType::Integer);
  field.setRequired(true);
  ts.addField(field, false);
  // UCNN_Id_FK FK
  fk.clear();
  fk.setCreateChildIndex(true);
  fk.setParentTableName("UCNN_tbl");
  fk.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk.addKeyFields("Id_PK", field);
  ts.addForeignKey(fk);
  // ACNN_Id_FK field
  field.clear();
  field.setName("ACNN_Id_FK");
  field.setType(mdtSqlFieldType::Integer);
  field.setRequired(true);
  ts.addField(field, false);
  // ACNN_Id_FK FK
  fk.clear();
  fk.setCreateChildIndex(true);
  fk.setParentTableName("ACNN_tbl");
  fk.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk.addKeyFields("Id_PK", field);
  ts.addForeignKey(fk);
  // Check SQL statements to create the table
  sqlList = ts.getSqlForCreateTable(db);
  QCOMPARE(sqlList.size(), 3);
  expectedSql  = "CREATE TABLE \"Link_tbl\" (\n";
  expectedSql += "  \"Id_PK\" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\n";
  expectedSql += "  \"UCNN_Id_FK\" INTEGER NOT NULL DEFAULT NULL,\n";
  expectedSql += "  \"ACNN_Id_FK\" INTEGER NOT NULL DEFAULT NULL,\n";
  expectedSql += "  FOREIGN KEY (\"UCNN_Id_FK\")\n";
  expectedSql += "   REFERENCES \"UCNN_tbl\" (\"Id_PK\")\n";
  expectedSql += "   ON DELETE RESTRICT\n";
  expectedSql += "   ON UPDATE CASCADE,\n";
  expectedSql += "  FOREIGN KEY (\"ACNN_Id_FK\")\n";
  expectedSql += "   REFERENCES \"ACNN_tbl\" (\"Id_PK\")\n";
  expectedSql += "   ON DELETE RESTRICT\n";
  expectedSql += "   ON UPDATE CASCADE\n";
  expectedSql += ");\n";
  QCOMPARE(sqlList.at(0), expectedSql);
  expectedSql = "CREATE INDEX \"Link_tbl_UCNN_Id_FK_index\" ON \"Link_tbl\" (\"UCNN_Id_FK\");\n";
  QCOMPARE(sqlList.at(1), expectedSql);
  expectedSql = "CREATE INDEX \"Link_tbl_ACNN_Id_FK_index\" ON \"Link_tbl\" (\"ACNN_Id_FK\");\n";
  QCOMPARE(sqlList.at(2), expectedSql);
  // Check SQL statements to drop the table
  sqlList = ts.getSqlForDropTable(db);
  QCOMPARE(sqlList.size(), 3);
  expectedSql = "DROP TABLE IF EXISTS \"Link_tbl\";\n";
  QCOMPARE(sqlList.at(0), expectedSql);
  expectedSql = "DROP INDEX IF EXISTS \"Link_tbl_UCNN_Id_FK_index\";\n";
  QCOMPARE(sqlList.at(1), expectedSql);
  expectedSql = "DROP INDEX IF EXISTS \"Link_tbl_ACNN_Id_FK_index\";\n";
  QCOMPARE(sqlList.at(2), expectedSql);
}

void mdtDatabaseTest::sqlTableSchemaCreateSqliteTest()
{
  QTemporaryFile tempFile;
  QFileInfo dbFileInfo;
  QSqlDatabase db;
  QString sql;
  mdtSqlTableSchema ts;
  QSqlRecord record;
  mdtSqlField field;
  mdtSqlIndex index;
  mdtSqlForeignKey fk;
  mdtSqlPrimaryKey pk;

  /*
   * Create a temp file that will be used for database
   */
  QVERIFY(tempFile.open());
  tempFile.close();
  dbFileInfo.setFile(tempFile.fileName() + ".db");

  // Open database
  db = QSqlDatabase::addDatabase("QSQLITE", "sqlSchemaTableSqliteTest");
  db.setDatabaseName(dbFileInfo.filePath());
  QVERIFY(db.open());

  // QSqlQuery must be created after db.open() was called.
  QSqlQuery q(db);

  /*
   * Enable foreing keys support
   */
  sql = "PRAGMA foreign_keys = ON";
  QVERIFY(q.exec(sql));

  /*
   * Create Client table
   */
  ts.clear();
  ts.setTableName("Client_tbl", "UTF8");
  field.clear();
  // Id_PK field
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  ts.addField(field, true);
  field.clear();
  // Name field
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  ts.addField(field, false);
  index.clear();
  index.addField("Name");
  index.setUnique(true);
  ts.addIndex(index);
  // Create table
  for(const auto & sql : ts.getSqlForDropTable(db)){
    QVERIFY(q.exec(sql));
  }
  for(const auto & sql : ts.getSqlForCreateTable(db)){
    QVERIFY(q.exec(sql));
  }
  // Check that table was correctly created
  QVERIFY(db.tables().contains("Client_tbl"));
  record = db.record("Client_tbl");
  QCOMPARE(record.count(), 2);
  field.setupFromQSqlField(record.field(0), mdtSqlDriverType::SQLite);
  QCOMPARE(field.name(), QString("Id_PK"));
  QVERIFY(field.type() == mdtSqlFieldType::Integer);
  QVERIFY(field.isAutoValue());
  field.setupFromQSqlField(record.field(1), mdtSqlDriverType::SQLite);
  QCOMPARE(field.name(), QString("Name"));
  QVERIFY(field.type() == mdtSqlFieldType::Varchar);
  // Check that index was created
  sql = "PRAGMA index_list(Client_tbl);";
  QVERIFY(q.exec(sql));
  QVERIFY(q.next());
  record = q.record();
  QCOMPARE(record.value("name"), QVariant("Client_tbl_Name_index"));
  QCOMPARE(record.value("unique"), QVariant(1));
  /*
   * Create second table that is child of Client_tbl
   */
  ts.clear();
  ts.setTableName("Address_tbl", "UTF8");
  // Id_PK field
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  ts.addField(field, true);
  // Street field
  field.clear();
  field.setName("Street");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  ts.addField(field, false);
  // Client_Id_FK field
  field.clear();
  field.setName("Client_Id_FK");
  field.setType(mdtSqlFieldType::Integer);
  field.setRequired(true);
  ts.addField(field, false);
  // Client_Id_FK
  fk.clear();
  fk.setParentTableName("Client_tbl");
  fk.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk.setCreateChildIndex(true);
  fk.addKeyFields("Id_PK", field);
  ts.addForeignKey(fk);
  // Create table
  for(const auto & sql : ts.getSqlForDropTable(db)){
    QVERIFY(q.exec(sql));
  }
  for(const auto & sql : ts.getSqlForCreateTable(db)){
    QVERIFY(q.exec(sql));
  }
  // Check that table was correctly created
  QVERIFY(db.tables().contains("Address_tbl"));
  record = db.record("Address_tbl");
  QCOMPARE(record.count(), 3);
  field.setupFromQSqlField(record.field(0), mdtSqlDriverType::SQLite);
  QCOMPARE(field.name(), QString("Id_PK"));
  QVERIFY(field.type() == mdtSqlFieldType::Integer);
  QVERIFY(field.isAutoValue());
  field.setupFromQSqlField(record.field(1), mdtSqlDriverType::SQLite);
  QCOMPARE(field.name(), QString("Street"));
  QVERIFY(field.type() == mdtSqlFieldType::Varchar);
  field.setupFromQSqlField(record.field(2), mdtSqlDriverType::SQLite);
  QCOMPARE(field.name(), QString("Client_Id_FK"));
  QVERIFY(field.type() == mdtSqlFieldType::Integer);
  // Check that index was created
  sql = "PRAGMA index_list(Address_tbl);";
  QVERIFY(q.exec(sql));
  QVERIFY(q.next());
  record = q.record();
  QCOMPARE(record.value("name"), QVariant("Address_tbl_Client_Id_FK_index"));
  QCOMPARE(record.value("unique"), QVariant(0));
  /*
   * Try to insert some data
   */
  sql = "INSERT INTO Client_tbl (Name) VALUES ('Name 1')";
  QVERIFY(q.exec(sql));
  sql = "INSERT INTO Client_tbl (Id_PK, Name) VALUES (2, 'Name 2')";
  QVERIFY(q.exec(sql));
  sql = "INSERT INTO Address_tbl (Street, Client_Id_FK) VALUES ('Street 1', 2)";
  QVERIFY(q.exec(sql));
  sql = "INSERT INTO Address_tbl (Street, Client_Id_FK) VALUES ('Street 2', 3)";
  QVERIFY(!q.exec(sql));
  /*
   * Check table schema setup from database for Client_tbl
   */
  QVERIFY(ts.setupFromTable("Client_tbl", db));
  QCOMPARE(ts.tableName(), QString("Client_tbl"));
  QVERIFY(!ts.isTemporary());
  QCOMPARE(ts.fieldCount(), 2);
  // Check Id_PK field
  field = ts.field(0);
  QCOMPARE(field.name(), QString("Id_PK"));
  QVERIFY(field.type() == mdtSqlFieldType::Integer);
  QVERIFY(field.isAutoValue());
  QVERIFY(field.defaultValue().isNull());
  QCOMPARE(field.length(), -1);
  // Check Street field
  field = ts.field(1);
  QCOMPARE(field.name(), QString("Name"));
  QVERIFY(field.type() == mdtSqlFieldType::Varchar);
  QVERIFY(!field.isAutoValue());
  QCOMPARE(field.defaultValue(), QVariant("NULL"));
  QVERIFY(!field.isRequired());
  QCOMPARE(field.length(), 50);
  // Check primary key
  pk = ts.primaryKey();
  QCOMPARE(pk.fieldCount(), 1);
  QCOMPARE(pk.fieldName(0), QString("Id_PK"));
  ///QVERIFY(pk.isUnique());
  ///QCOMPARE(pk.name(), QString("Client_tbl_Name_index"));
  // Check indexes
  QCOMPARE(ts.indexList().size(), 1);
  index = ts.indexList().at(0);
  QCOMPARE(index.fieldCount(), 1);
  QCOMPARE(index.fieldName(0), QString("Name"));
  QVERIFY(index.isUnique());
  QCOMPARE(index.tableName(), QString("Client_tbl"));
  QCOMPARE(index.name(), QString("Client_tbl_Name_index"));
  // Check foreign keys
  QCOMPARE(ts.foreignKeyList().size(), 0);
  /*
   * Check table schema setup from database for Address_tbl
   */
  QVERIFY(ts.setupFromTable("Address_tbl", db));
  QCOMPARE(ts.tableName(), QString("Address_tbl"));
  QVERIFY(!ts.isTemporary());
  QCOMPARE(ts.fieldCount(), 3);
  // Check Id_PK field
  field = ts.field(0);
  QCOMPARE(field.name(), QString("Id_PK"));
  QVERIFY(field.type() == mdtSqlFieldType::Integer);
  QVERIFY(field.isAutoValue());
  QVERIFY(field.defaultValue().isNull());
  QCOMPARE(field.length(), -1);
  // Check Street field
  field = ts.field(1);
  QCOMPARE(field.name(), QString("Street"));
  QVERIFY(field.type() == mdtSqlFieldType::Varchar);
  QVERIFY(!field.isAutoValue());
  QCOMPARE(field.defaultValue(), QVariant("NULL"));
  QVERIFY(!field.isRequired());
  QCOMPARE(field.length(), 50);
  // Check Client_Id_FK field
  field = ts.field(2);
  QCOMPARE(field.name(), QString("Client_Id_FK"));
  QVERIFY(field.type() == mdtSqlFieldType::Integer);
  QVERIFY(!field.isAutoValue());
  QCOMPARE(field.defaultValue(), QVariant("NULL"));
  QVERIFY(field.isRequired());
  QCOMPARE(field.length(), -1);
  // Check primary key
  pk = ts.primaryKey();
  QCOMPARE(pk.fieldCount(), 1);
  QCOMPARE(pk.fieldName(0), QString("Id_PK"));
  // Check indexes
  QCOMPARE(ts.indexList().size(), 1);
  index = ts.indexList().at(0);
  QCOMPARE(index.fieldCount(), 1);
  QCOMPARE(index.fieldName(0), QString("Client_Id_FK"));
  QVERIFY(!index.isUnique());
  QCOMPARE(index.tableName(), QString("Address_tbl"));
  QCOMPARE(index.name(), QString("Address_tbl_Client_Id_FK_index"));
  // Check foreign keys
  QCOMPARE(ts.foreignKeyList().size(), 1);
  fk = ts.foreignKeyList().at(0);
  QCOMPARE(fk.parentTableName(), QString("Client_tbl"));
  QCOMPARE(fk.childTableName(), QString("Address_tbl"));
  QCOMPARE(fk.childTableFields().size(), 1);
  QCOMPARE(fk.childTableFields().at(0), QString("Client_Id_FK"));
  QVERIFY(fk.onDeleteAction() == mdtSqlForeignKey::Restrict);
  QVERIFY(fk.onUpdateAction() == mdtSqlForeignKey::Cascade);
  /*
   * Create Client2_tbl witch has the same structure than Client_tbl
   */
  ts.clear();
  QCOMPARE(ts.fieldCount(), 0);
  QVERIFY(ts.setupFromTable("Client_tbl", db));
  ts.setTableName("Client2_tbl");
  ts.setTemporary(true);
  QCOMPARE(ts.field("Id_PK").name(), QString("Id_PK"));
  QVERIFY(ts.field("Id_PK").type() == mdtSqlFieldType::Integer);
  QCOMPARE(ts.field("Name").name(), QString("Name"));
  QVERIFY(ts.field("Name").type() == mdtSqlFieldType::Varchar);
  // Create table
  for(const auto & sql : ts.getSqlForCreateTable(db)){
    QVERIFY(q.exec(sql));
  }
  // Check that table was correctly created
  QVERIFY(db.tables().contains("Client2_tbl"));
  record = db.record("Client2_tbl");
  QCOMPARE(record.count(), 2);
  field.setupFromQSqlField(record.field(0), mdtSqlDriverType::SQLite);
  QCOMPARE(field.name(), QString("Id_PK"));
  QVERIFY(field.type() == mdtSqlFieldType::Integer);
  QVERIFY(field.isAutoValue());
  field.setupFromQSqlField(record.field(1), mdtSqlDriverType::SQLite);
  QCOMPARE(field.name(), QString("Name"));
  QVERIFY(field.type() == mdtSqlFieldType::Varchar);
  /*
   * Create Address2_tbl witch has the same structure than Address_tbl
   */
  QVERIFY(ts.setupFromTable("Address_tbl", db));
  ts.setTableName("Address2_tbl");
  ts.setTemporary(true);
  // We have to update FK's parent table
  QVERIFY(ts.foreignKeyList().size() == 1);
  fk = ts.foreignKeyList().at(0);
  fk.setParentTableName("Client2_tbl");
  ts.setForeignKeyAt(0, fk);
  QCOMPARE(ts.field("Id_PK").name(), QString("Id_PK"));
  QVERIFY(ts.field("Id_PK").type() == mdtSqlFieldType::Integer);
  QCOMPARE(ts.field("Street").name(), QString("Street"));
  QVERIFY(ts.field("Street").type() == mdtSqlFieldType::Varchar);
  QCOMPARE(ts.field("Client_Id_FK").name(), QString("Client_Id_FK"));
  QVERIFY(ts.field("Client_Id_FK").type() == mdtSqlFieldType::Integer);
  // Create table
  for(const auto & sql : ts.getSqlForCreateTable(db)){
    QVERIFY(q.exec(sql));
  }
  // Check that table was correctly created
  QVERIFY(db.tables().contains("Address2_tbl"));
  record = db.record("Address2_tbl");
  QCOMPARE(record.count(), 3);
  field.setupFromQSqlField(record.field(0), mdtSqlDriverType::SQLite);
  QCOMPARE(field.name(), QString("Id_PK"));
  QVERIFY(field.type() == mdtSqlFieldType::Integer);
  QVERIFY(field.isAutoValue());
  field.setupFromQSqlField(record.field(1), mdtSqlDriverType::SQLite);
  QCOMPARE(field.name(), QString("Street"));
  QVERIFY(field.type() == mdtSqlFieldType::Varchar);
  field.setupFromQSqlField(record.field(2), mdtSqlDriverType::SQLite);
  QCOMPARE(field.name(), QString("Client_Id_FK"));
  QVERIFY(field.type() == mdtSqlFieldType::Integer);
  // Try to insert some data
  sql = "INSERT INTO Client2_tbl (Name) VALUES ('Name 1')";
  QVERIFY(q.exec(sql));
  sql = "INSERT INTO Client2_tbl (Id_PK, Name) VALUES (2, 'Name 2')";
  QVERIFY(q.exec(sql));
  sql = "INSERT INTO Address2_tbl (Street, Client_Id_FK) VALUES ('Street 1', 2)";
  QVERIFY(q.exec(sql));
  sql = "INSERT INTO Address2_tbl (Street, Client_Id_FK) VALUES ('Street 2', 3)";
  QVERIFY(!q.exec(sql));
  /*
   * Create a table to check various field types
   */
  ts.clear();
  ts.setTableName("DataTypeTest_tbl", "UTF8");
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  ts.addField(field, true);
  // String type
  field.clear();
  field.setName("StringType");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  ts.addField(field, false);
  // Int type
  field.clear();
  field.setName("IntType");
  field.setType(mdtSqlFieldType::Integer);
  ts.addField(field, false);
  // Double type
  field.clear();
  field.setName("DoubleType");
  field.setType(mdtSqlFieldType::Double);
  ts.addField(field, false);
  // Date type
  field.clear();
  field.setName("DateType");
  field.setType(mdtSqlFieldType::Date);
  ts.addField(field, false);
  // Time type
  field.clear();
  field.setName("TimeType");
  field.setType(mdtSqlFieldType::Time);
  ts.addField(field, false);
  // DateTime type
  field.clear();
  field.setName("DateTimeType");
  field.setType(mdtSqlFieldType::DateTime);
  ts.addField(field, false);
  // Create table
  for(const auto & sql : ts.getSqlForCreateTable(db)){
    QVERIFY(q.exec(sql));
  }
  // Check that table was correctly created
  QVERIFY(db.tables().contains("DataTypeTest_tbl"));
  record = db.record("DataTypeTest_tbl");
  QCOMPARE(record.count(), 7);
  field.setupFromQSqlField(record.field(0), mdtSqlDriverType::SQLite);
  QCOMPARE(field.name(), QString("Id_PK"));
  QVERIFY(field.type() == mdtSqlFieldType::Integer);
  QVERIFY(field.isAutoValue());
  field.setupFromQSqlField(record.field(1), mdtSqlDriverType::SQLite);
  QCOMPARE(field.name(), QString("StringType"));
  QVERIFY(field.type() == mdtSqlFieldType::Varchar);
  field.setupFromQSqlField(record.field(2), mdtSqlDriverType::SQLite);
  QCOMPARE(field.name(), QString("IntType"));
  QVERIFY(field.type() == mdtSqlFieldType::Integer);
  field.setupFromQSqlField(record.field(3), mdtSqlDriverType::SQLite);
  QCOMPARE(field.name(), QString("DoubleType"));
  QVERIFY(field.type() == mdtSqlFieldType::Double);
  field.setupFromQSqlField(record.field(4), mdtSqlDriverType::SQLite);
  // Now check that mdtSqlSchemaTable fetches existing table correctly
  QVERIFY(ts.setupFromTable("DataTypeTest_tbl", db));
  field = ts.field("Id_PK");
  QCOMPARE(field.name(), QString("Id_PK"));
  QVERIFY(field.type() == mdtSqlFieldType::Integer);
  QVERIFY(field.isAutoValue());
  QVERIFY(field.isRequired());
  field = ts.field("StringType");
  QCOMPARE(field.name(), QString("StringType"));
  QVERIFY(field.type() == mdtSqlFieldType::Varchar);
  QVERIFY(!field.isAutoValue());
  QVERIFY(!field.isRequired());
  field = ts.field("IntType");
  QCOMPARE(field.name(), QString("IntType"));
  QVERIFY(field.type() == mdtSqlFieldType::Integer);
  QVERIFY(!field.isAutoValue());
  QVERIFY(!field.isRequired());
  field = ts.field("DoubleType");
  QCOMPARE(field.name(), QString("DoubleType"));
  QVERIFY(field.type() == mdtSqlFieldType::Double);
  QVERIFY(!field.isAutoValue());
  QVERIFY(!field.isRequired());
  field = ts.field("DateType");
  QCOMPARE(field.name(), QString("DateType"));
  QVERIFY(field.type() == mdtSqlFieldType::Date);
  QVERIFY(!field.isAutoValue());
  QVERIFY(!field.isRequired());
  field = ts.field("TimeType");
  QCOMPARE(field.name(), QString("TimeType"));
  QVERIFY(field.type() == mdtSqlFieldType::Time);
  QVERIFY(!field.isAutoValue());
  QVERIFY(!field.isRequired());
  field = ts.field("DateTimeType");
  QCOMPARE(field.name(), QString("DateTimeType"));
  QVERIFY(field.type() == mdtSqlFieldType::DateTime);
  QVERIFY(!field.isAutoValue());
  QVERIFY(!field.isRequired());
  // Drop table
  for(const auto & sql : ts.getSqlForDropTable(db)){
    QVERIFY(q.exec(sql));
  }
  QVERIFY(!db.tables().contains("DataTypeTest_tbl"));
  /*
   * Create a table with a 2 fields foreign key
   */
  ts.clear();
  ts.setTableName("Link_tbl", "UTF8");
  // Id_PK field
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  ts.addField(field, true);
  // CNN_A_Id_FK and CNN_B_Id_FK foreign key common setup
  fk.clear();
  fk.setCreateChildIndex(true);
  fk.setParentTableName("CNN_tbl");
  fk.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  // CNN_A_Id_FK field
  field.clear();
  field.setName("CNN_A_Id_FK");
  field.setType(mdtSqlFieldType::Integer);
  field.setRequired(true);
  ts.addField(field, false);
  fk.addKeyFields("Id_A_PK", field);
  // CNN_B_Id_FK field
  field.clear();
  field.setName("CNN_B_Id_FK");
  field.setType(mdtSqlFieldType::Integer);
  field.setRequired(true);
  ts.addField(field, false);
  fk.addKeyFields("Id_B_PK", field);
  ts.addForeignKey(fk);
  // Create table
  for(const auto & sql : ts.getSqlForCreateTable(db)){
    QVERIFY(q.exec(sql));
  }
  // Check that getting table setup from DB works
  QVERIFY(ts.setupFromTable("Link_tbl", db));
  QCOMPARE(ts.tableName(), QString("Link_tbl"));
  QVERIFY(!ts.isTemporary());
  QCOMPARE(ts.fieldCount(), 3);
  // Check Id_PK field
  field = ts.field(0);
  QCOMPARE(field.name(), QString("Id_PK"));
  QVERIFY(field.type() == mdtSqlFieldType::Integer);
  QVERIFY(field.isAutoValue());
  QVERIFY(field.defaultValue().isNull());
  QCOMPARE(field.length(), -1);
  // Check CNN_A_Id_FK field
  field = ts.field(1);
  QCOMPARE(field.name(), QString("CNN_A_Id_FK"));
  QVERIFY(field.type() == mdtSqlFieldType::Integer);
  QVERIFY(!field.isAutoValue());
  QCOMPARE(field.defaultValue(), QVariant("NULL"));
  QVERIFY(field.isRequired());
  QCOMPARE(field.length(), -1);
  // Check CNN_B_Id_FK field
  field = ts.field(2);
  QCOMPARE(field.name(), QString("CNN_B_Id_FK"));
  QVERIFY(field.type() == mdtSqlFieldType::Integer);
  QVERIFY(!field.isAutoValue());
  QCOMPARE(field.defaultValue(), QVariant("NULL"));
  QVERIFY(field.isRequired());
  QCOMPARE(field.length(), -1);
  // Check primary key
  pk = ts.primaryKey();
  QCOMPARE(pk.fieldCount(), 1);
  QCOMPARE(pk.fieldName(0), QString("Id_PK"));
  // Check indexes
  QCOMPARE(ts.indexList().size(), 1);
  index = ts.indexList().at(0);
  QCOMPARE(index.fieldCount(), 2);
  QCOMPARE(index.fieldName(0), QString("CNN_A_Id_FK"));
  QCOMPARE(index.fieldName(1), QString("CNN_B_Id_FK"));
  QVERIFY(!index.isUnique());
  QCOMPARE(index.tableName(), QString("Link_tbl"));
  QCOMPARE(index.name(), QString("Link_tbl_CNN_A_Id_FK_CNN_B_Id_FK_index"));
  // Check foreign keys
  QCOMPARE(ts.foreignKeyList().size(), 1);
  fk = ts.foreignKeyList().at(0);
  QCOMPARE(fk.parentTableName(), QString("CNN_tbl"));
  QCOMPARE(fk.childTableName(), QString("Link_tbl"));
  QCOMPARE(fk.childTableFields().size(), 2);
  QCOMPARE(fk.childTableFields().at(0), QString("CNN_A_Id_FK"));
  QCOMPARE(fk.childTableFields().at(1), QString("CNN_B_Id_FK"));
  QVERIFY(fk.onDeleteAction() == mdtSqlForeignKey::Restrict);
  QVERIFY(fk.onUpdateAction() == mdtSqlForeignKey::Cascade);
  for(const auto & sql : ts.getSqlForDropTable(db)){
    QVERIFY(q.exec(sql));
  }
  QVERIFY(!db.tables().contains("Link_tbl"));

  // Free resources
  db.close();
  db = QSqlDatabase();
  QSqlDatabase::removeDatabase(db.connectionName());
}

void mdtDatabaseTest::sqlTableSchemaModelTest()
{
  mdtSqlTableSchemaModel model;
  mdtSqlTableSchema st;
  QTableView tableView;
  QTreeView treeView;
  QComboBox cb;
  mdtSqlField field;

  /*
   * Setup fields
   */
  // Id_PK
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  st.addField(field, true);
  // Name
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(100);
  st.addField(field, false);
  /*
   * Setup model and views
   */
  model.setTableSchema(st, mdtSqlDriverType::SQLite);
  tableView.setModel(&model);
  tableView.resize(400, 200);
  tableView.show();
  treeView.setModel(&model);
  treeView.show();
  cb.setModel(&model);
  cb.setModelColumn(0);
  cb.setInsertPolicy(QComboBox::InsertAtCurrent);
  cb.setEditable(true);
  cb.show();
  /*
   * Check getting field
   */
  QVERIFY(model.field(-1).isNull());
  QVERIFY(!model.field(0).isNull());
  QCOMPARE(model.field(0).name(), QString("Id_PK"));
  QVERIFY(model.isPartOfPrimaryKey(0));
  QVERIFY(!model.field(1).isNull());
  QCOMPARE(model.field(1).name(), QString("Name"));
  QVERIFY(!model.isPartOfPrimaryKey(1));
  QVERIFY(model.field(20).isNull());
  QVERIFY(!model.isPartOfPrimaryKey(20));
  // Check with combo box
  cb.setCurrentIndex(0);
  QVERIFY(!model.currentField(&cb).isNull());
  QCOMPARE(model.currentField(&cb).name(), QString("Id_PK"));
  cb.setCurrentIndex(1);
  QVERIFY(!model.currentField(&cb).isNull());
  QCOMPARE(model.currentField(&cb).name(), QString("Name"));
  cb.setCurrentIndex(-1);
  QVERIFY(model.currentField(&cb).isNull());

  /*
   * Play
   */
  
  while(tableView.isVisible()){
    QTest::qWait(500);
  }
}

void mdtDatabaseTest::sqlTableSetupWidgetTest()
{
  mdtSqlTableSetupWidget tsw;

  tsw.show();

  /*
   * Play
   */
  
  while(tsw.isVisible()){
    QTest::qWait(500);
  }

}

void mdtDatabaseTest::sqlFieldSetupWidgetTest()
{
  mdtSqlFieldSetupWidget w(nullptr);
  mdtSqlFieldSetupData data;

  /*
   * Reverse Client_tbl
   */
  QVERIFY(w.setTable("Client_tbl", pvDatabase));
  w.show();
  /*
   * Check displaying attributes in edition mode
   */
  w.setEditionMode(mdtSqlFieldSetupEditionMode::Edition);
  // Select Id_PK and check
  w.setField(0);
  QCOMPARE(w.cbField->currentText(), QString("Id_PK"));
  QVERIFY(w.cbType->currentData() == mdtSqlFieldType::Integer);
  QVERIFY(w.cbPartOfPk->isChecked());
  QVERIFY(w.cbAutoIncrement->isChecked());
  // Select FirstName and check
  w.setField(1);
  QCOMPARE(w.cbField->currentText(), QString("FirstName"));
  QVERIFY(w.cbType->currentData() == mdtSqlFieldType::Varchar);
  QVERIFY(!w.cbPartOfPk->isChecked());
  QVERIFY(!w.cbAutoIncrement->isChecked());
  QVERIFY(w.cbRequired->isChecked());
  // Select Remarks and check
  w.setField(2);
  QCOMPARE(w.cbField->currentText(), QString("Remarks"));
  QVERIFY(w.cbType->currentData() == mdtSqlFieldType::Varchar);
  QVERIFY(!w.cbPartOfPk->isChecked());
  QVERIFY(!w.cbAutoIncrement->isChecked());
  QVERIFY(!w.cbRequired->isChecked());
  /*
   * Check displaying attributes in selection mode
   */
  w.setEditionMode(mdtSqlFieldSetupEditionMode::Selection);
  // Select Id_PK and check
  w.setField(0);
  QCOMPARE(w.cbField->currentText(), QString("Id_PK"));
  QVERIFY(w.cbType->currentData() == mdtSqlFieldType::Integer);
  QVERIFY(w.cbPartOfPk->isChecked());
  QVERIFY(w.cbAutoIncrement->isChecked());
  // Select FirstName and check
  w.setField(1);
  QCOMPARE(w.cbField->currentText(), QString("FirstName"));
  QVERIFY(w.cbType->currentData() == mdtSqlFieldType::Varchar);
  QVERIFY(!w.cbPartOfPk->isChecked());
  QVERIFY(!w.cbAutoIncrement->isChecked());
  QVERIFY(w.cbRequired->isChecked());
  // Select Remarks and check
  w.setField(2);
  QCOMPARE(w.cbField->currentText(), QString("Remarks"));
  QVERIFY(w.cbType->currentData() == mdtSqlFieldType::Varchar);
  QVERIFY(!w.cbPartOfPk->isChecked());
  QVERIFY(!w.cbAutoIncrement->isChecked());
  QVERIFY(!w.cbRequired->isChecked());

  /*
   * Play
   */
  while(w.isVisible()){
    QTest::qWait(500);
  }

  // Check that we didn't close the default database
  QVERIFY(pvDatabase.isOpen());
}

void mdtDatabaseTest::sqlFieldSetupDialogTest()
{
  mdtSqlFieldSetupDialog dialog;

  
  QVERIFY(dialog.setTable("Client_tbl", pvDatabase));
  dialog.setEditionMode(mdtSqlFieldSetupEditionMode::Edition);
  
  /*
   * Play
   */
  dialog.exec();
}

void mdtDatabaseTest::sqlViewSchemaFieldTest()
{
  using namespace mdtSqlViewSchema;
  SelectField field1;

  QCOMPARE(pvDatabase.driverName(), QString("QSQLITE"));
  /*
   * Construction + set/get
   */
  SelectField field2("Id_PK");
  QCOMPARE(field2.fieldName(), QString("Id_PK"));
  SelectField field3("Id_PK", "Client ID");
  QCOMPARE(field3.fieldName(), QString("Id_PK"));
  QCOMPARE(field3.alias(), QString("Client ID"));
  field3.setFieldName("Id_A");
  field3.setAlias("A");
  QCOMPARE(field3.fieldName(), QString("Id_A"));
  QCOMPARE(field3.alias(), QString("A"));
  /*
   * Set/get
   */
  field1.setFieldName("Id_PK");
  field1.setAlias("Id");
  field1.setTable("TBL");
  QCOMPARE(field1.fieldName(), QString("Id_PK"));
  QCOMPARE(field1.alias(), QString("Id"));
  QCOMPARE(field1.table(), QString("TBL"));
  /*
   * Clear
   */
  field1.clear();
  QVERIFY(field1.fieldName().isEmpty());
  QVERIFY(field1.alias().isEmpty());
  QVERIFY(field1.table().isEmpty());
  /*
   * SQL
   */
  // Empty field
  QVERIFY(field1.getSql(pvDatabase.driver()).isEmpty());
  // Field with only field name
  field1.setFieldName("Id_PK");
  QCOMPARE(field1.getSql(pvDatabase.driver()), QString("\"Id_PK\""));
  // Field with field name and table name/alias
  field1.setTable("CLI");
  QCOMPARE(field1.getSql(pvDatabase.driver()), QString("\"CLI\".\"Id_PK\""));
  // Field with field name and alias
  field1.clear();
  field1.setFieldName("Id_PK");
  field1.setAlias("Client ID");
  QCOMPARE(field1.getSql(pvDatabase.driver()), QString("\"Id_PK\" AS \"Client ID\""));
  // Field with field name and alias + table name/alias
  field1.setTable("CLI");
  QCOMPARE(field1.getSql(pvDatabase.driver()), QString("\"CLI\".\"Id_PK\" AS \"Client ID\""));
  // Check with table name/alias + * as field name
  field1.clear();
  field1.setTable("CLI");
  field1.setFieldName("*");
  QCOMPARE(field1.getSql(pvDatabase.driver()), QString("\"CLI\".*"));
}

void mdtDatabaseTest::sqlViewSchemaTableTest()
{
  using namespace mdtSqlViewSchema;
  Table client1;

  QCOMPARE(pvDatabase.driverName(), QString("QSQLITE"));
  /*
   * Construction
   */
  Table client2("Client_tbl");
  QCOMPARE(client2.tableName(), QString("Client_tbl"));
  Table client3("Client_tbl", "CLI");
  QCOMPARE(client3.tableName(), QString("Client_tbl"));
  QCOMPARE(client3.alias(), QString("CLI"));
  /*
   * Set/get
   */
  client1.setTableName("TBL");
  QCOMPARE(client1.tableName(), QString("TBL"));
  QCOMPARE(client1.aliasOrTableName(), QString("TBL"));
  client1.setAlias("Table");
  QCOMPARE(client1.aliasOrTableName(), QString("Table"));
  QCOMPARE(client1.alias(), QString("Table"));
  /*
   * Clear
   */
  client1.clear();
  QVERIFY(client1.tableName().isEmpty());
  QVERIFY(client1.alias().isEmpty());
}

void mdtDatabaseTest::sqlViewSchemaJoinKeyTest()
{
  using namespace mdtSqlViewSchema;
  JoinKey key;

  /*
   * Initial state
   */
  QVERIFY(key.constraintOperator() == JoinKey::On);
  QVERIFY(key.fieldOperator() == JoinKey::Equal);
  /*
   * Set/get
   */
  key.setMainTableField("Id_PK");
  key.setTableToJoinField("Client_Id_FK");
  key.setConstraintOperator(JoinKey::And);
  key.setFieldOperator(JoinKey::MtfLessThanTdjf);
  QCOMPARE(key.mainTableField(), QString("Id_PK"));
  QCOMPARE(key.tableToJoinField(), QString("Client_Id_FK"));
  QVERIFY(key.constraintOperator() == JoinKey::And);
  QCOMPARE(key.constraintOperatorStr(), QString("AND"));
  QVERIFY(key.fieldOperator() == JoinKey::MtfLessThanTdjf);
  QCOMPARE(key.fieldOperatorStr(), QString("<"));
  key.setConstraintOperator(JoinKey::And);
  /*
   * Clear
   */
  key.clear();
  QVERIFY(key.mainTableField().isEmpty());
  QVERIFY(key.tableToJoinField().isEmpty());
  QVERIFY(key.constraintOperator() == JoinKey::On);
  QCOMPARE(key.constraintOperatorStr(), QString("ON"));
  QVERIFY(key.fieldOperator() == JoinKey::Equal);
  QCOMPARE(key.fieldOperatorStr(), QString("="));
}

void mdtDatabaseTest::sqlViewSchemaJoinClauseTest()
{
  using namespace mdtSqlViewSchema;
  QString expectedSql;
  Table client;
  Table address;
  JoinKey key;
  JoinClause join;

  QCOMPARE(pvDatabase.driverName(), QString("QSQLITE"));
  // Empty join clause
  /*
   * Initial state
   */
  QVERIFY(join.isNull());
  QCOMPARE(join.getSql(pvDatabase.driver()), QString());
  /*
   * Simple JOIN
   */
  // Setup join clause
  client.setTableName("Client_tbl");
  client.setAlias("CLI");
  address.setTableName("Address_tbl");
  address.setAlias("ADR");
  join.setMainTable(client);
  join.setTableToJoin(address);
  QVERIFY(!join.isNull());
  key.setMainTableField("Id_PK");
  key.setTableToJoinField("Client_Id_FK");
  join.addKey(key);
  // Check SQL
  expectedSql  = " JOIN \"Address_tbl\" \"ADR\"\n";
  expectedSql += "  ON \"CLI\".\"Id_PK\" = \"ADR\".\"Client_Id_FK\"";
  QCOMPARE(join.getSql(pvDatabase.driver()), expectedSql);
  /*
   * Clear
   */
  join.clear();
  QVERIFY(join.isNull());
  QCOMPARE(join.getSql(pvDatabase.driver()), QString());
//   mdtSqlViewSchemaJoinClause vsjc;
//   QString expectedSql;
// 
//   /*
//    * Initial state
//    */
//   QVERIFY(vsjc.getSql(pvDatabase.driver()).isEmpty());
//   /*
//    * Set JOIN clause - NOT using aliases
//    */
//   vsjc.setTables("Client_tbl", "Address_tbl");
//   QVERIFY(vsjc.getSql(pvDatabase.driver()).isEmpty());
//   vsjc.setConstraintOn("Id_PK", "Client_Id_FK");
//   expectedSql = " JOIN \"Address_tbl\"\n  ON \"Address_tbl\".\"Client_Id_FK\" = \"Client_tbl\".\"Id_PK\"\n";
//   QCOMPARE(vsjc.getSql(pvDatabase.driver()), expectedSql);
//   /*
//    * Clear
//    */
//   vsjc.clear();
//   QVERIFY(vsjc.getSql(pvDatabase.driver()).isEmpty());
//   /*
//    * Set JOIN clause - Using aliases
//    */
//   vsjc.setTables("CLI", "Address_tbl", "ADR");
//   QVERIFY(vsjc.getSql(pvDatabase.driver()).isEmpty());
//   vsjc.setConstraintOn("Id_PK", "Client_Id_FK");
//   expectedSql = " JOIN \"Address_tbl\" \"ADR\"\n  ON \"ADR\".\"Client_Id_FK\" = \"CLI\".\"Id_PK\"\n";
//   QCOMPARE(vsjc.getSql(pvDatabase.driver()), expectedSql);
}

void mdtDatabaseTest::sqlViewSchemaTest()
{
  using namespace mdtSqlViewSchema;
  QString expectedSql;
  const QSqlDriver *driver = pvDatabase.driver();
  Schema view;
  Table client;
  Table address;
  JoinClause join;
  JoinKey key;

  QCOMPARE(pvDatabase.driverName(), QString("QSQLITE"));
  /*
   * Initial state
   */
  QVERIFY(view.getSqlForCreate(driver).isEmpty());
  QVERIFY(view.getSqlForDrop(driver).isEmpty());
  /*
   * Simple 1 table view - No aliases
   */
  // Setup
  client.clear();
  client.setTableName("Client_tbl");
  view.setName("Client_view");
  view.setTable(client);
  view.addSelectField(client, SelectField("Id_PK"));
  view.addSelectField(client, SelectField("Name"));
  // Check SQL to DROP view
  expectedSql = "DROP VIEW IF EXISTS \"Client_view\"";
  QCOMPARE(view.getSqlForDrop(driver), expectedSql);
  // Check SQL to CREATE view
  expectedSql  = "CREATE VIEW \"Client_view\" AS\n";
  expectedSql += "SELECT\n";
  expectedSql += " \"Client_tbl\".\"Id_PK\",\n";
  expectedSql += " \"Client_tbl\".\"Name\"\n";
  expectedSql += "FROM \"Client_tbl\"";
  QCOMPARE(view.getSqlForCreate(driver), expectedSql);
  // Clear and check empty SQL
  view.clear();
  QVERIFY(view.getSqlForCreate(driver).isEmpty());
  QVERIFY(view.getSqlForDrop(driver).isEmpty());
  /*
   * Simple 1 table view - With table and field aliases
   */
  // Setup
  client.clear();
  client.setTableName("Client_tbl");
  client.setAlias("CLI");
  view.setName("Client_view");
  view.setTable(client);
  view.addSelectField(client, SelectField("Id_PK", "Client ID"));
  view.addSelectField(client, SelectField("Name"));
  // Check SQL to DROP view
  expectedSql = "DROP VIEW IF EXISTS \"Client_view\"";
  QCOMPARE(view.getSqlForDrop(driver), expectedSql);
  // Check SQL to CREATE view
  expectedSql  = "CREATE VIEW \"Client_view\" AS\n";
  expectedSql += "SELECT\n";
  expectedSql += " \"CLI\".\"Id_PK\" AS \"Client ID\",\n";
  expectedSql += " \"CLI\".\"Name\"\n";
  expectedSql += "FROM \"Client_tbl\" \"CLI\"";
  QCOMPARE(view.getSqlForCreate(driver), expectedSql);
  // Clear and check empty SQL
  view.clear();
  QVERIFY(view.getSqlForCreate(driver).isEmpty());
  QVERIFY(view.getSqlForDrop(driver).isEmpty());
  /*
   * View with a JOIN
   */
  // Setup
  client.clear();
  client.setTableName("Client_tbl");
  client.setAlias("CLI");
  address.clear();
  address.setTableName("Address_tbl");
  address.setAlias("ADR");
  view.setName("Client_address_view");
  view.setTable(client);
  view.addSelectField(client, SelectField("Id_PK", "Client_Id"));
  view.addSelectField(client, SelectField("Name"));
  view.addSelectField(address, SelectField("Id_PK", "Address_Id"));
  view.addSelectField(address, SelectField("Street"));
  join.clear();
  join.setMainTable(client);
  join.setTableToJoin(address);
  key.clear();
  key.setMainTableField("Id_PK");
  key.setTableToJoinField("Client_Id_FK");
  join.addKey(key);
  view.addJoinClause(join);
  // Check SQL to DROP view
  expectedSql = "DROP VIEW IF EXISTS \"Client_address_view\"";
  QCOMPARE(view.getSqlForDrop(driver), expectedSql);
  // Check SQL to CREATE view
  expectedSql  = "CREATE VIEW \"Client_address_view\" AS\n";
  expectedSql += "SELECT\n";
  expectedSql += " \"CLI\".\"Id_PK\" AS \"Client_Id\",\n";
  expectedSql += " \"CLI\".\"Name\",\n";
  expectedSql += " \"ADR\".\"Id_PK\" AS \"Address_Id\",\n";
  expectedSql += " \"ADR\".\"Street\"\n";
  expectedSql += "FROM \"Client_tbl\" \"CLI\"\n";
  expectedSql += " JOIN \"Address_tbl\" \"ADR\"\n";
  expectedSql += "  ON \"CLI\".\"Id_PK\" = \"ADR\".\"Client_Id_FK\"";
  QCOMPARE(view.getSqlForCreate(driver), expectedSql);
  // Clear and check empty SQL
  view.clear();
  QVERIFY(view.getSqlForCreate(driver).isEmpty());
  QVERIFY(view.getSqlForDrop(driver).isEmpty());
}

void mdtDatabaseTest::sqlViewSchemaBenchmark()
{
  using namespace mdtSqlViewSchema;
//   Schema vs;
//   QString expectedSql;
//   QString sqlD, sqlC;
// 
//   vs.setName("Simple_view");
//   vs.setTableName("Simple_tbl");
//   vs.addSelectItem("Id_PK");
//   vs.addSelectItem("Name");
//   ///vs.setAfterSelectStatement("FROM Simple_tbl");
//   QBENCHMARK{
//     sqlD = vs.getSqlForDrop(pvDatabase.driver());
//     sqlC = vs.getSqlForCreate(pvDatabase.driver());
//   }
//   expectedSql =  "CREATE VIEW \"Simple_view\" AS\n";
//   expectedSql += "SELECT\n";
//   expectedSql += " \"Id_PK\",\n";
//   expectedSql += " \"Name\"\n";
//   expectedSql += "FROM \"Simple_tbl\"\n";
//   QCOMPARE(sqlC, expectedSql);
}

void mdtDatabaseTest::sqlTriggerSchemaTest()
{
  mdtSqlTriggerSchema trigger;

  /*
   * Initial state
   */
  QVERIFY(!trigger.isTemporary());
  QVERIFY(trigger.event() == mdtSqlTriggerSchema::Unknown);
  /*
   * Set/get
   */
  trigger.setTemporary(true);
  QVERIFY(trigger.isTemporary());
  trigger.setName("TestTrigger");
  QCOMPARE(trigger.name(), QString("TestTrigger"));
  trigger.setEvent(mdtSqlTriggerSchema::AfterInsert);
  QVERIFY(trigger.event() == mdtSqlTriggerSchema::AfterInsert);
  trigger.setTableName("Client_tbl");
  QCOMPARE(trigger.tableName(), QString("Client_tbl"));
  trigger.setScript("UPDATE Client_tbl SET Name = 'Some name';");
  QCOMPARE(trigger.script(), QString("UPDATE Client_tbl SET Name = 'Some name';"));
  /*
   * Clear
   */
  trigger.clear();
  QVERIFY(!trigger.isTemporary());
  QVERIFY(trigger.name().isEmpty());
  QVERIFY(trigger.event() == mdtSqlTriggerSchema::Unknown);
  QVERIFY(trigger.tableName().isEmpty());
  QVERIFY(trigger.script().isEmpty());
}

void mdtDatabaseTest::sqlTriggerSchemaGetSqlSqliteTest()
{
  mdtSqlTriggerSchema trigger;
  QString expectedSql;

  QCOMPARE(pvDatabase.driverName(), QString("QSQLITE"));

  /*
   * Simple trigger
   */
  // Setup trigger
  trigger.setTemporary(true);
  trigger.setName("TestTrigger");
  trigger.setEvent(mdtSqlTriggerSchema::AfterInsert);
  trigger.setTableName("Client_tbl");
  trigger.setScript(" UPDATE Client_tbl SET Name = 'Some name';");
  // Check SQL for DROP
  expectedSql = "DROP TRIGGER IF EXISTS \"TestTrigger\"";
  QCOMPARE(trigger.getSqlForDropTrigger(pvDatabase), expectedSql);
  // Check SQL for CREATE
  expectedSql = "CREATE TEMPORARY TRIGGER \"TestTrigger\" AFTER INSERT ON \"Client_tbl\"\n" \
                "FOR EACH ROW\n" \
                "BEGIN\n" \
                " UPDATE Client_tbl SET Name = 'Some name';\n" \
                "END";
  QCOMPARE(trigger.getSqlForCreateTrigger(pvDatabase), expectedSql);
}

void mdtDatabaseTest::sqlTablePopulationSchemaTest()
{
  mdtSqlTablePopulationSchema tps;
  QString expectedSql;

  /*
   * Initial state
   */
  
  /*
   * Set data starting from a empty object
   */
  // Set
  tps.setName("Type_tbl data");
  tps.setTableName("Type_tbl");
  tps.addFieldName("Id_PK");
  tps.addFieldName("Name");
  // Check data
  QCOMPARE(tps.name(), QString("Type_tbl data"));
  QCOMPARE(tps.tableName(), QString("Type_tbl"));
  QCOMPARE(tps.fieldNameList().size(), 2);
  QCOMPARE(tps.fieldNameList().at(0), QString("Id_PK"));
  QCOMPARE(tps.fieldNameList().at(1), QString("Name"));
  // Check SQL for insertion
  expectedSql = "INSERT INTO \"Type_tbl\" (\"Id_PK\",\"Name\") VALUES (?,?)";
  QCOMPARE(tps.sqlForInsert(pvDatabase.driver()), expectedSql);
  /*
   * Populate with data
   */
  QCOMPARE(tps.rowDataCount(), 0);
  // Add a row of data
  tps.currentRowData() << 1 << "Name 1";
  tps.commitCurrentRowData();
  QCOMPARE(tps.rowDataCount(), 1);
  QCOMPARE(tps.rowData(0).size(), 2);
  QCOMPARE(tps.rowData(0).at(0), QVariant(1));
  QCOMPARE(tps.rowData(0).at(1), QVariant("Name 1"));
  // Add a row of data
  tps.currentRowData() << 2 << "Name 2";
  tps.commitCurrentRowData();
  QCOMPARE(tps.rowDataCount(), 2);
  QCOMPARE(tps.rowData(1).size(), 2);
  QCOMPARE(tps.rowData(1).at(0), QVariant(2));
  QCOMPARE(tps.rowData(1).at(1), QVariant("Name 2"));
  /*
   * Clear
   */
  tps.clear();
  QVERIFY(tps.name().isEmpty());
  QVERIFY(tps.tableName().isEmpty());
  QVERIFY(tps.fieldNameList().isEmpty());
  QCOMPARE(tps.rowDataCount(), 0);
}

void mdtDatabaseTest::sqlDatabaseSchemaTest()
{
  QString expectedSql;
  mdtSqlDatabaseSchema s;
  mdtSqlTableSchema table;
  mdtSqlField field;
  mdtSqlForeignKey fk;
  mdtSqlTriggerSchema trigger;

  QCOMPARE(pvDatabase.driverName(), QString("QSQLITE"));
  /*
   * Initial state
   */
  QCOMPARE(s.tableCount(), 0);
  /*
   * Create Client_tbl
   */
  table.clear();
  table.setTableName("Client_tbl");
  // Id_PK
  field.clear();
  field.setAutoValue(true);
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  table.addField(field, true);
  // Name
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  table.addField(field, false);
  // Add table to database schema and check
  s.addTable(table);
  QCOMPARE(s.tableCount(), 1);
  /*
   * Address_tbl
   */
  table.clear();
  table.setTableName("Address_tbl");
  // Id_PK
  field.clear();
  field.setAutoValue(true);
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  table.addField(field, true);
  // Street
  field.clear();
  field.setName("Street");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  table.addField(field, false);
  // Client_Id_FK
  field.clear();
  field.setName("Client_Id_FK");
  field.setType(mdtSqlFieldType::Integer);
  field.setRequired(true);
  table.addField(field, false);
  fk.clear();
  fk.setParentTableName("Client_tbl");
  fk.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk.setCreateChildIndex(true);
  fk.addKeyFields("Id_PK", field);
  table.addForeignKey(fk);
  // Add table to database schema and check
  s.addTable(table);
  QCOMPARE(s.tableCount(), 2);
  /*
   * Trigger
   */
  // Setup trigger
  trigger.clear();
  trigger.setName("TestTrigger");
  trigger.setEvent(mdtSqlTriggerSchema::AfterInsert);
  trigger.setTableName("Client_tbl");
  trigger.setScript(" UPDATE Client_tbl SET Name = 'Some name';");
  // Add trigger to database schema
  s.addTrigger(trigger);
  QCOMPARE(s.triggerCount(), 1);
  QCOMPARE(s.triggerName(0), QString("TestTrigger"));

}

void mdtDatabaseTest::sqlDatabaseSchemaGetJoinClauseTest()
{
  using namespace mdtSqlViewSchema;
  QString expectedSql;
  mdtSqlDatabaseSchema s;
  mdtSqlTableSchema table;
  mdtSqlField field;
  mdtSqlForeignKey fk;
  JoinClause join;

  QCOMPARE(pvDatabase.driverName(), QString("QSQLITE"));
  /*
   * Setup Client_tbl and Address_tbl
   *  witch are related by a single filed FK
   */
  /*
   * Create Client_tbl
   */
  table.clear();
  table.setTableName("Client_tbl");
  // Id_PK
  field.clear();
  field.setAutoValue(true);
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  table.addField(field, true);
  // Name
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  table.addField(field, false);
  // Add table to database schema and check
  s.addTable(table);
  QCOMPARE(s.tableCount(), 1);
  /*
   * Address_tbl
   */
  table.clear();
  table.setTableName("Address_tbl");
  // Id_PK
  field.clear();
  field.setAutoValue(true);
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  table.addField(field, true);
  // Street
  field.clear();
  field.setName("Street");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  table.addField(field, false);
  // Client_Id_FK
  field.clear();
  field.setName("Client_Id_FK");
  field.setType(mdtSqlFieldType::Integer);
  field.setRequired(true);
  table.addField(field, false);
  fk.clear();
  fk.setParentTableName("Client_tbl");
  fk.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk.setCreateChildIndex(true);
  fk.addKeyFields("Id_PK", field);
  table.addForeignKey(fk);
  // Add table to database schema and check
  s.addTable(table);
  QCOMPARE(s.tableCount(), 2);
  /*
   * Get clause to JOIN Address_tbl to Client_tbl
   */
  join = s.joinClause(Table("Client_tbl"), Table("Address_tbl"));
  QVERIFY(!join.isNull());
  // Check SQL clause
  expectedSql  = " JOIN \"Address_tbl\"\n";
  expectedSql += "  ON \"Client_tbl\".\"Id_PK\" = \"Address_tbl\".\"Client_Id_FK\"";
  QCOMPARE(join.getSql(pvDatabase.driver()), expectedSql);
  /*
   * Get clause to JOIN Client_tbl to Address_tbl
   * Note: this is a very common use case
   */
  join = s.joinClause(Table("Address_tbl"), Table("Client_tbl"));
  QVERIFY(!join.isNull());
  expectedSql  = " JOIN \"Client_tbl\"\n";
  expectedSql += "  ON \"Address_tbl\".\"Client_Id_FK\" = \"Client_tbl\".\"Id_PK\"";
  QCOMPARE(join.getSql(pvDatabase.driver()), expectedSql);
  /*
   * Setup ArticleLink_tbl and Link_tbl
   *  witch are related by a multiple fields FK
   */
  /*
   * ArticleLink_tbl
   */
  table.clear();
  table.setTableName("ArticleLink_tbl");
  // Id_S_PK
  field.clear();
  field.setName("Id_S_PK");
  field.setType(mdtSqlFieldType::Integer);
  table.addField(field, true);
  // Id_E_PK
  field.clear();
  field.setName("Id_E_PK");
  field.setType(mdtSqlFieldType::Integer);
  table.addField(field, true);
  s.addTable(table);
  /*
   * Link_tbl
   */
  table.clear();
  table.setTableName("Link_tbl");
  // Setup FK's common
  fk.clear();
  fk.setParentTableName("ArticleLink_tbl");
  fk.setOnDeleteAction(mdtSqlForeignKey::Restrict);
  fk.setOnUpdateAction(mdtSqlForeignKey::Cascade);
  fk.setCreateChildIndex(true);
  // Id_S_PK_FK
  field.clear();
  field.setName("Id_S_PK_FK");
  field.setType(mdtSqlFieldType::Integer);
  table.addField(field, true);
  fk.addKeyFields("Id_S_PK", field);
  // Id_E_PK_FK
  field.clear();
  field.setName("Id_E_PK_FK");
  field.setType(mdtSqlFieldType::Integer);
  table.addField(field, true);
  fk.addKeyFields("Id_E_PK", field);
  table.addForeignKey(fk);
  // Add table to database schema and check
  s.addTable(table);
  /*
   * Get clause to JOIN ArticleLink_tbl to Link_tbl
   */
  join = s.joinClause(Table("Link_tbl"), Table("ArticleLink_tbl"));
  QVERIFY(!join.isNull());
  // Check SQL clause
  expectedSql  = " JOIN \"ArticleLink_tbl\"\n";
  expectedSql += "  ON \"Link_tbl\".\"Id_S_PK_FK\" = \"ArticleLink_tbl\".\"Id_S_PK\"\n";
  expectedSql += "  AND \"Link_tbl\".\"Id_E_PK_FK\" = \"ArticleLink_tbl\".\"Id_E_PK\"";
  QCOMPARE(join.getSql(pvDatabase.driver()), expectedSql);
  /*
   * Get clause to JOIN Link_tbl to ArticleLink_tbl
   */
  join = s.joinClause(Table("ArticleLink_tbl"), Table("Link_tbl"));
  QVERIFY(!join.isNull());
  // Check SQL clause
  expectedSql  = " JOIN \"Link_tbl\"\n";
  expectedSql += "  ON \"ArticleLink_tbl\".\"Id_S_PK\" = \"Link_tbl\".\"Id_S_PK_FK\"\n";
  expectedSql += "  AND \"ArticleLink_tbl\".\"Id_E_PK\" = \"Link_tbl\".\"Id_E_PK_FK\"";
  QCOMPARE(join.getSql(pvDatabase.driver()), expectedSql);
}

void mdtDatabaseTest::sqlDatabaseSchemaModelTest()
{
  const int objectColumn = 0;
  const int progressColumn = 1;
  const int statusColumn = 2;
  mdtSqlDatabaseSchema s;
  mdtSqlDatabaseSchemaModel model;
  mdtSqlTableSchema ts;
  mdtSqlViewSchema::Schema vs;
  mdtSqlTablePopulationSchema tps;
  mdtSqlTriggerSchema trigger;
  mdtSqlField field;
  QTreeView treeView;
  QModelIndex index, parentIndex;

  /*
   * Build a database schema
   */
  // Build Client_tbl
  ts.clear();
  ts.setTableName("Client_tbl", "UTF8");
  // Id_PK
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  ts.addField(field, true);
  // Name
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  ts.addField(field, false);
  s.addTable(ts);
  // Build Address_tbl
  ts.clear();
  ts.setTableName("Address_tbl", "UTF8");
  // Id_PK
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  ts.addField(field, true);
  s.addTable(ts);
  // Build Client_view
  vs.clear();
  vs.setName("Client_view");
  s.addView(vs);
  // Build table population schema for Client_tbl
  tps.clear();
  tps.setName("Client_tbl base data");
  s.addTablePopulation(tps);
  // Build trigger schema
  trigger.clear();
  trigger.setName("Client_tbl trigger");
  s.addTrigger(trigger);
  // Set schema to model
  model.setSchema(s);
  /*
   * Currently, we have this tree:
   *
   * Tables
   *   |--Client_tbl
   *   |--Address_tbl
   * Views
   *   |--Client_view
   * Table population
   *   |--Client_tbl base data
   * Triggers
   *   |--Client_tbl trigger
   *
   * Check getting objects
   */
  // Check tables
  index = model.index(0, objectColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Tables"));
  parentIndex = index;
  index = model.index(0, 0, parentIndex);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Client_tbl"));
  index = model.index(1, 0, parentIndex);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Address_tbl"));
  // Check views
  index = model.index(1, objectColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Views"));
  parentIndex = index;
  index = model.index(0, 0, parentIndex);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Client_view"));
  // Check table pupolations
  index = model.index(2, objectColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Table population"));
  parentIndex = index;
  index = model.index(0, 0, parentIndex);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Client_tbl base data"));
  // Check triggers
  index = model.index(3, objectColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Triggers"));
  parentIndex = index;
  index = model.index(0, 0, parentIndex);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Client_tbl trigger"));
  /*
   * Check set/get progress
   */
  // Tables category
  model.setProgress(mdtSqlDatabaseSchemaModel::Table, 15);
  index = model.index(0, progressColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant(15));
  // Client_tbl
  parentIndex = index;
  model.setProgress(mdtSqlDatabaseSchemaModel::Table, "Client_tbl", 7);
  index = model.index(0, progressColumn, parentIndex);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant(7));
  /*
   * Check set/get status
   */
  // Tables category
  model.setStatus(mdtSqlDatabaseSchemaModel::Table, mdtSqlDatabaseSchemaModel::StatusOk);
  index = model.index(0, statusColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Ok"));
  // Client_tbl
  parentIndex = index;
  model.setStatus(mdtSqlDatabaseSchemaModel::Table, "Client_tbl", mdtSqlDatabaseSchemaModel::StatusError);
  index = model.index(0, statusColumn, parentIndex);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant("Error"));
  /*
   * Clear status and progress
   */
  model.clearStatusAndProgress();
  // Check Table category status
  index = model.index(0, statusColumn);
  QVERIFY(index.isValid());
  QVERIFY(model.data(index).isNull());
  // Check Table category progress
  index = model.index(0, progressColumn);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant(0));
  // Check Client_tbl status
  parentIndex = index;
  index = model.index(0, statusColumn, parentIndex);
  QVERIFY(index.isValid());
  QVERIFY(model.data(index).isNull());
  // Check Client_tbl progress
  index = model.index(0, progressColumn, parentIndex);
  QVERIFY(index.isValid());
  QCOMPARE(model.data(index), QVariant(0));
  /// \todo Should check model by calling its function as we weher a tree view.
  
  /// \todo See also http://code.qt.io/cgit/qt/qt.git/tree/tests/auto/modeltest
  
  
  
  treeView.setModel(&model);
  treeView.show();

  /*
   * Update model
   */
  model.setSchema(s);
  
  model.setProgress(mdtSqlDatabaseSchemaModel::Table, "Client_tbl", 4);
  model.setProgress(mdtSqlDatabaseSchemaModel::Table, "Address_tbl", 6);
  model.setProgress(mdtSqlDatabaseSchemaModel::Table, 10);

  /*
   * Play
   */
  while(treeView.isVisible()){
    QTest::qWait(500);
  }
}

void mdtDatabaseTest::sqlDatabaseSchemaDialogTest()
{
  mdtSqlDatabaseSchemaDialog dialog;
  mdtSqlDatabaseSchema s;
  mdtSqlTableSchema ts;
  mdtSqlViewSchema::Schema vs;
  mdtSqlTablePopulationSchema tps;
  mdtSqlTriggerSchema trigger;
  mdtSqlField field;

  /*
   * Build a database schema
   */
  // Build Client_tbl
  ts.clear();
  ts.setTableName("Client_tbl", "UTF8");
  // Id_PK
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  ts.addField(field, true);
  // Name
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  ts.addField(field, false);
  s.addTable(ts);
  // Build Address_tbl
  ts.clear();
  ts.setTableName("Address_tbl", "UTF8");
  // Id_PK
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  ts.addField(field, true);
  s.addTable(ts);
  // Build Client_view
  vs.clear();
  vs.setName("Client_view");
  s.addView(vs);
  // Build table population schema for Client_tbl
  tps.clear();
  tps.setName("Client_tbl base data");
  tps.setTableName("Client_tbl");
  tps.addFieldName("Id_PK");
  tps.addFieldName("Name");
  tps.currentRowData() << QVariant() << "A name 1";
  tps.commitCurrentRowData();
  tps.currentRowData() << QVariant() << "A name 2";
  tps.commitCurrentRowData();
  tps.currentRowData() << QVariant() << "A name 3";
  tps.commitCurrentRowData();
  s.addTablePopulation(tps);
  // Build trigger for Client_tbl
  trigger.clear();
  trigger.setName("on_Client_tbl_AfterInsert");
  trigger.setTableName("Client_tbl");
  trigger.setScript(" UPDATE Client_tbl SET Remarks = 'Some remark';");
  s.addTrigger(trigger);

  dialog.setDatabase(pvDatabase);
  dialog.setSchema(s);
  dialog.exec();
}

void mdtDatabaseTest::sqlDatabaseSchemaThreadTest()
{
  mdtSqlDatabaseSchemaThread thread;
  QTemporaryFile tempFile;
  QFileInfo dbFileInfo;
  QSqlDatabase db;
  QString sql;
  mdtSqlDatabaseSchema s;
  mdtSqlTableSchema ts;
  mdtSqlTablePopulationSchema tps;
  mdtSqlTriggerSchema trigger;
  ///QSqlRecord record;
  mdtSqlField field;
  mdtSqlIndex index;
  mdtSqlForeignKey fk;
  ///mdtSqlPrimaryKey pk;

  /*
   * Create a temporary database
   */
  QVERIFY(tempFile.open());
  tempFile.close();
  dbFileInfo.setFile(tempFile.fileName() + ".db");
  // Create connection to database
  db = QSqlDatabase::addDatabase("QSQLITE", "sqlDatabaseSchemaThreadTest");
  db.setDatabaseName(dbFileInfo.filePath());
  QVERIFY(db.open());
  // QSqlQuery must be created after db.open() was called.
  QSqlQuery q(db);
  /*
   * Enable foreing keys support
   */
  sql = "PRAGMA foreign_keys = ON";
  QVERIFY(q.exec(sql));
  /*
   * Build a database schema
   */
  // Build Client_tbl
  ts.clear();
  ts.setTableName("Client_tbl", "UTF8");
  // Id_PK
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  ts.addField(field, true);
  // Name
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  ts.addField(field, false);
  // Remarks (populated by trigger)
  field.clear();
  field.setName("Remarks");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(100);
  ts.addField(field, false);
  s.addTable(ts);
  // Build Client_view
  mdtSqlViewSchema::Schema view;
  mdtSqlViewSchema::Table client("Client_tbl", "CLI");
  view.setName("Client_view");
  view.setTable(client);
  view.addSelectField(client, mdtSqlViewSchema::SelectField("*"));
  s.addView(view);
  // Build table population schema for Client_tbl
  tps.clear();
  tps.setName("Client_tbl base data");
  tps.setTableName("Client_tbl");
  tps.addFieldName("Id_PK");
  tps.addFieldName("Name");
  tps.currentRowData() << QVariant() << "A name 1";
  tps.commitCurrentRowData();
  tps.currentRowData() << QVariant() << "A name 2";
  tps.commitCurrentRowData();
  tps.currentRowData() << QVariant() << "A name 3";
  tps.commitCurrentRowData();
  s.addTablePopulation(tps);
  // Build trigger for Client_tbl
  trigger.clear();
  trigger.setName("on_Client_tbl_AfterInsert");
  trigger.setTableName("Client_tbl");
  trigger.setScript(" UPDATE Client_tbl SET Remarks = 'Some remark';");
  s.addTrigger(trigger);
  /*
   * Create schema in blocking way
   */
  QVERIFY(thread.createSchemaBlocking(s, db));
  // Check that objects where created
  QVERIFY(db.tables(QSql::AllTables).contains("Client_tbl"));
  QVERIFY(db.tables(QSql::AllTables).contains("Client_view"));
  // Check that Client_tbl was populated
  QVERIFY(q.exec("SELECT * FROM Client_tbl"));
  QVERIFY(q.next());
  QCOMPARE(q.record().value("Name"), QVariant("A name 1"));
  QCOMPARE(q.record().value("Remarks"), QVariant("Some remark"));
  QVERIFY(q.next());
  QCOMPARE(q.record().value("Name"), QVariant("A name 2"));
  QVERIFY(q.next());
  QCOMPARE(q.record().value("Name"), QVariant("A name 3"));
  /*
   * Drop schema
   */
  QVERIFY(q.exec("DROP TABLE Client_tbl"));
  QVERIFY(q.exec("DROP VIEW Client_view"));
  QVERIFY(!db.tables(QSql::AllTables).contains("Client_tbl"));
  QVERIFY(!db.tables(QSql::AllTables).contains("Client_view"));
  /*
   * Create schema in asnyc way (and wait thread end)
   */
  thread.createSchema(s, db);
  thread.wait();
  // Check that objects where created
  QVERIFY(db.tables(QSql::AllTables).contains("Client_tbl"));
  QVERIFY(db.tables(QSql::AllTables).contains("Client_view"));
  // Check that Client_tbl was populated
  QVERIFY(q.exec("SELECT * FROM Client_tbl"));
  QVERIFY(q.next());
  QCOMPARE(q.record().value("Name"), QVariant("A name 1"));
  QCOMPARE(q.record().value("Remarks"), QVariant("Some remark"));
  QVERIFY(q.next());
  QCOMPARE(q.record().value("Name"), QVariant("A name 2"));
  QVERIFY(q.next());
  QCOMPARE(q.record().value("Name"), QVariant("A name 3"));

  // Free resources
  db.close();
  db = QSqlDatabase();
  QSqlDatabase::removeDatabase(db.connectionName());
}

void mdtDatabaseTest::connectionNameWidgetTest()
{
  mdtSqlConnectionNameWidget widget;
  QStringList connectionNames;

  qDebug() << QSqlDatabase::connectionNames();
  /*
   * Check getConnectionNames() function
   */
  // Initially, we have only the default connection, witch uses the SQLite driver
  QCOMPARE(QSqlDatabase::connectionNames().size(), 1);
  // Create some connections
  QSqlDatabase::addDatabase("QSQLITE", "cnn1");
  QSqlDatabase::addDatabase("QMYSQL", "cnn2");
  QSqlDatabase::addDatabase("QPSQL", "cnn3");
  QSqlDatabase::addDatabase("QSQLITE", "cnn4");
  // Get connection names and check
  connectionNames = mdtSqlConnectionNameWidget::getConnectionNames(mdtSqlDriverType::SQLite, true);
  QCOMPARE(connectionNames.size(), 3);
  QVERIFY(connectionNames.contains("cnn1"));
  QVERIFY(!connectionNames.contains("cnn2"));
  QVERIFY(!connectionNames.contains("cnn3"));
  QVERIFY(connectionNames.contains("cnn4"));
  /*
   * Check widget
   */
  // Initial state
  QVERIFY(widget.pvDriverType == mdtSqlDriverType::Unknown);
  QCOMPARE(widget.cbConnectionNames->count(), 0);
  // Set SQLite driver type
  widget.setDriverType(mdtSqlDriverType::SQLite);
  QCOMPARE(widget.cbConnectionNames->count(), 3);

  /*
   * Play
   */
  widget.show();
  while(widget.isVisible()){
    QTest::qWait(500);
  }

  // Check that we didn't close the default database
  QVERIFY(pvDatabase.isOpen());
  // Remove added connections
  QSqlDatabase::removeDatabase("cnn1");
  QSqlDatabase::removeDatabase("cnn2");
  QSqlDatabase::removeDatabase("cnn3");
  QSqlDatabase::removeDatabase("cnn4");
}

void mdtDatabaseTest::basicInfoWidgetTest()
{
  mdtSqlDatabaseBasicInfoWidget w;

  w.show();
  w.displayInfo(QSqlDatabase());
  QTest::qWait(2000);
  w.displayInfo(pvDatabase);

  /*
   * Play
   */
  while(w.isVisible()){
    QTest::qWait(500);
  }
}

void mdtDatabaseTest::databaseSqliteTest()
{
  /*
   * Check getOpenConnectionReferingDatabaseCount() function
   */
  // Initially, we have only the default connection, witch uses the SQLite driver
  QCOMPARE(QSqlDatabase::connectionNames().size(), 1);
  QCOMPARE(mdtSqlDatabaseSqlite::getOpenConnectionReferingDatabaseCount(pvDatabase), 1);
  // Create some connections
  QSqlDatabase::addDatabase("QSQLITE", "cnn1");
  QSqlDatabase::addDatabase("QMYSQL", "cnn2");
  QSqlDatabase::addDatabase("QPSQL", "cnn3");
  QSqlDatabase::addDatabase("QSQLITE", "cnn4");
  QCOMPARE(mdtSqlDatabaseSqlite::getOpenConnectionReferingDatabaseCount(pvDatabase), 1);
  /*
   * Check object construction
   */
  // Default constructed
  mdtSqlDatabaseSqlite db1;
  QVERIFY(!db1.isValid());
  QVERIFY(db1.database().connectionName().isEmpty());
  QVERIFY(!db1.isOpen());
  QVERIFY(pvDatabase.isOpen());
  
  // We peeked up the default connection, so we must re-open the database
  ///QVERIFY(pvDatabase.open());
  // Constructed on base of a valid QSqlDatabase object
  mdtSqlDatabaseSqlite db2(QSqlDatabase::database("cnn1", false));
  QVERIFY(db2.isValid());
  QCOMPARE(db2.database().connectionName(), QString("cnn1"));
  QVERIFY(!db2.isOpen());
  // Clear
  db2.clear();
  QVERIFY(!db2.isValid());
  QVERIFY(!db2.isOpen());
  /*
   * Check database (file) creation
   */
  QTemporaryFile dbFile;
  QFileInfo dbFileInfo;
  // Setup file info
  QVERIFY(dbFile.open());
  dbFileInfo.setFile(dbFile);
  dbFile.close();
  // Prepare db2 to a known state
  db2.close();
  db2.setDatabase(QSqlDatabase::database("cnn1", false));
  db2.clearDatabaseName();
  QVERIFY(db2.isValid());
  QVERIFY(!db2.isOpen());
  QVERIFY(db2.database().databaseName().isEmpty());
  // Currently, no connection refers to database that we want to create
  QVERIFY(db2.getConnectionNameUsingDatabase(dbFileInfo).isEmpty());
  // Check creation with ovewriting existing file
  QVERIFY(db2.createDatabase(dbFileInfo, mdtSqlDatabaseSqlite::OverwriteExisting));
  QVERIFY(db2.isOpen());
  QCOMPARE(db2.database().databaseName(), dbFileInfo.absoluteFilePath());
  // Now, cnn1 refers to freshly created database - Check with open database object
  QCOMPARE(db2.getConnectionNameUsingDatabase(dbFileInfo), QString("cnn1"));
  // Now we have 2 open connections that refers to 2 different databases
  QCOMPARE(mdtSqlDatabaseSqlite::getOpenConnectionReferingDatabaseCount(pvDatabase), 1);
  QCOMPARE(mdtSqlDatabaseSqlite::getOpenConnectionReferingDatabaseCount(db2.database()), 1);
  // Close - must keep database name
  db2.close();
  QCOMPARE(db2.database().databaseName(), dbFileInfo.absoluteFilePath());
  // As before, cnn1 refers to freshly created database - Check with closed database object
  QCOMPARE(db2.getConnectionNameUsingDatabase(dbFileInfo), QString("cnn1"));
  // Now we have 2 connections that refers to 2 different databases, but only the one refered by pvDatabase is open
  QCOMPARE(mdtSqlDatabaseSqlite::getOpenConnectionReferingDatabaseCount(pvDatabase), 1);
  QCOMPARE(mdtSqlDatabaseSqlite::getOpenConnectionReferingDatabaseCount(db2.database()), 0);
  // Check creation with failing on existing database file
  QVERIFY(!db2.createDatabase(dbFileInfo, mdtSqlDatabaseSqlite::FailIfExists));
  QVERIFY(!db2.isOpen());
  QCOMPARE(db2.database().databaseName(), dbFileInfo.absoluteFilePath());
  // Check creation with keep existing database file
  QVERIFY(db2.createDatabase(dbFileInfo, mdtSqlDatabaseSqlite::KeepExisting));
  QVERIFY(db2.isOpen());
  QCOMPARE(db2.database().databaseName(), dbFileInfo.absoluteFilePath());
  db2.close();
  // Check opening given file
  QVERIFY(db2.openDatabase(dbFileInfo));
  QVERIFY(db2.isOpen());
  QCOMPARE(db2.database().databaseName(), dbFileInfo.absoluteFilePath());
  db2.close();
  // Check opening allready set database
  QVERIFY(db2.openDatabase());
  QVERIFY(db2.isOpen());
  QCOMPARE(db2.database().databaseName(), dbFileInfo.absoluteFilePath());
  db2.close();
  db2.clear();

  // Check that we didn't close the default database
  QVERIFY(pvDatabase.isOpen());
  // Remove added connections
  QSqlDatabase::removeDatabase("cnn1");
  QSqlDatabase::removeDatabase("cnn2");
  QSqlDatabase::removeDatabase("cnn3");
  QSqlDatabase::removeDatabase("cnn4");
}

void mdtDatabaseTest::databaseDialogSqliteTest()
{
  mdtSqlDatabaseDialogSqlite *dialog = new mdtSqlDatabaseDialogSqlite;

  /*
   * Check set/get database
   */
  // Initially, we have only the default connection, witch uses the SQLite driver
  QVERIFY(dialog->database().database().isValid());
  QVERIFY(QSqlDatabase::database().isOpen());
  // Add some connections
  QSqlDatabase::addDatabase("QSQLITE", "cnn1");
  QSqlDatabase::addDatabase("QSQLITE", "cnn2");
  QSqlDatabase::addDatabase("QSQLITE", "cnn3");
  dialog->updateConnectionsList();
  dialog->addNonEditableConnectionName(pvDatabase.connectionName());
  // Set connections and check
  dialog->setDatabase(QSqlDatabase::database("cnn2", false));
  QCOMPARE(dialog->database().database().connectionName(), QString("cnn2"));
  QVERIFY(!dialog->database().database().isOpen());
  
  

  
  dialog->exec();
  /*
   * Delete dialog here, so that contained database object is destructed,
   *  else, Qt will warn that we are removing a connection that is currently used.
   */
  delete dialog;
  // Check that we didn't close the default database
  QVERIFY(pvDatabase.isOpen());
  // Remove added connections
  QSqlDatabase::removeDatabase("cnn1");
  QSqlDatabase::removeDatabase("cnn2");
  QSqlDatabase::removeDatabase("cnn3");
}

void mdtDatabaseTest::databaseManagerTest()
{
  mdtSqlDatabaseManager m;
  mdtSqlTableSchema st;
  mdtSqlField field;

  QTemporaryFile dbFile;
  QFileInfo dbFileInfo;

  /*
   * Check Sqlite database creation
   */
  QVERIFY(dbFile.open());
  dbFileInfo.setFile(dbFile);
  dbFile.close();
  // database was never open, creation must work (file will be overwritten)
  QVERIFY(m.createDatabaseSqlite(dbFileInfo, mdtSqlDatabaseManager::OverwriteExisting, "databaseManagerTest"));
  QVERIFY(m.database().isOpen());
  // database exists and is allready open, must work
  QVERIFY(m.createDatabaseSqlite(dbFileInfo, mdtSqlDatabaseManager::KeepExisting, "databaseManagerTest"));
  QVERIFY(m.database().isOpen());
  // database exists and is allready open, must fail
  QVERIFY(!m.createDatabaseSqlite(dbFileInfo, mdtSqlDatabaseManager::FailIfExists, "databaseManagerTest"));
  // Check that manager not closes the database
  QVERIFY(m.database().isOpen());
  m.database().close();
  QVERIFY(!m.database().isOpen());
  // database is closed, but allready exists, must fail
  QVERIFY(!m.createDatabaseSqlite(dbFileInfo, mdtSqlDatabaseManager::FailIfExists, "databaseManagerTest"));
  QVERIFY(!m.database().isOpen());
  // database is closed but exists. We want to overwite -> must work
  QVERIFY(m.createDatabaseSqlite(dbFileInfo, mdtSqlDatabaseManager::OverwriteExisting, "databaseManagerTest"));
  QVERIFY(m.database().isOpen());
  // Check table creation
  st.setTableName("Client_tbl", "UTF8");
  field.clear();
  field.setName("Id_PK");
  field.setType(mdtSqlFieldType::Integer);
  field.setAutoValue(true);
  st.addField(field, true);
  field.clear();
  field.setName("Name");
  field.setType(mdtSqlFieldType::Varchar);
  field.setLength(50);
  st.addField(field, false);
  // First creation
  QVERIFY(m.createTable(st, mdtSqlDatabaseManager::FailIfExists));
  QVERIFY(m.database().tables().contains("Client_tbl"));
  // Check overwite
  QVERIFY(m.createTable(st, mdtSqlDatabaseManager::OverwriteExisting));
  QVERIFY(m.database().tables().contains("Client_tbl"));
  
  // Cleanup
  m.database().close();
  QFile::remove(dbFileInfo.filePath());
  // Check that we didn't close the default database
  QVERIFY(pvDatabase.isOpen());
}

void mdtDatabaseTest::sqlRecordTest()
{
  mdtSqlRecord record, record2;
  QList<mdtSqlRecord> recordList;
  QSqlQuery query(pvDatabase);
  QString sql;
  mdtSqlRecord matchData;
  QStringList expectedFieldNames;

  // Set database data
  clearTestDatabaseData();
  populateTestDatabase();

  /*
   * Fields creation
   */

  // Add existing fields
  expectedFieldNames.clear();
  expectedFieldNames << "Id_PK";
  QVERIFY(record.addField("Id_PK", "Client_tbl", pvDatabase));
  QCOMPARE(record.count(), 1);
  QCOMPARE(record.field(0).name(), QString("Id_PK"));
  QCOMPARE(record.fieldNames(), expectedFieldNames);
  expectedFieldNames << "FirstName";
  QVERIFY(record.addField("FirstName", "Client_tbl", pvDatabase));
  QCOMPARE(record.count(), 2);
  QCOMPARE(record.field(1).name(), QString("FirstName"));
  QCOMPARE(record.fieldNames(), expectedFieldNames);
  // Try to add a non existing field
  QVERIFY(!record.addField("jkhhdsh dhaskhd sah asdk", "Client_tbl", pvDatabase));
  QCOMPARE(record.count(), 2);
  QCOMPARE(record.fieldNames(), expectedFieldNames);
  // Clear fields
  record.clear();
  QCOMPARE(record.count(), 0);
  expectedFieldNames.clear();
  QCOMPARE(record.fieldNames(), expectedFieldNames);
  expectedFieldNames << "Id_PK" << "FirstName" << "Remarks";
  // Add all fields of Client_tbl and check
  QVERIFY(record.addAllFields("Client_tbl", pvDatabase));
  QCOMPARE(record.count(), 3);
  QCOMPARE(record.field(0).name(), QString("Id_PK"));
  QCOMPARE(record.field(1).name(), QString("FirstName"));
  QCOMPARE(record.field(2).name(), QString("Remarks"));
  QCOMPARE(record.fieldNames(), expectedFieldNames);

  /*
   * Edition
   */

  // Initial flags
  QCOMPARE(record.count(), 3);
  QVERIFY(!record.hasValue(0));
  QVERIFY(!record.hasValue(1));
  QVERIFY(!record.hasValue(2));
  QVERIFY(!record.hasValue("Id_PK"));
  QVERIFY(!record.hasValue("FirstName"));
  QVERIFY(!record.hasValue("Remarks"));
  QCOMPARE(record.fieldIndexesWithValue().size(), 0);
  // Edit a field and check
  record.setValue("FirstName", "A test name");
  QVERIFY(!record.hasValue("Id_PK"));
  QVERIFY(record.hasValue("FirstName"));
  QVERIFY(!record.hasValue("Remarks"));
  QCOMPARE(record.fieldIndexesWithValue().size(), 1);
  QCOMPARE(record.fieldIndexesWithValue().at(0), 1);
  // Clear values and check
  record.clearValues();
  QCOMPARE(record.count(), 3);
  QVERIFY(!record.hasValue("Id_PK"));
  QVERIFY(!record.hasValue("FirstName"));
  QVERIFY(!record.hasValue("Remarks"));
  QCOMPARE(record.fieldIndexesWithValue().size(), 0);
  // Edit a field with a Null value and check
  record.setValue("FirstName", QVariant());
  QVERIFY(!record.hasValue("Id_PK"));
  QVERIFY(record.hasValue("FirstName"));
  QVERIFY(!record.hasValue("Remarks"));
  QCOMPARE(record.fieldIndexesWithValue().size(), 1);
  QCOMPARE(record.fieldIndexesWithValue().at(0), 1);
  // Clear values and check
  record.clearValues();
  QCOMPARE(record.count(), 3);
  QVERIFY(!record.hasValue("Id_PK"));
  QVERIFY(!record.hasValue("FirstName"));
  QVERIFY(!record.hasValue("Remarks"));
  QCOMPARE(record.fieldIndexesWithValue().size(), 0);

  /*
   * Copy
   */

  // Copy and check
  record2 = record;
  QCOMPARE(record2.count(), 3);
  QCOMPARE(record2.field(0).name(), QString("Id_PK"));
  QCOMPARE(record2.field(1).name(), QString("FirstName"));
  QCOMPARE(record2.field(2).name(), QString("Remarks"));
  QVERIFY(!record2.hasValue("Id_PK"));
  QVERIFY(!record2.hasValue("FirstName"));
  QVERIFY(!record2.hasValue("Remarks"));
  // Edit first
  record.setValue("Remarks", "A test remark");
  QVERIFY(!record.hasValue("Id_PK"));
  QVERIFY(!record.hasValue("FirstName"));
  QVERIFY(record.hasValue("Remarks"));
  QVERIFY(!record2.hasValue("Id_PK"));
  QVERIFY(!record2.hasValue("FirstName"));
  QVERIFY(!record2.hasValue("Remarks"));

  /*
   * Build a mdtSqlRecord on the base of a QSqlRecord
   */

  // Create a QSqlRecord
  QSqlRecord sqlRecord = pvDatabase.record("Client_tbl");
  // Build mdtSqlRecord
  mdtSqlRecord record3(sqlRecord);
  QCOMPARE(record3.count(), 3);
  QCOMPARE(record3.field(0).name(), QString("Id_PK"));
  QCOMPARE(record3.field(1).name(), QString("FirstName"));
  QCOMPARE(record3.field(2).name(), QString("Remarks"));

  /*
   * Assign a QSqlRecord to a mdtSqlRecord
   */

  record.clear();
  QCOMPARE(record.count(), 0);
  record = sqlRecord;
  QCOMPARE(record.count(), 3);
  QCOMPARE(record.field(0).name(), QString("Id_PK"));
  QCOMPARE(record.field(1).name(), QString("FirstName"));
  QCOMPARE(record.field(2).name(), QString("Remarks"));

  /*
   * List storage
   */

  // Get all records of Client_tbl
  sql = "SELECT Id_PK, FirstName, Remarks FROM Client_tbl";
  QVERIFY(query.exec(sql));
  while(query.next()){
    recordList.append(query.record());
  }
  // Check record list
  QCOMPARE(recordList.size(), 2);
  record = recordList.at(0);
  QCOMPARE(record.count(), 3);
  QCOMPARE(record.field(0).name(), QString("Id_PK"));
  QCOMPARE(record.field(1).name(), QString("FirstName"));
  QCOMPARE(record.field(2).name(), QString("Remarks"));
  QCOMPARE(record.value("FirstName"), QVariant("Andy"));
  QCOMPARE(record.value("Remarks"), QVariant(""));
  record = recordList.at(1);
  QCOMPARE(record.count(), 3);
  QCOMPARE(record.field(0).name(), QString("Id_PK"));
  QCOMPARE(record.field(1).name(), QString("FirstName"));
  QCOMPARE(record.field(2).name(), QString("Remarks"));
  QCOMPARE(record.value("FirstName"), QVariant("Bety"));
  QCOMPARE(record.value("Remarks"), QVariant("Remark on Bety"));

  /*
   * SQL for insertion
   */

  // Check contiguous fields
  record.clearValues();
  QCOMPARE(record.count(), 3);
  record.setValue("Id_PK", 1);
  QCOMPARE(record.sqlForInsert("Client_tbl"), QString("INSERT INTO Client_tbl (Id_PK) VALUES (?)"));
  record.setValue("Id_PK", 2);
  record.setValue("FirstName", "Name 2");
  QCOMPARE(record.sqlForInsert("Client_tbl"), QString("INSERT INTO Client_tbl (Id_PK,FirstName) VALUES (?,?)"));
  record.setValue("Id_PK", 3);
  record.setValue("FirstName", "Name 3");
  record.setValue("Remarks", "REM 3");
  QCOMPARE(record.sqlForInsert("Client_tbl"), QString("INSERT INTO Client_tbl (Id_PK,FirstName,Remarks) VALUES (?,?,?)"));
  // Check non contiguous fields
  record.clearValues();
  record.setValue("Remarks", "REM 4");
  QCOMPARE(record.sqlForInsert("Client_tbl"), QString("INSERT INTO Client_tbl (Remarks) VALUES (?)"));
  record.setValue("Id_PK", 4);
  QCOMPARE(record.sqlForInsert("Client_tbl"), QString("INSERT INTO Client_tbl (Id_PK,Remarks) VALUES (?,?)"));

  /*
   * SQL for update
   */

  // Update 1 field , 1 condition field
  record.clearValues();
  matchData.clear();
  record.setValue("FirstName", "Name 5");
  QVERIFY(matchData.addField("Id_PK", "Client_tbl", pvDatabase));
  matchData.setValue("Id_PK", 5);
  QCOMPARE(record.sqlForUpdate("Client_tbl", matchData), QString("UPDATE Client_tbl SET FirstName=? WHERE Id_PK=5"));
  // Update 2 fields , 1 condition field
  record.clearValues();
  matchData.clear();
  record.setValue("FirstName", "Name 6");
  record.setValue("Remarks", "Remark 6");
  QVERIFY(matchData.addField("Id_PK", "Client_tbl", pvDatabase));
  matchData.setValue("Id_PK", 6);
  QCOMPARE(record.sqlForUpdate("Client_tbl", matchData), QString("UPDATE Client_tbl SET FirstName=?,Remarks=? WHERE Id_PK=6"));
  // Update 3 fields , 1 condition field
  record.clearValues();
  matchData.clear();
  record.setValue("Id_PK", 7);
  record.setValue("FirstName", "Name 7");
  record.setValue("Remarks", "Remark 7");
  QVERIFY(matchData.addField("Id_PK", "Client_tbl", pvDatabase));
  matchData.setValue("Id_PK", 7);
  QCOMPARE(record.sqlForUpdate("Client_tbl", matchData), QString("UPDATE Client_tbl SET Id_PK=?,FirstName=?,Remarks=? WHERE Id_PK=7"));
  // Update 1 field , 2 condition fields
  record.clearValues();
  matchData.clear();
  record.setValue("FirstName", "Name 8");
  QVERIFY(matchData.addField("Id_PK", "Client_tbl", pvDatabase));
  QVERIFY(matchData.addField("FirstName", "Client_tbl", pvDatabase));
  matchData.setValue("Id_PK", 7);
  matchData.setValue("FirstName", "Name 7");
  QCOMPARE(record.sqlForUpdate("Client_tbl", matchData), QString("UPDATE Client_tbl SET FirstName=? WHERE Id_PK=7 AND FirstName='Name 7'"));
  // Update 1 field , 3 condition fields
  record.clearValues();
  matchData.clear();
  record.setValue("FirstName", "Name 9");
  QVERIFY(matchData.addField("Id_PK", "Client_tbl", pvDatabase));
  QVERIFY(matchData.addField("FirstName", "Client_tbl", pvDatabase));
  QVERIFY(matchData.addField("Remarks", "Client_tbl", pvDatabase));
  matchData.setValue("Id_PK", 8);
  matchData.setValue("FirstName", "Name 8");
  matchData.setValue("Remarks", "Remark 8");
  QCOMPARE(record.sqlForUpdate("Client_tbl", matchData), QString("UPDATE Client_tbl SET FirstName=? WHERE Id_PK=8 AND FirstName='Name 8' AND Remarks='Remark 8'"));

  // Check that we didn't close the default database
  QVERIFY(pvDatabase.isOpen());
  // Cleanup
  clearTestDatabaseData();
}

void mdtDatabaseTest::sqlTransactionTest()
{
  mdtSqlTransaction t(pvDatabase);

  // Initial state
  QVERIFY(!t.isStarted());
  // Begin
  QVERIFY(t.begin());
  QVERIFY(t.isStarted());
  QVERIFY(t.beginIfNotStarted());
  QVERIFY(t.isStarted());
  // Rollback
  QVERIFY(t.rollback());
  QVERIFY(!t.isStarted());
  // Begin
  QVERIFY(t.beginIfNotStarted());
  QVERIFY(t.isStarted());
  // Commit
  QVERIFY(t.commit());
  QVERIFY(!t.isStarted());

  // Check that we didn't close the default database
  QVERIFY(pvDatabase.isOpen());
}

void mdtDatabaseTest::sqlRelationInfoTest()
{
  mdtSqlRelationInfo relationInfo;

  /*
   * Initial state
   */
  QVERIFY(relationInfo.relationType() == mdtSqlRelationInfo::OneToMany);
  QVERIFY(relationInfo.parentTableName().isEmpty());
  QVERIFY(relationInfo.childTableName().isEmpty());
  QCOMPARE(relationInfo.items().size(), 0);
  /*
   * Setup a (fake) 1-1 relation
   */
  // Setup relation
  relationInfo.setRelationType(mdtSqlRelationInfo::OneToOne);
  relationInfo.setParentTableName("Client_tbl");
  relationInfo.setChildTableName("Address_tbl");
  relationInfo.addRelation("Id_PK", "Client_Id_FK", true);
  // Check
  QVERIFY(relationInfo.relationType() == mdtSqlRelationInfo::OneToOne);
  QCOMPARE(relationInfo.parentTableName(), QString("Client_tbl"));
  QCOMPARE(relationInfo.childTableName(), QString("Address_tbl"));
  QCOMPARE(relationInfo.items().size(), 1);
  /*
   * Check clear
   */
  relationInfo.clear();
  QVERIFY(relationInfo.relationType() == mdtSqlRelationInfo::OneToMany);
  QVERIFY(relationInfo.parentTableName().isEmpty());
  QVERIFY(relationInfo.childTableName().isEmpty());
  QCOMPARE(relationInfo.items().size(), 0);
}

void mdtDatabaseTest::sqlRelationTest()
{
  QSqlTableModel clientModel(0, pvDatabase);
  QSqlTableModel addressModel(0, pvDatabase);

  // Set database data
  clearTestDatabaseData();
  populateTestDatabase();

  /*
   * Setup models
   */
  // Setup client model
  clientModel.setTable("Client_tbl");
  QVERIFY(clientModel.select());
  QCOMPARE(clientModel.rowCount(), 2);
  // Setup address model
  addressModel.setTable("Address_tbl");
  QVERIFY(addressModel.select());
  QCOMPARE(addressModel.rowCount(), 3);


  // Cleanup
  clearTestDatabaseData();
  
  QFAIL("Test not implemented yet..");
}

void mdtDatabaseTest::doubleValueTest()
{
  QSqlQuery q(pvDatabase);

  // -infinity
  QVERIFY(q.prepare("INSERT INTO Value_tbl (Id_PK, Value) VALUES(:Id_PK, :Value)"));
  q.bindValue("Id_PK", 1);
  q.bindValue("Value", -std::numeric_limits<double>::infinity());
  QVERIFY(q.exec());
  QVERIFY(q.exec("SELECT Value FROM Value_tbl WHERE Id_PK = 1"));
  QVERIFY(q.next());
  QCOMPARE(q.value(0), QVariant(-std::numeric_limits<double>::infinity()));
  // +infinity
  QVERIFY(q.prepare("INSERT INTO Value_tbl (Id_PK, Value) VALUES(:Id_PK, :Value)"));
  q.bindValue("Id_PK", 2);
  q.bindValue("Value", std::numeric_limits<double>::infinity());
  QVERIFY(q.exec());
  QVERIFY(q.exec("SELECT Value FROM Value_tbl WHERE Id_PK = 2"));
  QVERIFY(q.next());
  QCOMPARE(q.value(0), QVariant(std::numeric_limits<double>::infinity()));

}


/*
 * Test data base manipulation methods
 */

void mdtDatabaseTest::createTestDatabase()
{
  QTemporaryFile tempFile;
  QSqlField fld;
  QString sql;

  // Create a temp file that will be used for database
  QVERIFY(tempFile.open());
  tempFile.close();
  pvDatabaseFileInfo.setFile(tempFile.fileName() + ".db");
  qDebug() << "Init, DB file: " << pvDatabaseFileInfo.filePath();

  // Open database
  pvDatabase = QSqlDatabase::addDatabase("QSQLITE");
  pvDatabase.setDatabaseName(pvDatabaseFileInfo.filePath());
  QVERIFY(pvDatabase.open());

  // QSqlQuery must be created after db.open() was called.
  QSqlQuery q(pvDatabase);

  /*
   * Enable foreing keys support
   */
  sql = "PRAGMA foreign_keys = ON";
  QVERIFY(q.exec(sql));

  /*
   * Create Client_tbl table
   */
  sql = "CREATE TABLE 'Client_tbl' (";
  sql += "'Id_PK' INTEGER PRIMARY KEY AUTOINCREMENT, ";
  sql += "'FirstName' VARCHAR(50) NOT NULL, ";
  sql += "'Remarks' VARCHAR(500) )";
  QVERIFY(q.exec(sql));
  // Verify some attributes
  fld = pvDatabase.record("Client_tbl").field("Id_PK");
  QVERIFY(fld.isAutoValue());
  fld = pvDatabase.record("Client_tbl").field("FirstName");
  QVERIFY(fld.requiredStatus() == QSqlField::Required);

  /*
   * Create Address_tbl table
   */
  sql = "CREATE TABLE 'Address_tbl' (";
  sql += "'Id_PK' INTEGER PRIMARY KEY AUTOINCREMENT, ";
  sql += "'StreetName' VARCHAR(50) NOT NULL, ";
  sql += "'StreetNumber' INTEGER NOT NULL, ";
  sql += "'Client_Id_FK' INTEGER NOT NULL, ";
  sql += "FOREIGN KEY(Client_Id_FK) REFERENCES Client_tbl(Id_PK) ";
  sql += "ON UPDATE RESTRICT ";
  sql += "ON DELETE RESTRICT ";
  sql += ");";
  QVERIFY(q.exec(sql));
  // Verify some attributes
  fld = pvDatabase.record("Address_tbl").field("Id_PK");
  QVERIFY(fld.isAutoValue());
  fld = pvDatabase.record("Address_tbl").field("StreetName");
  QVERIFY(fld.requiredStatus() == QSqlField::Required);
  fld = pvDatabase.record("Address_tbl").field("StreetNumber");
  QVERIFY(fld.requiredStatus() == QSqlField::Required);
  fld = pvDatabase.record("Address_tbl").field("Client_Id_FK");
  QVERIFY(fld.requiredStatus() == QSqlField::Required);

  /*
   * Create Value_tbl table
   */
  sql = "CREATE TABLE 'Value_tbl' (";
  sql += "'Id_PK' INTEGER PRIMARY KEY AUTOINCREMENT, ";
  sql += "'Value' DOUBLE NOT NULL, ";
  sql += "'Remarks' VARCHAR(500) )";
  QVERIFY(q.exec(sql));

}

void mdtDatabaseTest::populateTestDatabase()
{
  QSqlQuery query(pvDatabase);

  // Inert some data in Client_tbl
  QVERIFY(query.exec("INSERT INTO 'Client_tbl' ('Id_PK', 'FirstName') VALUES(1, 'Andy')"));
  QVERIFY(query.exec("INSERT INTO 'Client_tbl' ('Id_PK', 'FirstName', 'Remarks') VALUES(2, 'Bety', 'Remark on Bety')"));
  QVERIFY(query.exec("SELECT * FROM 'Client_tbl'"));
  while(query.next()){
    QVERIFY(!query.record().isEmpty());
  }
  // Inert some data into Address_tbl
  QVERIFY(query.exec("INSERT INTO 'Address_tbl' ('StreetName', 'StreetNumber', 'Client_Id_FK') VALUES('Andy street 1', 11 , 1)"));
  QVERIFY(query.exec("INSERT INTO 'Address_tbl' ('StreetName', 'StreetNumber', 'Client_Id_FK') VALUES('Andy street 2', 12 , 1)"));
  QVERIFY(query.exec("INSERT INTO 'Address_tbl' ('StreetName', 'StreetNumber', 'Client_Id_FK') VALUES('Bety street 1', 21 , 2)"));
  QVERIFY(query.exec("SELECT * FROM 'Address_tbl'"));
  while(query.next()){
    QVERIFY(!query.record().isEmpty());
  }
}

void mdtDatabaseTest::clearTestDatabaseData()
{
  QString sql;
  QSqlQuery query(pvDatabase);

  sql = "DELETE FROM Address_tbl";
  QVERIFY(query.exec(sql));
  sql = "DELETE FROM Client_tbl";
  QVERIFY(query.exec(sql));
  sql = "DELETE FROM Value_tbl";
  QVERIFY(query.exec(sql));
}

void mdtDatabaseTest::cleanupTestCase()
{
  QFile::remove(pvDatabaseFileInfo.filePath());
}

/*
 * Main
 */
int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtDatabaseTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}

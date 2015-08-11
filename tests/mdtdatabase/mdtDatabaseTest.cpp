/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include "mdtSqlSchemaTable.h"

#include "mdtSqlSchemaTableModel.h"

#include "mdtSqlFieldSetupWidget.h"

#include "mdtSqlRecord.h"
#include "mdtSqlRelation.h"
#include "mdtSqlRelationInfo.h"
#include "mdtSqlTransaction.h"
#include "mdtSqlDriverType.h"
#include "mdtSqlFieldType.h"
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
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QFile>
#include <QVariant>
#include <QTimer>
#include <QAbstractButton>
#include <QModelIndex>
#include <QList>
#include <limits>

#include <QTableView>
#include <QItemSelectionModel>

#include <QSignalSpy>

#include <QDebug>

void mdtDatabaseTest::initTestCase()
{
  createTestDatabase();
  clearTestDatabaseData();
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

  // Static functions - SQLite
  /// \todo To be done
  // Available field type list - SQLite
  fieldTypeList = mdtSqlFieldType::availableFieldTypeList(mdtSqlDriverType::SQLite);
  QCOMPARE(fieldTypeList.size(), 6);
  QVERIFY(fieldTypeList.at(0).type() == QVariant::Int);
  QCOMPARE(fieldTypeList.at(0).name(), QString("INTEGER"));
  QVERIFY(fieldTypeList.at(1).type() == QVariant::String);
  QCOMPARE(fieldTypeList.at(1).name(), QString("VARCHAR"));
  QVERIFY(fieldTypeList.at(2).type() == QVariant::Double);
  QCOMPARE(fieldTypeList.at(2).name(), QString("DOUBLE"));
  QVERIFY(fieldTypeList.at(3).type() == QVariant::Bool);
  QCOMPARE(fieldTypeList.at(3).name(), QString("BOOLEAN"));
  QVERIFY(fieldTypeList.at(4).type() == QVariant::Date);
  QCOMPARE(fieldTypeList.at(4).name(), QString("DATE"));
  QVERIFY(fieldTypeList.at(5).type() == QVariant::DateTime);
  QCOMPARE(fieldTypeList.at(5).name(), QString("DATETIME"));
  // Static functions - MySQL
  /// \todo To be done
  // Available field type list - MariaDB/MySQL
  /// \todo To be done
  // Initial state
  QVERIFY(ft.type() == QVariant::Invalid);
  QVERIFY(ft.isNull());
  // Set from type
  QVERIFY(ft.setType(QVariant::Int, mdtSqlDriverType::SQLite));
  QVERIFY(!ft.isNull());
  QVERIFY(ft.type() == QVariant::Int);
  QCOMPARE(ft.name(), QString("INTEGER"));
  QVERIFY(!ft.setType(QVariant::Color, mdtSqlDriverType::SQLite));
  QVERIFY(ft.type() == QVariant::Invalid);
  QVERIFY(ft.isNull());
  // Set a valid type before clear test
  QVERIFY(ft.setType(QVariant::String, mdtSqlDriverType::SQLite));
  QVERIFY(!ft.isNull());
  QVERIFY(ft.type() == QVariant::String);
  QCOMPARE(ft.name(), QString("VARCHAR"));
  // Clear
  ft.clear();
  QVERIFY(ft.type() == QVariant::Invalid);
  QVERIFY(ft.name().isEmpty());
  QVERIFY(ft.isNull());
}

void mdtDatabaseTest::sqlSchemaTableTest()
{
  mdtSqlSchemaTable st;
  QString expectedSql;
  QString expectedSql2; // For copy test, at end
  QSqlField field;

  // Initial
  QVERIFY(st.sqlForCreateTable().isEmpty());
  QVERIFY(st.sqlForDropTable().isEmpty());

  // Simple Maria DB/MySQL table
  st.setDriverName("QMYSQL");
  st.setTableName("Client_tbl");
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  st.addField(field, true);
  field = QSqlField();  // To clear field attributes (QSqlField::clear() only clear values)
  field.setLength(0);
  field.setName("Name");
  field.setType(QVariant::String);
  field.setLength(50);
  st.addField(field, false);
  QCOMPARE(st.field("Id_PK").name(), QString("Id_PK"));
  QCOMPARE(st.field("Id_PK").type(), QVariant::Int);
  expectedSql =  "CREATE TABLE `Client_tbl` (\n";
  expectedSql += "  `Id_PK` INT NOT NULL AUTO_INCREMENT,\n";
  expectedSql += "  `Name` VARCHAR(50) DEFAULT NULL,\n";
  expectedSql += "  PRIMARY KEY (`Id_PK`)\n";
  expectedSql += ");\n";
  QCOMPARE(st.sqlForCreateTable(), expectedSql);
  expectedSql = "DROP TABLE IF EXISTS `Client_tbl`;\n";
  QCOMPARE(st.sqlForDropTable(), expectedSql);

  // Clear
  st.clear();
  QVERIFY(st.sqlForCreateTable().isEmpty());
  QVERIFY(st.sqlForDropTable().isEmpty());

  // Simple Maria DB/MySQL table - We specifiy a database to use
  st.setDriverName("QMYSQL");
  st.setDatabaseName("sandbox");
  st.setTableName("Client_tbl");
  field = QSqlField();  // To clear field attributes (QSqlField::clear() only clear values)
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  st.addField(field, true);
  field = QSqlField();  // To clear field attributes (QSqlField::clear() only clear values)
  field.setName("Name");
  field.setType(QVariant::String);
  field.setLength(50);
  st.addField(field, false);
  expectedSql =  "CREATE TABLE `sandbox`.`Client_tbl` (\n";
  expectedSql += "  `Id_PK` INT NOT NULL AUTO_INCREMENT,\n";
  expectedSql += "  `Name` VARCHAR(50) DEFAULT NULL,\n";
  expectedSql += "  PRIMARY KEY (`Id_PK`)\n";
  expectedSql += ");\n";
  QCOMPARE(st.sqlForCreateTable(), expectedSql);
  expectedSql = "DROP TABLE IF EXISTS `sandbox`.`Client_tbl`;\n";
  QCOMPARE(st.sqlForDropTable(), expectedSql);

  /*
   * Simple Maria DB/MySQL Table:
   *  - Storage engine is specified
   *  - Charset is specified
   */
  st.clear();
  st.setDriverName("QMYSQL");
  st.setDatabaseName("sandbox");
  st.setStorageEngineName("InnoDB");
  st.setTableName("Client_tbl", "UTF8");
  field = QSqlField();  // To clear field attributes (QSqlField::clear() only clear values)
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  st.addField(field, true);
  field = QSqlField();  // To clear field attributes (QSqlField::clear() only clear values)
  field.setName("Name");
  field.setType(QVariant::String);
  field.setLength(50);
  st.addField(field, false);
  expectedSql  = "CREATE TABLE `sandbox`.`Client_tbl` (\n";
  expectedSql += "  `Id_PK` INT NOT NULL AUTO_INCREMENT,\n";
  expectedSql += "  `Name` VARCHAR(50) DEFAULT NULL,\n";
  expectedSql += "  PRIMARY KEY (`Id_PK`)\n";
  expectedSql += ") ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;\n";
  QCOMPARE(st.sqlForCreateTable(), expectedSql);
  expectedSql = "DROP TABLE IF EXISTS `sandbox`.`Client_tbl`;\n";
  QCOMPARE(st.sqlForDropTable(), expectedSql);

  /*
   * Second table that is child of Client_tbl
   */
  st.clear();
  st.setDriverName("QMYSQL");
  st.setDatabaseName("sandbox");
  st.setStorageEngineName("InnoDB");
  st.setTableName("Address_tbl", "UTF8");
  field = QSqlField();  // To clear field attributes (QSqlField::clear() only clear values)
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  st.addField(field, true);
  field = QSqlField();  // To clear field attributes (QSqlField::clear() only clear values)
  field.setName("Street");
  field.setType(QVariant::String);
  field.setLength(50);
  st.addField(field, false);
  field = QSqlField();  // To clear field attributes (QSqlField::clear() only clear values)
  field.setName("Client_Id_FK");
  field.setType(QVariant::Int);
  field.setRequiredStatus(QSqlField::Required);
  st.addField(field, false);
  st.addIndex("Client_Id_FK_IDX", false);
  QVERIFY(st.addFieldToIndex("Client_Id_FK_IDX", "Client_Id_FK"));
  st.addForeignKey("Client_Id_FK1", "Client_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  QVERIFY(st.addFieldToForeignKey("Client_Id_FK1", "Client_Id_FK", "Id_PK"));
  expectedSql  = "CREATE TABLE `sandbox`.`Address_tbl` (\n";
  expectedSql += "  `Id_PK` INT NOT NULL AUTO_INCREMENT,\n";
  expectedSql += "  `Street` VARCHAR(50) DEFAULT NULL,\n";
  expectedSql += "  `Client_Id_FK` INT NOT NULL DEFAULT NULL,\n";
  expectedSql += "  PRIMARY KEY (`Id_PK`),\n";
  expectedSql += "  INDEX `Client_Id_FK_IDX` (`Client_Id_FK`),\n";
  expectedSql += "  FOREIGN KEY `Client_Id_FK1` (`Client_Id_FK`)\n";
  expectedSql += "   REFERENCES `Client_tbl` (`Id_PK`)\n";
  expectedSql += "   ON DELETE RESTRICT\n";
  expectedSql += "   ON UPDATE CASCADE\n";
  expectedSql += ") ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;\n";
  QCOMPARE(st.sqlForCreateTable(), expectedSql);
  expectedSql = "DROP TABLE IF EXISTS `sandbox`.`Address_tbl`;\n";
  QCOMPARE(st.sqlForDropTable(), expectedSql);

  /*
   * Simple Sqlite Table:
   *  - Storage engine is specified (must be ignored)
   *  - Charset is specified
   */
  st.clear();
  st.setDriverName("QSQLITE");
  st.setDatabaseName("sandbox");
  st.setStorageEngineName("InnoDB");
  st.setTableName("Client_tbl", "UTF8");
  field = QSqlField();  // To clear field attributes (QSqlField::clear() only clear values)
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  st.addField(field, true);
  field = QSqlField();  // To clear field attributes (QSqlField::clear() only clear values)
  field.setName("Name");
  field.setType(QVariant::String);
  field.setLength(50);
  st.addField(field, false);
  expectedSql  = "CREATE TABLE 'sandbox'.'Client_tbl' (\n";
  expectedSql += "  'Id_PK' INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\n";
  expectedSql += "  'Name' VARCHAR(50) DEFAULT NULL COLLATE NOCASE\n";
  expectedSql += ");\n";
  QCOMPARE(st.sqlForCreateTable(), expectedSql);
  expectedSql = "DROP TABLE IF EXISTS 'sandbox'.'Client_tbl';\n";
  QCOMPARE(st.sqlForDropTable(), expectedSql);

  /*
   * Simple Sqlite Table:
   *  - Storage engine is specified (must be ignored)
   *  - Charset is specified
   *  - A NON UNIQUE index is added (must be ignored)
   *  - A UNIQUE index is added
   */
  st.clear();
  st.setDriverName("QSQLITE");
  st.setDatabaseName("sandbox");
  st.setStorageEngineName("InnoDB");
  st.setTableName("Client_tbl", "UTF8");
  field = QSqlField();  // To clear field attributes (QSqlField::clear() only clear values)
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  st.addField(field, true);
  field = QSqlField();  // To clear field attributes (QSqlField::clear() only clear values)
  field.setName("Name");
  field.setType(QVariant::String);
  field.setLength(50);
  st.addField(field, false);
  field = QSqlField();  // To clear field attributes (QSqlField::clear() only clear values)
  field.setName("NameU");
  field.setType(QVariant::String);
  field.setLength(50);
  st.addField(field, false);
  st.addIndex("Index", false);
  QVERIFY(st.addFieldToIndex("Index", "Name"));
  st.addIndex("IndexU", true);
  QVERIFY(st.addFieldToIndex("IndexU", "NameU"));
  expectedSql  = "CREATE TABLE 'sandbox'.'Client_tbl' (\n";
  expectedSql += "  'Id_PK' INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\n";
  expectedSql += "  'Name' VARCHAR(50) DEFAULT NULL COLLATE NOCASE,\n";
  expectedSql += "  'NameU' VARCHAR(50) DEFAULT NULL COLLATE NOCASE,\n";
  expectedSql += "  UNIQUE ('NameU')\n";
  expectedSql += ");\n";
  QCOMPARE(st.sqlForCreateTable(), expectedSql);
  expectedSql = "DROP TABLE IF EXISTS 'sandbox'.'Client_tbl';\n";
  QCOMPARE(st.sqlForDropTable(), expectedSql);

  /*
   * Second table for Sqlite that is child of Client_tbl
   *  Note: we added a non existant foreing key (had a bug when more than one was declared)
   */
  st.clear();
  st.setDriverName("QSQLITE");
  st.setDatabaseName("sandbox");
  st.setStorageEngineName("InnoDB");
  st.setTableName("Address_tbl", "UTF8");
  field = QSqlField();  // To clear field attributes (QSqlField::clear() only clear values)
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  st.addField(field, true);
  field = QSqlField();  // To clear field attributes (QSqlField::clear() only clear values)
  field.setName("Street");
  field.setType(QVariant::String);
  field.setLength(50);
  st.addField(field, false);
  field = QSqlField();  // To clear field attributes (QSqlField::clear() only clear values)
  field.setName("Client_Id_FK");
  field.setType(QVariant::Int);
  field.setRequiredStatus(QSqlField::Required);
  st.addField(field, false);
  st.addIndex("Client_Id_FK_IDX", false);
  QVERIFY(st.addFieldToIndex("Client_Id_FK_IDX", "Client_Id_FK"));
  st.addForeignKey("Client_Id_FK1", "Client_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  QVERIFY(st.addFieldToForeignKey("Client_Id_FK1", "Client_Id_FK", "Id_PK"));
  st.addForeignKey("Client_Id_FK2", "Client_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  QVERIFY(st.addFieldToForeignKey("Client_Id_FK2", "Client_Id_FK", "Id_PK"));
  expectedSql  = "CREATE TABLE 'sandbox'.'Address_tbl' (\n";
  expectedSql += "  'Id_PK' INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\n";
  expectedSql += "  'Street' VARCHAR(50) DEFAULT NULL COLLATE NOCASE,\n";
  expectedSql += "  'Client_Id_FK' INTEGER NOT NULL DEFAULT NULL,\n";
  expectedSql += "  FOREIGN KEY ('Client_Id_FK')\n";
  expectedSql += "   REFERENCES 'Client_tbl' ('Id_PK')\n";
  expectedSql += "   ON DELETE RESTRICT\n";
  expectedSql += "   ON UPDATE CASCADE,\n";
  expectedSql += "  FOREIGN KEY ('Client_Id_FK')\n";
  expectedSql += "   REFERENCES 'Client_tbl' ('Id_PK')\n";
  expectedSql += "   ON DELETE RESTRICT\n";
  expectedSql += "   ON UPDATE CASCADE\n";
  expectedSql += ");\n";
  QCOMPARE(st.sqlForCreateTable(), expectedSql);
  expectedSql2 = "DROP TABLE IF EXISTS 'sandbox'.'Address_tbl';\n";
  QCOMPARE(st.sqlForDropTable(), expectedSql2);

  // Copy test
  mdtSqlSchemaTable st2;
  QCOMPARE(st2.sqlForCreateTable(), QString());
  QCOMPARE(st2.sqlForDropTable(), QString());
  st2 = st;
  QCOMPARE(st2.sqlForCreateTable(), expectedSql);
  QCOMPARE(st2.sqlForDropTable(), expectedSql2);

  /*
   * Simple Sqlite temporary Table:
   *  - Storage engine is specified (must be ignored)
   *  - Charset is specified
   */
  st.clear();
  st.setDriverName("QSQLITE");
  st.setDatabaseName("sandbox");
  st.setStorageEngineName("InnoDB");
  st.setTableName("Client_tbl", "UTF8");
  st.setTableTemporary(true);
  field = QSqlField();  // To clear field attributes (QSqlField::clear() only clear values)
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  st.addField(field, true);
  field = QSqlField();  // To clear field attributes (QSqlField::clear() only clear values)
  field.setName("Name");
  field.setType(QVariant::String);
  field.setLength(50);
  st.addField(field, false);
  expectedSql  = "CREATE TEMPORARY TABLE 'sandbox'.'Client_tbl' (\n";
  expectedSql += "  'Id_PK' INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\n";
  expectedSql += "  'Name' VARCHAR(50) DEFAULT NULL COLLATE NOCASE\n";
  expectedSql += ");\n";
  QCOMPARE(st.sqlForCreateTable(), expectedSql);
  expectedSql = "DROP TABLE IF EXISTS 'sandbox'.'Client_tbl';\n";
  QCOMPARE(st.sqlForDropTable(), expectedSql);

}

void mdtDatabaseTest::sqlSchemaTableSqliteTest()
{
  // Try to really create a table with Sqlite
  QTemporaryFile tempFile;
  QFileInfo dbFileInfo;
  QSqlDatabase db;
  QString sql;
  mdtSqlSchemaTable st;
  QSqlRecord record;
  QSqlField field;

  // Create a temp file that will be used for database
  QVERIFY(tempFile.open());
  tempFile.close();
  dbFileInfo.setFile(tempFile.fileName() + ".db");
  qDebug() << "DB file: " << dbFileInfo.filePath();

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
  st.setDriverName(db.driverName());
  ///st.setDatabaseName(db.databaseName());
  st.setStorageEngineName("InnoDB");  // Must be ignored
  st.setTableName("Client_tbl", "UTF8");
  field = QSqlField();  // To clear field attributes (QSqlField::clear() only clear values)
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  st.addField(field, true);
  field = QSqlField();  // To clear field attributes (QSqlField::clear() only clear values)
  field.setName("Name");
  field.setType(QVariant::String);
  field.setLength(50);
  st.addField(field, false);
  st.addIndex("Name_idx", true);
  QVERIFY(st.addFieldToIndex("Name_idx", "Name"));
  sql = st.sqlForDropTable();
  QVERIFY(q.exec(sql));
  sql = st.sqlForCreateTable();
  QVERIFY(q.exec(sql));
  // Check that table was correctly created
  QVERIFY(db.tables().contains("Client_tbl"));
  record = db.record("Client_tbl");
  QCOMPARE(record.count(), 2);
  field = record.field(0);
  QCOMPARE(field.name(), QString("Id_PK"));
  QCOMPARE(field.type(), QVariant::Int);
  QVERIFY(field.isAutoValue());
  field = record.field(1);
  QCOMPARE(field.name(), QString("Name"));
  QCOMPARE(field.type(), QVariant::String);

  /*
   * Create second table that is child of Client_tbl
   */
  st.clear();
  st.setDriverName(db.driverName());
  ///st.setDatabaseName("sandbox");
  st.setStorageEngineName("InnoDB");
  st.setTableName("Address_tbl", "UTF8");
  field = QSqlField();  // To clear field attributes (QSqlField::clear() only clear values)
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  st.addField(field, true);
  field = QSqlField();  // To clear field attributes (QSqlField::clear() only clear values)
  field.setName("Street");
  field.setType(QVariant::String);
  field.setLength(50);
  st.addField(field, false);
  field = QSqlField();  // To clear field attributes (QSqlField::clear() only clear values)
  field.setName("Client_Id_FK");
  field.setType(QVariant::Int);
  field.setRequiredStatus(QSqlField::Required);
  st.addField(field, false);
  st.addIndex("Client_Id_FK_IDX", false);
  QVERIFY(st.addFieldToIndex("Client_Id_FK_IDX", "Client_Id_FK"));
  st.addForeignKey("Client_Id_FK1", "Client_tbl", mdtSqlSchemaTable::Restrict, mdtSqlSchemaTable::Cascade);
  QVERIFY(st.addFieldToForeignKey("Client_Id_FK1", "Client_Id_FK", "Id_PK"));
  sql = st.sqlForDropTable();
  QVERIFY(q.exec(sql));
  sql = st.sqlForCreateTable();
  QVERIFY(q.exec(sql));
  // Check that table was correctly created
  QVERIFY(db.tables().contains("Address_tbl"));
  record = db.record("Address_tbl");
  QCOMPARE(record.count(), 3);
  field = record.field(0);
  QCOMPARE(field.name(), QString("Id_PK"));
  QCOMPARE(field.type(), QVariant::Int);
  QVERIFY(field.isAutoValue());
  field = record.field(1);
  QCOMPARE(field.name(), QString("Street"));
  QCOMPARE(field.type(), QVariant::String);
  field = record.field(2);
  QCOMPARE(field.name(), QString("Client_Id_FK"));
  QCOMPARE(field.type(), QVariant::Int);

  // Try to insert some data
  sql = "INSERT INTO Client_tbl (Name) VALUES ('Name 1')";
  QVERIFY(q.exec(sql));
  sql = "INSERT INTO Client_tbl (Id_PK, Name) VALUES (2, 'Name 2')";
  QVERIFY(q.exec(sql));
  sql = "INSERT INTO Address_tbl (Street, Client_Id_FK) VALUES ('Street 1', 2)";
  QVERIFY(q.exec(sql));
  sql = "INSERT INTO Address_tbl (Street, Client_Id_FK) VALUES ('Street 2', 3)";
  QVERIFY(!q.exec(sql));

  /*
   * Create Client2_tbl witch has the same structure than Client_tbl
   */
  st.clear();
  QCOMPARE(st.fieldCount(), 0);
  QVERIFY(st.setupFromTable("Client_tbl", db));
  st.setTableName("Client2_tbl");
  st.setTableTemporary(true);
  QCOMPARE(st.field("Id_PK").name(), QString("Id_PK"));
  QCOMPARE(st.field("Id_PK").type(), QVariant::Int);
  QCOMPARE(st.field("Name").name(), QString("Name"));
  QCOMPARE(st.field("Name").type(), QVariant::String);
  sql = st.sqlForDropTable();
  QVERIFY(q.exec(sql));
  sql = st.sqlForCreateTable();
  QVERIFY(q.exec(sql));
  // Check that table was correctly created
  QVERIFY(db.tables().contains("Client2_tbl"));
  record = db.record("Client2_tbl");
  QCOMPARE(record.count(), 2);
  field = record.field(0);
  QCOMPARE(field.name(), QString("Id_PK"));
  QCOMPARE(field.type(), QVariant::Int);
  QVERIFY(field.isAutoValue());
  field = record.field(1);
  QCOMPARE(field.name(), QString("Name"));
  QCOMPARE(field.type(), QVariant::String);

  /*
   * Create Address2_tbl witch has the same structure than Address_tbl
   */
  QVERIFY(st.setupFromTable("Address_tbl", db));
  st.setTableName("Address2_tbl");
  st.setTableTemporary(true);
  st.updateForeignKeyReferingTable("Client_Id_FK_fk", "Client2_tbl");
  QCOMPARE(st.field("Id_PK").name(), QString("Id_PK"));
  QCOMPARE(st.field("Id_PK").type(), QVariant::Int);
  QCOMPARE(st.field("Street").name(), QString("Street"));
  QCOMPARE(st.field("Street").type(), QVariant::String);
  QCOMPARE(st.field("Client_Id_FK").name(), QString("Client_Id_FK"));
  QCOMPARE(st.field("Client_Id_FK").type(), QVariant::Int);
  sql = st.sqlForDropTable();
  QVERIFY(q.exec(sql));
  sql = st.sqlForCreateTable();
  QVERIFY(q.exec(sql));
  // Check that table was correctly created
  QVERIFY(db.tables().contains("Address2_tbl"));
  record = db.record("Address2_tbl");
  QCOMPARE(record.count(), 3);
  field = record.field(0);
  QCOMPARE(field.name(), QString("Id_PK"));
  QCOMPARE(field.type(), QVariant::Int);
  QVERIFY(field.isAutoValue());
  field = record.field(1);
  QCOMPARE(field.name(), QString("Street"));
  QCOMPARE(field.type(), QVariant::String);
  field = record.field(2);
  QCOMPARE(field.name(), QString("Client_Id_FK"));
  QCOMPARE(field.type(), QVariant::Int);

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
  st.clear();
  st.setDriverName(db.driverName());
  st.setTableName("DataTypeTest_tbl", "UTF8");
  field = QSqlField();
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  st.addField(field, true);
  // String type
  field = QSqlField();
  field.setName("StringType");
  field.setType(QVariant::String);
  field.setLength(50);
  st.addField(field, false);
  // Int type
  field = QSqlField();
  field.setName("IntType");
  field.setType(QVariant::Int);
  st.addField(field, false);
  // Double type
  field = QSqlField();
  field.setName("DoubleType");
  field.setType(QVariant::Double);
  st.addField(field, false);
  // Date type
  field = QSqlField();
  field.setName("DateType");
  field.setType(QVariant::Date);
  st.addField(field, false);
  // Time type
  field = QSqlField();
  field.setName("TimeType");
  field.setType(QVariant::Time);
  st.addField(field, false);
  // DateTime type
  field = QSqlField();
  field.setName("DateTimeType");
  field.setType(QVariant::DateTime);
  st.addField(field, false);
  // Create table
  sql = st.sqlForCreateTable();
  QVERIFY(q.exec(sql));
  // Clear mdtSqlSchemaTable for next step
  st.clear();
  field = st.field("Id_PK");
  QVERIFY(!field.isValid());
  field = st.field("StringType");
  QVERIFY(!field.isValid());
  field = st.field("IntType");
  QVERIFY(!field.isValid());
  field = st.field("DoubleType");
  QVERIFY(!field.isValid());
  field = st.field("DateType");
  QVERIFY(!field.isValid());
  // Check that table was correctly created
  QVERIFY(db.tables().contains("DataTypeTest_tbl"));
  record = db.record("DataTypeTest_tbl");
  QCOMPARE(record.count(), 7);
  field = record.field(0);
  QCOMPARE(field.name(), QString("Id_PK"));
  QCOMPARE(field.type(), QVariant::Int);
  QVERIFY(field.isAutoValue());
  field = record.field(1);
  QCOMPARE(field.name(), QString("StringType"));
  QCOMPARE(field.type(), QVariant::String);
  field = record.field(2);
  QCOMPARE(field.name(), QString("IntType"));
  QCOMPARE(field.type(), QVariant::Int);
  field = record.field(3);
  QCOMPARE(field.name(), QString("DoubleType"));
  QCOMPARE(field.type(), QVariant::Double);
  field = record.field(4);
  // Now check that mdtSqlSchemaTable fetches existing table correctly
  QVERIFY(st.setupFromTable("DataTypeTest_tbl", db));
  field = st.field("Id_PK");
  QCOMPARE(field.name(), QString("Id_PK"));
  QCOMPARE(field.type(), QVariant::Int);
  QVERIFY(field.isAutoValue());
  QVERIFY(field.requiredStatus() == QSqlField::Required);
  field = st.field("StringType");
  QCOMPARE(field.name(), QString("StringType"));
  QCOMPARE(field.type(), QVariant::String);
  QVERIFY(!field.isAutoValue());
  QVERIFY(field.requiredStatus() == QSqlField::Optional);
  field = st.field("IntType");
  QCOMPARE(field.name(), QString("IntType"));
  QCOMPARE(field.type(), QVariant::Int);
  QVERIFY(!field.isAutoValue());
  QVERIFY(field.requiredStatus() == QSqlField::Optional);
  field = st.field("DoubleType");
  QCOMPARE(field.name(), QString("DoubleType"));
  QCOMPARE(field.type(), QVariant::Double);
  QVERIFY(!field.isAutoValue());
  QVERIFY(field.requiredStatus() == QSqlField::Optional);
  field = st.field("DateType");
  QCOMPARE(field.name(), QString("DateType"));
  QCOMPARE(field.type(), QVariant::Date);
  QVERIFY(!field.isAutoValue());
  QVERIFY(field.requiredStatus() == QSqlField::Optional);
  field = st.field("TimeType");
  QCOMPARE(field.name(), QString("TimeType"));
  QCOMPARE(field.type(), QVariant::DateTime);
  QVERIFY(!field.isAutoValue());
  QVERIFY(field.requiredStatus() == QSqlField::Optional);
  field = st.field("DateTimeType");
  QCOMPARE(field.name(), QString("DateTimeType"));
  QCOMPARE(field.type(), QVariant::DateTime);
  QVERIFY(!field.isAutoValue());
  QVERIFY(field.requiredStatus() == QSqlField::Optional);
  // Drop table
  sql = st.sqlForDropTable();
  QVERIFY(q.exec(sql));
  QVERIFY(!db.tables().contains("DataTypeTest_tbl"));

}

void mdtDatabaseTest::sqlSchemaTableModelTest()
{
  mdtSqlSchemaTableModel model;
  mdtSqlSchemaTable st;
  QTableView tableView;
  QTreeView treeView;
  QComboBox cb;
///  QList<QSqlField> fields;
  QSqlField field;
///  QSqlIndex pk;

  /*
   * Setup fields
   */
  // Id_PK
  field = QSqlField();
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  st.addField(field, true);
//   fields.append(field);
//   pk.append(field);
  // Name
  field = QSqlField();
  field.setName("Name");
  field.setType(QVariant::String);
  field.setLength(100);
  st.addField(field, false);
//   fields.append(field);
  /*
   * Setup model and views
   */
  model.setTableSchema(st);
//   model.setFieldList(fields);
//   model.setPrimaryKey(pk);
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
  QVERIFY(!model.field(-1).isValid());
  QVERIFY(model.field(0).isValid());
  QCOMPARE(model.field(0).name(), QString("Id_PK"));
  QVERIFY(model.isPartOfPrimaryKey(0));
  QVERIFY(model.field(1).isValid());
  QCOMPARE(model.field(1).name(), QString("Name"));
  QVERIFY(!model.isPartOfPrimaryKey(1));
  QVERIFY(!model.field(20).isValid());
  QVERIFY(!model.isPartOfPrimaryKey(20));
  // Check with combo box
  cb.setCurrentIndex(0);
  QVERIFY(model.currentField(&cb).isValid());
  QCOMPARE(model.currentField(&cb).name(), QString("Id_PK"));
  cb.setCurrentIndex(1);
  QVERIFY(model.currentField(&cb).isValid());
  QCOMPARE(model.currentField(&cb).name(), QString("Name"));
  cb.setCurrentIndex(-1);
  QVERIFY(!model.currentField(&cb).isValid());

  /*
   * Play
   */
  
  while(tableView.isVisible()){
    QTest::qWait(500);
  }
}

void mdtDatabaseTest::sqlFieldSetupWidgetTest()
{
  mdtSqlFieldSetupWidget w(nullptr);
  mdtSqlFieldSetupData data;

  data.tableName = "Client_tbl";
  data.editionMode = mdtSqlFieldSetupEditionMode_t::Edition;
  QVERIFY(w.setData(data, pvDatabase));
  w.show();

  /*
   * Play
   */
  while(w.isVisible()){
    QTest::qWait(500);
  }
}

void mdtDatabaseTest::databaseManagerTest()
{
  mdtSqlDatabaseManager m;
  QSqlDatabase db;
  mdtSqlSchemaTable st;
  QSqlField field;

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
  field = QSqlField();  // To clear field attributes (QSqlField::clear() only clear values)
  field.setName("Id_PK");
  field.setType(QVariant::Int);
  field.setAutoValue(true);
  st.addField(field, true);
  field = QSqlField();  // To clear field attributes (QSqlField::clear() only clear values)
  field.setName("Name");
  field.setType(QVariant::String);
  field.setLength(50);
  st.addField(field, false);
  // Fisrt creation
  QVERIFY(m.createTable(st, mdtSqlDatabaseManager::FailIfExists));
  QVERIFY(m.database().tables().contains("Client_tbl"));
  // Check overwite
  QVERIFY(m.createTable(st, mdtSqlDatabaseManager::OverwriteExisting));
  QVERIFY(m.database().tables().contains("Client_tbl"));
  
  // Cleanup
  m.database().close();
  QFile::remove(dbFileInfo.filePath());
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

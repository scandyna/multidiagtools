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

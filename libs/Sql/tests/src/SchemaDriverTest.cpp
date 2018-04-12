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
#include "Mdt/Sql/Schema/Driver.h"
#include "Mdt/Sql/Schema/DriverSQLite.h"
#include <QSqlDatabase>
#include <QSqlQuery>


void SchemaDriverTest::initTestCase()
{
  QSqlDatabase::addDatabase("QSQLITE", "SQLITE_1");
//   QSqlDatabase::addDatabase("QMYSQL", "MYSQL_1");
}

void SchemaDriverTest::cleanupTestCase()
{
  QSqlDatabase::removeDatabase("SQLITE_1");
//   QSqlDatabase::removeDatabase("MYSQL_1");
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
  QVERIFY(Driver::typeFromDbmsType(QSqlDriver::PostgreSQL) == DriverType::PostgreSQL);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  SchemaDriverTest test;

  return QTest::qExec(&test, argc, argv);
}

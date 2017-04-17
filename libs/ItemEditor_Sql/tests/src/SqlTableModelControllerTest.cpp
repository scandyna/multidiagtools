/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#include "SqlTableModelControllerTest.h"
#include "SqlTableModelControllerTester.h"
#include "Schema/Client.h"
#include "Schema/Address.h"
#include "Mdt/Sql/Schema/Driver.h"
#include <QSqlQuery>

using namespace Mdt::ItemEditor;
namespace Sql = Mdt::Sql;
using Schema::Client;
using Schema::Address;
using Schema::ClientAddressSchema;
using Schema::ClientPopulation;

void SqlTableModelControllerTest::initTestCase()
{
  if(!initDatabaseSqlite()){
    QSKIP("Could not init database - Skip all tests");  // Will also skip all tests
  }
  createSchema(ClientAddressSchema());
}

void SqlTableModelControllerTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void SqlTableModelControllerTest::sandbox()
{
  qDebug() << "Tables: " << database().tables();
}

/*
 * Helpers
 */

void SqlTableModelControllerTest::createSchema(const Schema::ClientAddressSchema& schema)
{
  Sql::Schema::Driver driver(database());

  QVERIFY(driver.isValid());
  QVERIFY(driver.dropSchema(schema));
  QVERIFY(driver.createSchema(schema));
}

bool SqlTableModelControllerTest::deleteClientData()
{
  QSqlQuery query(database());
  return query.exec("DELETE FROM Client_tbl");
}

bool SqlTableModelControllerTest::repopulateClientTable(const Schema::ClientPopulation& tp)
{
  Sql::Schema::Driver driver(database());

  Q_ASSERT(driver.isValid());
  if(!deleteClientData()){
    return false;
  }
  return driver.populateTable(tp);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  SqlTableModelControllerTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}

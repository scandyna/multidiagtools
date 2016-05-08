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
#include "QueryTest.h"
#include "TestDatabaseSchema.h"
#include "mdtApplication.h"
#include "Mdt/Sql/Query.h"

#include <QDebug>

void QueryTest::initTestCase()
{
  /*
   * Init and open database
   */
  pvDatabase = QSqlDatabase::addDatabase("QSQLITE");
  QVERIFY(pvDatabase.isValid());
  QVERIFY(pvTempFile.open());
  pvTempFile.close();
  pvDatabase.setDatabaseName(pvTempFile.fileName());
  QVERIFY(pvDatabase.open());
  /*
   * Create schema
   */
  TestDatabaseSchema schema;
  QVERIFY(schema.createSchema(pvDatabase));
}

void QueryTest::cleanupTestCase()
{
  pvDatabase.close();
}

/*
 * Tests
 */


/*
 * Main
 */

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  QueryTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}

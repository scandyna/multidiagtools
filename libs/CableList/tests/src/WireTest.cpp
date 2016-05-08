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
#include "WireTest.h"
#include "mdtApplication.h"
#include "Mdt/CableList/DatabaseSchema.h"
#include "Mdt/CableList/WirePk.h"

void WireTest::initTestCase()
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
  createDatabaseSchema();
}

void WireTest::cleanupTestCase()
{
  pvDatabase.close();
}

/*
 * Tests
 */

void WireTest::pkTest()
{
  using Mdt::CableList::WirePk;

  /*
   * Construction
   */
  // Default constructed
  WirePk pk;
  QVERIFY(pk.isNull());
  // Constrcut with a id
  WirePk pk2(1);
  QVERIFY(!pk2.isNull());
  QCOMPARE(pk2.id(), 1);
  /*
   * Set
   */
  QVERIFY(pk.isNull());
  pk.setId(5);
  QVERIFY(!pk.isNull());
  QCOMPARE(pk.id(), 5);
  /*
   * Clear
   */
  QVERIFY(!pk.isNull());
  pk.clear();
  QVERIFY(pk.isNull());
  /*
   * Get from QVariant
   */
  QVERIFY(pk.isNull());
  pk = WirePk::fromQVariant(2);
  QVERIFY(!pk.isNull());
  QCOMPARE(pk.id(), 2);
  pk = WirePk::fromQVariant(QVariant());
  QVERIFY(pk.isNull());
}

/*
 * Helper functions
 */

void WireTest::createDatabaseSchema()
{
  Mdt::CableList::DatabaseSchema schema;

  QVERIFY(pvDatabase.isOpen());
  QVERIFY(schema.createSchema(pvDatabase));
  QVERIFY(schema.checkSchema(pvDatabase));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  WireTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}

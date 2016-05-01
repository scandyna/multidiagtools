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
#include "UnitConnectionTest.h"
#include "mdtApplication.h"
#include "Mdt/CableList/DatabaseSchema.h"
#include "Mdt/CableList/UnitConnectionPk.h"
#include "Mdt/CableList/UnitConnectionPkList.h"

void UnitConnectionTest::initTestCase()
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

void UnitConnectionTest::cleanupTestCase()
{
  pvDatabase.close();
}

/*
 * Tests
 */

void UnitConnectionTest::pkTest()
{
  using Mdt::CableList::UnitConnectionPk;

  /*
   * Constructions
   */
  UnitConnectionPk pk;
  QVERIFY(pk.isNull());
  UnitConnectionPk pk2(1);
  QVERIFY(!pk2.isNull());
  QCOMPARE((int)pk2.id(), 1);
  /*
   * Set
   */
  QVERIFY(pk.isNull());
  pk.setId(10);
  QVERIFY(!pk.isNull());
  QCOMPARE((int)pk.id(), 10);
  /*
   * Clear
   */
  pk.clear();
  QVERIFY(pk.isNull());
}

void UnitConnectionTest::pkListTest()
{
  using Mdt::CableList::UnitConnectionPk;
  using Mdt::CableList::UnitConnectionPkList;

  UnitConnectionPkList pkList;

  /*
   * Initial state
   */
  QCOMPARE(pkList.size(), 0);
  QVERIFY(pkList.isEmpty());
  /*
   * Add
   */
  pkList.append(UnitConnectionPk(1));
  QCOMPARE(pkList.size(), 1);
  QVERIFY(!pkList.isEmpty());
  QCOMPARE((int)pkList.at(0).id(), 1);
  for(const auto pk : pkList){
    QCOMPARE((int)pk.id(), 1);
  }
  /*
   * Clear
   */
  pkList.clear();
  QCOMPARE(pkList.size(), 0);
  QVERIFY(pkList.isEmpty());
}

void UnitConnectionTest::pkListBenchmark()
{
  using Mdt::CableList::UnitConnectionPk;
  using Mdt::CableList::UnitConnectionPkList;

  const int N = 1000;

  QBENCHMARK{
    UnitConnectionPkList pkList;
    for(int i = 0; i < N; ++i){
      pkList.append(UnitConnectionPk(i));
      QCOMPARE(pkList.size(), i+1);
      QCOMPARE((int)pkList.at(i).id(), i);
    }
    auto pkListCopy = pkList;
    QCOMPARE(pkListCopy.size(), N);
    QCOMPARE((int)pkListCopy.at(N-1).id(), N-1);
  }
}

/*
 * Helper functions
 */

void UnitConnectionTest::createDatabaseSchema()
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
  UnitConnectionTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}

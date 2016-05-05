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
#include "LinkTest.h"
#include "mdtApplication.h"
#include "Mdt/CableList/DatabaseSchema.h"
#include "Mdt/CableList/LinkVersionPk.h"
#include "Mdt/CableList/LinkPk.h"
// #include "Mdt/CableList/UnitConnectionPk.h"
// #include "Mdt/CableList/UnitConnectionPkList.h"

void LinkTest::initTestCase()
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

void LinkTest::cleanupTestCase()
{
  pvDatabase.close();
}

/*
 * Tests
 */

void LinkTest::versionPkTest()
{
  using Mdt::CableList::LinkVersionPk;

  /*
   * Constructions
   */
  LinkVersionPk pk;
  QVERIFY(pk.isNull());
  LinkVersionPk pk1(1);
  QVERIFY(!pk1.isNull());
  QCOMPARE(pk1.version(), 1);
  /*
   * Set
   */
  QVERIFY(pk.isNull());
  pk.setVersion(25);
  QVERIFY(!pk.isNull());
  QCOMPARE(pk.version(), 25);
  /*
   * Get from QVariant
   */
  pk = LinkVersionPk::fromQVariant(52);
  QVERIFY(!pk.isNull());
  QCOMPARE(pk.version(), 52);
  pk = LinkVersionPk::fromQVariant(QVariant());
  QVERIFY(pk.isNull());
  /*
   * Clear
   */
  pk.setVersion(22);
  QVERIFY(!pk.isNull());
  pk.clear();
  QVERIFY(pk.isNull());
  /*
   * Comparisons
   */
  // lsh and rhs are null
  QVERIFY(!(LinkVersionPk() == LinkVersionPk()));
  QVERIFY(LinkVersionPk() != LinkVersionPk());
  // lhs is null
  QVERIFY(!(LinkVersionPk() == LinkVersionPk(1)));
  QVERIFY(LinkVersionPk() != LinkVersionPk(1));
  // rhs is null
  QVERIFY(!(LinkVersionPk(1) == LinkVersionPk()));
  QVERIFY(LinkVersionPk(1) != LinkVersionPk());
  // lhs == rhs
  QVERIFY(LinkVersionPk(1) == LinkVersionPk(1));
  QVERIFY(!(LinkVersionPk(1) != LinkVersionPk(1)));
  // lhs != rhs
  QVERIFY(!(LinkVersionPk(1) == LinkVersionPk(2)));
  QVERIFY(LinkVersionPk(1) != LinkVersionPk(2));
}

void LinkTest::linkPkTest()
{
  using Mdt::CableList::LinkPk;
  using Mdt::CableList::UnitConnectionPk;
  using Mdt::CableList::LinkVersionPk;
  using Mdt::CableList::ModificationType;
  using Mdt::CableList::ModificationPk;

  LinkPk pk;

  /*
   * Initial state
   */
  QVERIFY(pk.isNull());
  /*
   * Set
   */
  pk.setConnectionStart(UnitConnectionPk(1));
  QVERIFY(pk.isNull());
  QCOMPARE(pk.connectionStart().id(), 1);
  pk.setConnectionEnd(UnitConnectionPk(2));
  QVERIFY(pk.isNull());
  QCOMPARE(pk.connectionEnd().id(), 2);
  pk.setVersion(LinkVersionPk(100));
  QVERIFY(pk.isNull());
  QCOMPARE(pk.version().version(), 100);
  pk.setModification(ModificationPk(ModificationType::New));
  QVERIFY(pk.modification().type() == ModificationType::New);
  QVERIFY(!pk.isNull());
  /*
   * Clear
   */
  QVERIFY(!pk.isNull());
  pk.clear();
  QVERIFY(pk.connectionStart().isNull());
  QVERIFY(pk.connectionEnd().isNull());
  QVERIFY(pk.version().isNull());
  QVERIFY(pk.modification().isNull());
  /*
   * Clear start/end connection
   */
  pk.setConnectionStart(UnitConnectionPk(1));
  pk.setConnectionEnd(UnitConnectionPk(2));
  pk.setVersion(LinkVersionPk(100));
  pk.setModification(ModificationPk(ModificationType::New));
  QVERIFY(!pk.isNull());
  // Clear start connection
  pk.clearConnectionStart();
  QVERIFY(pk.connectionStart().isNull());
  QVERIFY(!pk.connectionEnd().isNull());
  QVERIFY(pk.isNull());
  pk.setConnectionStart(UnitConnectionPk(1));
  QVERIFY(!pk.isNull());
  // Clear end connection
  pk.clearConnectionEnd();
  QVERIFY(!pk.connectionStart().isNull());
  QVERIFY(pk.connectionEnd().isNull());
  QVERIFY(pk.isNull());
  /*
   * Equality comparison
   */
  LinkPk lhs, rhs;
  // lsh and rhs are null
  QVERIFY(!(lhs == rhs));
  QVERIFY(lhs != rhs);
  // lhs is null
  lhs.clear();
  rhs.setConnectionStart(UnitConnectionPk(1));
  rhs.setConnectionEnd(UnitConnectionPk(2));
  rhs.setVersion(LinkVersionPk(100));
  rhs.setModification(ModificationPk(ModificationType::New));
  QVERIFY(!(lhs == rhs));
  QVERIFY(lhs != rhs);
  // rhs is null
  lhs.setConnectionStart(UnitConnectionPk(1));
  lhs.setConnectionEnd(UnitConnectionPk(2));
  lhs.setVersion(LinkVersionPk(100));
  lhs.setModification(ModificationPk(ModificationType::New));
  rhs.clear();
  QVERIFY(!(lhs == rhs));
  QVERIFY(lhs != rhs);
  // lhs == rhs
  rhs = lhs;
  QVERIFY(lhs == rhs);
  QVERIFY(!(lhs != rhs));
  // lhs != rhs
  rhs.setConnectionEnd(UnitConnectionPk(20));
  QVERIFY(!(lhs == rhs));
  QVERIFY(lhs != rhs);
}

void LinkTest::linkPkListTest()
{
  
}

void LinkTest::linkPkListBenchmark()
{
  
}

/*
 * Helper functions
 */

void LinkTest::createDatabaseSchema()
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
  LinkTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}

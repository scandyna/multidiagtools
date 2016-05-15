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
#include "ModificationTest.h"
#include "Mdt/Application.h"
#include "Mdt/CableList/DatabaseSchema.h"
#include "Mdt/CableList/ModificationPk.h"
#include "Mdt/CableList/ModificationModel.h"

void ModificationTest::initTestCase()
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

void ModificationTest::cleanupTestCase()
{
  pvDatabase.close();
}

/*
 * Tests
 */

void ModificationTest::pkTest()
{
  using Mdt::CableList::ModificationType;
  using Mdt::CableList::ModificationPk;

  /*
   * Construction
   */
  // Default constructed
  ModificationPk pk;
  QVERIFY(pk.isNull());
  QVERIFY(pk.type() == ModificationType::Undefined);
  QVERIFY(pk.code().isEmpty());
  // Contruct with a type
  ModificationPk pk2(ModificationType::New);
  QVERIFY(!pk2.isNull());
  QVERIFY(pk2.type() == ModificationType::New);
  QCOMPARE(pk2.code(), QString("NEW"));
  /*
   * Set
   */
  QVERIFY(pk.isNull());
  pk.setType(ModificationType::New);
  QVERIFY(!pk.isNull());
  QVERIFY(pk.type() == ModificationType::New);
  QCOMPARE(pk.code(), QString("NEW"));
  /*
   * Clear
   */
  QVERIFY(!pk.isNull());
  pk.clear();
  QVERIFY(pk.isNull());
  QVERIFY(pk.type() == ModificationType::Undefined);
  QVERIFY(pk.code().isEmpty());
  /*
   * Get from code
   */
  pk = ModificationPk::fromCode("NEW");
  QVERIFY(pk.type() == ModificationType::New);
  QCOMPARE(pk.code(), QString("NEW"));
  /*
   * Get from QVariant
   */
  pk = ModificationPk::fromQVariant(QVariant());
  QVERIFY(pk.isNull());
  pk = ModificationPk::fromQVariant("NEW");
  QVERIFY(!pk.isNull());
  QVERIFY(pk.type() == ModificationType::New);
  /*
   * Comparisons
   */
  // lsh and rhs are null
  QVERIFY(!(ModificationPk() == ModificationPk()));
  QVERIFY(ModificationPk() != ModificationPk());
  // lhs is null
  QVERIFY(!(ModificationPk() == ModificationPk(ModificationType::New)));
  QVERIFY(ModificationPk() != ModificationPk(ModificationType::New));
  // rhs is null
  QVERIFY(!(ModificationPk(ModificationType::New) == ModificationPk()));
  QVERIFY(ModificationPk(ModificationType::New) != ModificationPk());
  // lhs == rhs
  QVERIFY(ModificationPk(ModificationType::New) == ModificationPk(ModificationType::New));
  QVERIFY(!(ModificationPk(ModificationType::New) != ModificationPk(ModificationType::New)));
  // lhs != rhs
  QVERIFY(!(ModificationPk(ModificationType::New) == ModificationPk(ModificationType::Rem)));
  QVERIFY(ModificationPk(ModificationType::New) != ModificationPk(ModificationType::Rem));
}

void ModificationTest::typeCodeTest()
{
  using Mdt::CableList::ModificationType;
  using Mdt::CableList::ModificationPk;

  /*
   * Modification type -> code
   */
  QVERIFY(ModificationPk(ModificationType::Undefined).code().isEmpty());
  QCOMPARE(ModificationPk(ModificationType::New).code(), QString("NEW"));
  QCOMPARE(ModificationPk(ModificationType::Rem).code(), QString("REM"));
  QCOMPARE(ModificationPk(ModificationType::ModNew).code(), QString("MODNEW"));
  QCOMPARE(ModificationPk(ModificationType::ModRem).code(), QString("MODREM"));
  QCOMPARE(ModificationPk(ModificationType::Exists).code(), QString("EXISTS"));
  /*
   * Code -> modification type
   */
  QVERIFY(ModificationPk::fromCode("").type() == ModificationType::Undefined);
  QVERIFY(ModificationPk::fromCode("NEW").type() == ModificationType::New);
  QVERIFY(ModificationPk::fromCode("REM").type() == ModificationType::Rem);
  QVERIFY(ModificationPk::fromCode("MODNEW").type() == ModificationType::ModNew);
  QVERIFY(ModificationPk::fromCode("MODREM").type() == ModificationType::ModRem);
  QVERIFY(ModificationPk::fromCode("EXISTS").type() == ModificationType::Exists);
}

void ModificationTest::modelTest()
{
  using Mdt::CableList::ModificationModel;
  using Mdt::CableList::ModificationType;
  using Mdt::CableList::ModificationPk;

  ModificationModel m(pvDatabase);

  m.setLocale(QLocale::English);
  /*
   * Initial state
   */
  QCOMPARE(m.rowCount(), 5);
  /*
   * Get row for modification type
   */
  QCOMPARE(m.row(ModificationType::Undefined), -1);
  QCOMPARE(m.row(ModificationType::Exists), 0);
  QCOMPARE(m.row(ModificationType::ModNew), 1);
  QCOMPARE(m.row(ModificationType::ModRem), 2);
  QCOMPARE(m.row(ModificationType::New), 3);
  QCOMPARE(m.row(ModificationType::Rem), 4);
  /*
   * Get row for modification PK
   */
  QCOMPARE(m.row(ModificationPk(ModificationType::Undefined)), -1);
  QCOMPARE(m.row(ModificationPk(ModificationType::Exists)), 0);
  QCOMPARE(m.row(ModificationPk(ModificationType::ModNew)), 1);
  QCOMPARE(m.row(ModificationPk(ModificationType::ModRem)), 2);
  QCOMPARE(m.row(ModificationPk(ModificationType::New)), 3);
  QCOMPARE(m.row(ModificationPk(ModificationType::Rem)), 4);
  /*
   * Get PK at row
   */
  QVERIFY(m.modificationPk(-1).type() == ModificationType::Undefined);
  QVERIFY(m.modificationPk(0).type() == ModificationType::Exists);
  QVERIFY(m.modificationPk(1).type() == ModificationType::ModNew);
  QVERIFY(m.modificationPk(2).type() == ModificationType::ModRem);
  QVERIFY(m.modificationPk(3).type() == ModificationType::New);
  QVERIFY(m.modificationPk(4).type() == ModificationType::Rem);
  QVERIFY(m.modificationPk(50).type() == ModificationType::Undefined);
}

/*
 * Helper functions
 */

void ModificationTest::createDatabaseSchema()
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
  Mdt::Application app(argc, argv);
  ModificationTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}

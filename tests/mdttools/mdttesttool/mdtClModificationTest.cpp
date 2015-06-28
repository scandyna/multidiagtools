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
#include "mdtClModificationTest.h"
#include "mdtApplication.h"
#include "mdtTtDatabaseSchema.h"
#include "mdtSqlRecord.h"
#include "mdtCableListTestScenario.h"
#include "mdtClModificationKeyData.h"
#include "mdtClModificationModel.h"
#include <QTemporaryFile>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QString>
#include <QStringList>
#include <QSqlDatabase>
#include <QFile>
#include <QVariant>
#include <QList>
#include <QLocale>

#include <QDebug>

void mdtClModificationTest::initTestCase()
{
  createDatabaseSchema();
  QVERIFY(pvDatabaseManager.database().isOpen());
}

void mdtClModificationTest::cleanupTestCase()
{
  QFile::remove(pvDbFileInfo.absoluteFilePath());
}

void mdtClModificationTest::modificationDataTest()
{
  mdtClModificationPkData pk;

  /*
   * Primary key data test
   */
  // Initial state
  QVERIFY(pk.isNull());
  QVERIFY(pk.modification() == mdtClModification_t::Undefined);
  // Set
  pk.code = "NEW";
  QVERIFY(!pk.isNull());
  QVERIFY(pk.modification() == mdtClModification_t::New);
  // Clear
  pk.clear();
  QVERIFY(pk.code.isEmpty());
  QVERIFY(pk.isNull());
  QVERIFY(pk.modification() == mdtClModification_t::Undefined);
  /*
   * Check setting modification
   */
  // New
  pk.setModification(mdtClModification_t::New);
  QCOMPARE(pk.code, QString("NEW"));
  QVERIFY(pk.modification() == mdtClModification_t::New);
  // Rem
  pk.setModification(mdtClModification_t::Rem);
  QCOMPARE(pk.code, QString("REM"));
  QVERIFY(pk.modification() == mdtClModification_t::Rem);
  // ModNew
  pk.setModification(mdtClModification_t::ModNew);
  QCOMPARE(pk.code, QString("MODNEW"));
  QVERIFY(pk.modification() == mdtClModification_t::ModNew);
  // ModRem
  pk.setModification(mdtClModification_t::ModRem);
  QCOMPARE(pk.code, QString("MODREM"));
  QVERIFY(pk.modification() == mdtClModification_t::ModRem);
  // Exists
  pk.setModification(mdtClModification_t::Exists);
  QCOMPARE(pk.code, QString("EXISTS"));
  QVERIFY(pk.modification() == mdtClModification_t::Exists);
  // Undefined
  pk.setModification(mdtClModification_t::Undefined);
  QCOMPARE(pk.code, QString(""));
  QVERIFY(pk.isNull());
  QVERIFY(pk.modification() == mdtClModification_t::Undefined);
}

void mdtClModificationTest::modificationModelTest()
{
  QLocale locale(QLocale::English);
  mdtClModificationModel m(pvDatabaseManager.database(), locale);

  // Initial state
  QCOMPARE(m.rowCount(), 5);
  // Check row of modification
  QCOMPARE(m.row(mdtClModification_t::Exists), 0);
  QCOMPARE(m.row(mdtClModification_t::ModNew), 1);
  QCOMPARE(m.row(mdtClModification_t::ModRem), 2);
  QCOMPARE(m.row(mdtClModification_t::New), 3);
  QCOMPARE(m.row(mdtClModification_t::Rem), 4);
  // Check modification of row
  QVERIFY(m.modificationPk(0).modification() == mdtClModification_t::Exists);
  QVERIFY(m.modificationPk(1).modification() == mdtClModification_t::ModNew);
  QVERIFY(m.modificationPk(2).modification() == mdtClModification_t::ModRem);
  QVERIFY(m.modificationPk(3).modification() == mdtClModification_t::New);
  QVERIFY(m.modificationPk(4).modification() == mdtClModification_t::Rem);
  QVERIFY(m.modificationPk(-1).modification() == mdtClModification_t::Undefined);
  QVERIFY(m.modificationPk(50).modification() == mdtClModification_t::Undefined);
}


/*
 * Test database helper methods
 */

void mdtClModificationTest::createDatabaseSchema()
{
  QTemporaryFile dbFile;

  /*
   * Check Sqlite database creation
   */
  QVERIFY(dbFile.open());
  dbFile.close();
  pvDbFileInfo.setFile(dbFile.fileName() + ".db");
  mdtTtDatabaseSchema schema(&pvDatabaseManager);
  QVERIFY(schema.createSchemaSqlite(pvDbFileInfo));
  QVERIFY(pvDatabaseManager.database().isOpen());
  QVERIFY(schema.checkSchema());
}

/*
 * Main
 */
int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtClModificationTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}

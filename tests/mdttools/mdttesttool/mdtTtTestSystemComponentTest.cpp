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
#include "mdtTtTestSystemComponentTest.h"
#include "mdtTtTestSystemComponent.h"
#include "mdtApplication.h"
#include "mdtTtDatabaseSchema.h"
#include "mdtSqlRecord.h"
#include "mdtTtBase.h"
#include "mdtTtTestLinkData.h"
#include <QTemporaryFile>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QStringList>
#include <QSqlDatabase>
#include <QFile>
#include <QList>
#include <QFileInfo>

#include <QDebug>

/*
 * Data sets implementation
 */

bool mdtTtTestSystemComponentTestData::populateTestSystems()
{
  QSqlQuery query(pvDatabase);

  if(!beginTransaction()){
    return false;
  }
  if(!query.exec("INSERT INTO TestSystem_tbl (Id_PK, NameEN) VALUES (1, 'Test system 01')")){
    debugSqlError(query);
    rollbackTransaction();
    return false;
  }
  if(!query.exec("INSERT INTO TestSystem_tbl (Id_PK, NameEN) VALUES (2, 'Test system 02')")){
    debugSqlError(query);
    rollbackTransaction();
    return false;
  }
  if(!commitTransaction()){
    return false;
  }

  return true;
}

void mdtTtTestSystemComponentTestData::removeTestSystems()
{
  QSqlQuery query(pvDatabase);
  QVERIFY(query.exec("DELETE FROM TestSystem_tbl"));
}

bool mdtTtTestSystemComponentTestData::populateTestSystemComponents()
{
  QSqlQuery query(pvDatabase);

  if(!beginTransaction()){
    return false;
  }
  if(!query.exec("INSERT INTO TestSystemComponent_tbl (Id_PK, Alias, NameEN) VALUES (11, 'TestSysComp11', 'Test system component 11')")){
    debugSqlError(query);
    rollbackTransaction();
    return false;
  }
  if(!query.exec("INSERT INTO TestSystemComponent_tbl (Id_PK, Alias, NameEN) VALUES (12, 'TestSysComp12', 'Test system component 12')")){
    debugSqlError(query);
    rollbackTransaction();
    return false;
  }
  if(!query.exec("INSERT INTO TestSystemComponent_tbl (Id_PK, Alias, NameEN) VALUES (13, 'TestSysComp13', 'Test system component 13')")){
    debugSqlError(query);
    rollbackTransaction();
    return false;
  }

  if(!commitTransaction()){
    return false;
  }

  return true;
}

void mdtTtTestSystemComponentTestData::removeTestSystemComponents()
{
  QSqlQuery query(pvDatabase);
  QVERIFY(query.exec("DELETE FROM TestSystemComponent_tbl"));
}

void mdtTtTestSystemComponentTestData::clear()
{
  removeTestSystemComponents();
  removeTestSystems();
}

bool mdtTtTestSystemComponentTestData::beginTransaction()
{
  if(!pvDatabase.transaction()){
    debugSqlError(pvDatabase.lastError());
    return false;
  }
  return true;
}

bool mdtTtTestSystemComponentTestData::rollbackTransaction()
{
  if(!pvDatabase.rollback()){
    debugSqlError(pvDatabase.lastError());
    return false;
  }
  return true;
}

bool mdtTtTestSystemComponentTestData::commitTransaction()
{
  if(!pvDatabase.commit()){
    debugSqlError(pvDatabase.lastError());
    return false;
  }
  return true;
}

void mdtTtTestSystemComponentTestData::debugSqlError(const QSqlError& error)
{
  qDebug() << error;
}

void mdtTtTestSystemComponentTestData::debugSqlError(const QSqlQuery & query)
{
  qDebug() << "Error on query '" << query.lastQuery() << "': " << query.lastError();
}


/*
 * Test implementation
 */

void mdtTtTestSystemComponentTest::initTestCase()
{
  createDatabaseSchema();
  QVERIFY(pvDatabaseManager.database().isOpen());
}

void mdtTtTestSystemComponentTest::cleanupTestCase()
{
  QFile::remove(pvDbFileInfo.absoluteFilePath());
}

void mdtTtTestSystemComponentTest::testSystemAssignationTest()
{
  mdtTtTestSystemComponent tsc(pvDatabaseManager.database());
  mdtTtTestSystemComponentTestData testData(pvDatabaseManager.database());
  QList<QSqlRecord> dataList;
  mdtTtTestSystemAssignationData data;
  bool ok;

  // Create dataset
  QVERIFY(testData.populateTestSystems());
  QVERIFY(testData.populateTestSystemComponents());

  // Check assignations
  dataList = tsc.getDataList<QSqlRecord>("SELECT * FROM TestSystem_TestSystemComponent_tbl", ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
  // Assign test system 01 - component 11
  data.testSystemId = 1;
  data.testSystemComponentId = 11;
  QVERIFY(tsc.addTestSystemAssignation(data));
  // Check assignations
  dataList = tsc.getDataList<QSqlRecord>("SELECT * FROM TestSystem_TestSystemComponent_tbl", ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  QCOMPARE(dataList.at(0).value("TestSystem_Id_FK"), QVariant(1));
  QCOMPARE(dataList.at(0).value("TestSystemComponent_Id_FK"), QVariant(11));
  // Try to remove an non existing assignation
  data.testSystemId = 20;
  data.testSystemComponentId = 11;
  tsc.removeTestSystemAssignation(data);
  ///QVERIFY(!tsc.removeTestSystemAssignation(data));
  // Check that nothing was deleted
  dataList = tsc.getDataList<QSqlRecord>("SELECT * FROM TestSystem_TestSystemComponent_tbl", ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  // Remove existing assignation
  data.testSystemId = 1;
  data.testSystemComponentId = 11;
  QVERIFY(tsc.removeTestSystemAssignation(data));
  // Check that assignation was removed
  dataList = tsc.getDataList<QSqlRecord>("SELECT * FROM TestSystem_TestSystemComponent_tbl", ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
}

void mdtTtTestSystemComponentTest::createUnitTest()
{
  mdtTtTestSystemComponent tsc(pvDatabaseManager.database());
  mdtTtTestSystemComponentTestData testData(pvDatabaseManager.database());
  QList<QSqlRecord> dataList;
  QVariant unitId;
  bool ok;

  // Create dataset
  QVERIFY(testData.populateTestSystems());
  QVERIFY(testData.populateTestSystemComponents());

  /*
   * Create a bus coupling relay that belongs to Test system component 11
   */
  // Create unit
  unitId = tsc.createUnit(11, "BUSCPLREL");
  QVERIFY(!unitId.isNull());
  // Check back
  dataList = tsc.getDataList<QSqlRecord>("SELECT * FROM TestSystemUnit_view", ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 1);
  QCOMPARE(dataList.at(0).value("TestSystemComponent_Id_FK"), QVariant(11));
  QCOMPARE(dataList.at(0).value("Type_Code_FK"), QVariant("BUSCPLREL"));
  // Remove unit
  QVERIFY(tsc.removeUnit(unitId, true));
  // Check back
  dataList = tsc.getDataList<QSqlRecord>("SELECT * FROM TestSystemUnit_view", ok);
  QVERIFY(ok);
  QCOMPARE(dataList.size(), 0);
}

/*
 * Test helper functions
 */

void mdtTtTestSystemComponentTest::createDatabaseSchema()
{
  QTemporaryFile dbFile;
  ///QFileInfo dbFileInfo;

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
  mdtTtTestSystemComponentTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}

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
#include "mdtTestCableTest.h"
#include "mdtTtDut01TestData.h"
#include "mdtApplication.h"
#include "mdtTtDatabaseSchema.h"
#include "mdtSqlDatabaseSchemaThread.h"
#include "mdtSqlRecord.h"
#include "mdtTtBase.h"
#include "mdtTtTestLinkData.h"
///#include "mdtClUnitConnectionData.h"
///#include "mdtTtTestNodeRoute.h"
///#include "mdtTtTestNodeSetupData.h"
///#include "mdtTtTestNodeUnitSetupData.h"
///#include "mdtTtTestModelItemData.h"
///#include "mdtTtTestModelItemRouteData.h"
///#include "mdtTtValueLimits.h"
///#include "mdtTtTestNodeTestData.h"
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

void mdtTestCableTest::initTestCase()
{
  QTemporaryFile dbFile;

  /*
   * Create a file for database
   */
  QVERIFY(dbFile.open());
  dbFile.close();
  /*
   * Open database
   */
  pvDatabase = QSqlDatabase::addDatabase("QSQLITE");
  pvDatabase.setDatabaseName(QFileInfo(dbFile).absoluteFilePath());
  QVERIFY(pvDatabase.open());
  /*
   * Create schema
   */
  createDatabaseSchema();
  QVERIFY(pvDatabase.isOpen());
}

void mdtTestCableTest::cleanupTestCase()
{
  pvDatabase.close();
  QFile::remove(QFileInfo(pvDatabase.databaseName()).absoluteFilePath());
}

void mdtTestCableTest::sandbox()
{
  mdtTtDut01TestData dut01Data(pvDatabase);

  QVERIFY(dut01Data.populate());
}


void mdtTestCableTest::createDatabaseSchema()
{
  mdtTtDatabaseSchema schema;
  mdtSqlDatabaseSchemaThread thread;
  QVERIFY(thread.createSchemaBlocking(schema.databaseSchema(), pvDatabase));
  QVERIFY(pvDatabase.isOpen());
}

/*
 * Main
 */
int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtTestCableTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}

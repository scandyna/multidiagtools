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
#include "mdtClArticleLinkTest.h"
#include "mdtClArticleLinkKeyData.h"
#include "mdtClArticleLinkData.h"
#include "mdtApplication.h"
#include "mdtTtDatabaseSchema.h"
#include "mdtSqlRecord.h"
#include "mdtCableListTestScenario.h"
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

void mdtClArticleLinkTest::initTestCase()
{
  createDatabaseSchema();
  QVERIFY(pvDatabaseManager.database().isOpen());
}

void mdtClArticleLinkTest::cleanupTestCase()
{
  QFile::remove(pvDbFileInfo.absoluteFilePath());
}

void mdtClArticleLinkTest::articleLinkDataTest()
{
  mdtClArticleLinkKeyData key;
  mdtClArticleLinkData data;

  /*
   * Key data test
   */
  // Initial state
  QVERIFY(key.isNull());
  // Set some data
  key.connectionStartFk.id = 1;
  key.connectionStartFk.articleId = 10;
  key.connectionStartFk.connectionTypeFk.setType(mdtClConnectionType_t::Socket);
  QVERIFY(!key.connectionStartFk.isNull());
  QVERIFY(key.isNull());
  key.connectionEndFk.id = 2;
  key.connectionEndFk.articleId = 10;
  key.connectionEndFk.connectionTypeFk.setType(mdtClConnectionType_t::Socket);
  QVERIFY(!key.connectionEndFk.isNull());
  QVERIFY(key.isNull());
  key.linkTypeFk.setType(mdtClLinkType_t::CableLink);
  QVERIFY(key.isNull());
  key.linkDirectionFk.setDirection(mdtClLinkDirection_t::Bidirectional);
  QVERIFY(!key.isNull());
  // Clear
  key.clear();
  QVERIFY(key.connectionStartFk.isNull());
  QVERIFY(key.connectionEndFk.isNull());
  QVERIFY(key.linkTypeFk.isNull());
  QVERIFY(key.linkDirectionFk.isNull());
  QVERIFY(key.isNull());
  /*
   * Data test
   */
  // Initial state
  QVERIFY(data.isNull());
  // Set data
  key.clear();
  key.connectionStartFk.id = 5;
  key.connectionStartFk.articleId = 20;
  key.connectionStartFk.connectionTypeFk.setType(mdtClConnectionType_t::Pin);
  QVERIFY(!key.connectionStartFk.isNull());
  key.connectionEndFk.id = 6;
  key.connectionEndFk.articleId = 20;
  key.connectionEndFk.connectionTypeFk.setType(mdtClConnectionType_t::Socket);
  QVERIFY(!key.connectionEndFk.isNull());
  key.linkTypeFk.setType(mdtClLinkType_t::CableLink);
  key.linkDirectionFk.setDirection(mdtClLinkDirection_t::Bidirectional);
  QVERIFY(!key.isNull());
  data.setKeyData(key);
  data.indetification = "Link 5-6";
  data.sinceVersion = 1.0;
  data.modification = "new";
  data.resistance = 0.5;
  QVERIFY(!data.isNull());
  // Clear
  data.clear();
  QVERIFY(data.keyData().isNull());
  QVERIFY(data.indetification.isNull());
  QVERIFY(data.sinceVersion.isNull());
  QVERIFY(data.modification.isNull());
  QVERIFY(data.resistance.isNull());
  QVERIFY(data.isNull());
}


/*
 * Test database helper methods
 */

void mdtClArticleLinkTest::createDatabaseSchema()
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
  mdtClArticleLinkTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}

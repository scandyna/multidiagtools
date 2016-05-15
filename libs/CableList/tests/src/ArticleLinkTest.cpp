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
#include "ArticleLinkTest.h"
#include "Mdt/Application.h"
#include "Mdt/CableList/DatabaseSchema.h"
#include "Mdt/CableList/ArticleLinkPk.h"

void ArticleLinkTest::initTestCase()
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

void ArticleLinkTest::cleanupTestCase()
{
  pvDatabase.close();
}

/*
 * Tests
 */
void ArticleLinkTest::linkPkTest()
{
  using Mdt::CableList::ArticleConnectionPk;
  using Mdt::CableList::ArticleLinkPk;

  ArticleLinkPk pk;

  /*
   * Initial state
   */
  QVERIFY(pk.isNull());
  /*
   * Set
   */
  pk.setConnectionStart(ArticleConnectionPk(1));
  QVERIFY(pk.isNull());
  QCOMPARE(pk.connectionStart().id(), 1);
  pk.setConnectionEnd(ArticleConnectionPk(2));
  QCOMPARE(pk.connectionEnd().id(), 2);
  QVERIFY(!pk.isNull());
  /*
   * Clear
   */
  QVERIFY(!pk.isNull());
  pk.clear();
  QVERIFY(pk.connectionStart().isNull());
  QVERIFY(pk.connectionEnd().isNull());
  /*
   * Clear start/end connection
   */
  pk.setConnectionStart(ArticleConnectionPk(1));
  pk.setConnectionEnd(ArticleConnectionPk(2));
  QVERIFY(!pk.isNull());
  // Clear start connection
  pk.clearConnectionStart();
  QVERIFY(pk.connectionStart().isNull());
  QVERIFY(!pk.connectionEnd().isNull());
  QVERIFY(pk.isNull());
  pk.setConnectionStart(ArticleConnectionPk(1));
  QVERIFY(!pk.isNull());
  // Clear end connection
  pk.clearConnectionEnd();
  QVERIFY(!pk.connectionStart().isNull());
  QVERIFY(pk.connectionEnd().isNull());
  QVERIFY(pk.isNull());
}

/*
 * Helper functions
 */

void ArticleLinkTest::createDatabaseSchema()
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
  ArticleLinkTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}

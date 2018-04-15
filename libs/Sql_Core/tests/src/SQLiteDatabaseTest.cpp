/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "SQLiteDatabaseTest.h"
#include "Mdt/Sql/SQLiteDatabase.h"
#include <QTemporaryDir>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QSqlQuery>

#include <QDebug>

using namespace Mdt::Sql;

void SQLiteDatabaseTest::initTestCase()
{
}

void SQLiteDatabaseTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void SQLiteDatabaseTest::constructTest()
{
  SQLiteDatabase sqliteDb;
  auto dbConnection = sqliteDb.database();
  QVERIFY(dbConnection.isValid());
  QCOMPARE(dbConnection.driverName(), QString("QSQLITE"));
  QVERIFY(!dbConnection.isOpen());

  SQLiteDatabase sqliteDbA("connection-A");
  dbConnection = sqliteDbA.database();
  QVERIFY(dbConnection.isValid());
  QCOMPARE(dbConnection.driverName(), QString("QSQLITE"));
  QCOMPARE(dbConnection.connectionName(), QString("connection-A"));
  QVERIFY(!dbConnection.isOpen());
}

void SQLiteDatabaseTest::createNewTest()
{
  QTemporaryDir dir;
  QVERIFY(dir.isValid());
  SQLiteDatabase sqliteDb;
  QString filePath;
  /*
   * File does not exist
   */
  filePath = dir.path() + "/dbA.sqlite";
  QVERIFY(!QFileInfo::exists(filePath));
  QVERIFY(sqliteDb.createNew(filePath));
  QVERIFY(QFileInfo::exists(filePath));
  auto dbConnection = sqliteDb.database();
  QVERIFY(dbConnection.isOpen());
  QVERIFY(!dbConnection.isOpenError());
  QVERIFY(createSimpleTable(dbConnection));
  /*
   * Path is a directory
   * (also checks that SQLiteDatabase closes before try to open)
   */
  filePath = dir.path();
  QVERIFY(!sqliteDb.createNew(filePath));
  dbConnection = sqliteDb.database();
  QVERIFY(!dbConnection.isOpen());
  /*
   * A file already exists
   */
  filePath = dir.path() + "/document.txt";
  QVERIFY(writeTextFile(filePath, "ABCD"));
  QVERIFY(!sqliteDb.createNew(filePath));
  dbConnection = sqliteDb.database();
  QVERIFY(!dbConnection.isOpen());
  // Check that the file was not touched
  QCOMPARE(readTextFile(filePath), QString("ABCD"));
}

void SQLiteDatabaseTest::openExistingTest()
{
  QTemporaryDir dir;
  QVERIFY(dir.isValid());
  SQLiteDatabase sqliteDb;
  QString filePath;
  /*
   * Open a valid existing SQLite database, read only
   */
  filePath = dir.path() + "/validRoDb.sqlite";
  QVERIFY(sqliteDb.createNew(filePath));
  QVERIFY(sqliteDb.openExisting(filePath, SQLiteDatabase::ReadOnly));
  auto dbConnection = sqliteDb.database();
  QVERIFY(dbConnection.isOpen());
  QVERIFY(!dbConnection.isOpenError());
  QVERIFY(!createSimpleTable(dbConnection));
  /*
   * Open a valid existing SQLite database
   */
  filePath = dir.path() + "/validDb.sqlite";
  QVERIFY(sqliteDb.createNew(filePath));
  QVERIFY(sqliteDb.openExisting(filePath));
  dbConnection = sqliteDb.database();
  QVERIFY(dbConnection.isOpen());
  QVERIFY(!dbConnection.isOpenError());
  QVERIFY(createSimpleTable(dbConnection));
  /*
   * Try to open a non existing file
   * (also checks that SQLiteDatabase closes before try to open)
   */
  filePath = dir.path() + "nonExisting.sqlite";
  QVERIFY(!QFileInfo::exists(filePath));
  QVERIFY(!sqliteDb.openExisting(filePath));
  QVERIFY(!QFileInfo::exists(filePath));
  dbConnection = sqliteDb.database();
  QVERIFY(!dbConnection.isOpen());
  /*
   * Try to open a file that is not a SQLite database
   */
  filePath = dir.path() + "/document.txt";
  QVERIFY(writeTextFile(filePath, "ABCD"));
  QVERIFY(!sqliteDb.openExisting(filePath));
  dbConnection = sqliteDb.database();
  QVERIFY(!dbConnection.isOpen());
  // Check that the file was not touched
  QCOMPARE(readTextFile(filePath), QString("ABCD"));
}

/*
 * Helpers
 */

bool SQLiteDatabaseTest::writeTextFile(const QString& path, const QString& text)
{
  QFile file(path);

  if(!file.open(QFile::ReadWrite | QFile::Text)){
    qWarning() << "Open file " << path << " failed: " << file.errorString();
    return false;
  }
  if(file.write(text.toLocal8Bit()) < 1){
    qWarning() << "Writing to file " << path << " failed: " << file.errorString();
    return false;
  }
  file.close();

  return true;
}

QString SQLiteDatabaseTest::readTextFile(const QString& path)
{
  QString str;
  QFile file(path);

  if(!file.open(QFile::ReadOnly | QFile::Text)){
    qWarning() << "Open file " << path << " failed: " << file.errorString();
    return str;
  }
  str = QString::fromLocal8Bit( file.readAll() );
  if(str.isEmpty()){
    qWarning() << "Reading file " << path << " failed: " << file.errorString();
    return str;
  }

  return str;
}

bool SQLiteDatabaseTest::createSimpleTable(const QSqlDatabase & dbConnection)
{
  Q_ASSERT(dbConnection.isOpen());

  QSqlQuery query(dbConnection);

  return query.exec("CREATE TABLE SimpleTable(name Text)");
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  SQLiteDatabaseTest test;

  return QTest::qExec(&test, argc, argv);
}

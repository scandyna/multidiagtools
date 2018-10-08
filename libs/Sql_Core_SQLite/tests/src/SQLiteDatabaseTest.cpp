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
#include <QTemporaryFile>
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
 * Helpers
 */

bool databaseUsesExtendedErrorCodes(const QSqlDatabase & db)
{
  return db.connectOptions().contains("QSQLITE_USE_EXTENDED_RESULT_CODES");
}

/*
 * Tests
 */

void SQLiteDatabaseTest::isSQLIteDriverTest()
{
  QVERIFY(SQLiteDatabase::isSQLiteDriver("QSQLITE"));
  QVERIFY(SQLiteDatabase::isSQLiteDriver("MDTQSQLITE"));
  QVERIFY(!SQLiteDatabase::isSQLiteDriver("QMYSQL"));
}

void SQLiteDatabaseTest::constructTest()
{
  SQLiteDatabase sqliteDb;
  auto dbConnection = sqliteDb.database();
  QVERIFY(dbConnection.isValid());
  QVERIFY(SQLiteDatabase::isSQLiteDriver(dbConnection.driverName()));
  QVERIFY(!dbConnection.isOpen());

  SQLiteDatabase sqliteDbA("connection-A");
  dbConnection = sqliteDbA.database();
  QVERIFY(dbConnection.isValid());
  QVERIFY(SQLiteDatabase::isSQLiteDriver(dbConnection.driverName()));
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
  QVERIFY(databaseUsesExtendedErrorCodes(dbConnection));
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
  /*
   * Create a new database again
   */
  filePath = dir.path() + "/dbB.sqlite";
  QVERIFY(!QFileInfo::exists(filePath));
  QVERIFY(sqliteDb.createNew(filePath));
  QVERIFY(QFileInfo::exists(filePath));
  dbConnection = sqliteDb.database();
  QVERIFY(dbConnection.isOpen());
  QVERIFY(!dbConnection.isOpenError());
  QVERIFY(databaseUsesExtendedErrorCodes(dbConnection));
  QVERIFY(createSimpleTable(dbConnection));
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
  QVERIFY(databaseUsesExtendedErrorCodes(dbConnection));
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
  QVERIFY(databaseUsesExtendedErrorCodes(dbConnection));
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

void SQLiteDatabaseTest::checkForeignKeySupportEnabled()
{
  QTemporaryDir dir;
  QVERIFY(dir.isValid());
  const QString dbPath = QDir::cleanPath(dir.path() + "/test.sqlite");

  {
    SQLiteDatabase sqliteDb;
    QVERIFY(sqliteDb.createNew(dbPath));
    const auto dbConnection = sqliteDb.database();
    QVERIFY(dbConnection.isOpen());
    QSqlQuery query(dbConnection);

    const QString createPersonTableSql
      = "CREATE TABLE Person(\n"\
        " Id_PK INTEGER PRIMARY KEY\n"\
        ")";
    QVERIFY(query.exec(createPersonTableSql));

    const QString createAddressTableSql
      = "CREATE TABLE Address(\n"\
        " Id_PK INTEGER PRIMARY KEY,\n"\
        " Person_Id_FK INTEGER,\n"\
        "FOREIGN KEY(Person_Id_FK) REFERENCES Person(Id_PK)"
        ")";
    QVERIFY(query.exec(createAddressTableSql));

    QVERIFY(query.exec("INSERT INTO Person (Id_PK) VALUES (1)"));
    QVERIFY(query.exec("INSERT INTO Address (Id_PK, Person_Id_FK) VALUES (11,1)"));
    QVERIFY(!query.exec("INSERT INTO Address (Id_PK, Person_Id_FK) VALUES (12,2)"));
  }

  {
    SQLiteDatabase sqliteDb;
    QVERIFY(sqliteDb.openExisting(dbPath));
    const auto dbConnection = sqliteDb.database();
    QVERIFY(dbConnection.isOpen());
    QSqlQuery query(dbConnection);

    QVERIFY(query.exec("INSERT INTO Person (Id_PK) VALUES (5)"));
    QVERIFY(query.exec("INSERT INTO Address (Id_PK, Person_Id_FK) VALUES (15,5)"));
    QVERIFY(!query.exec("INSERT INTO Address (Id_PK, Person_Id_FK) VALUES (16,6)"));
  }
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

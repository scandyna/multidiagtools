/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#include "Mdt/Sql/SQLiteConnectionParameters.h"
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QSqlQuery>

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

bool databaseIsOpenReadOnly(const QSqlDatabase & db)
{
  return db.connectOptions().contains("QSQLITE_OPEN_READONLY");
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

void SQLiteDatabaseTest::addConnection()
{
  const auto connection = SQLiteDatabase::addConnection();
  QVERIFY(connection);

  {
    const QSqlDatabase db = connection->database();

    QVERIFY( SQLiteDatabase::isSQLiteDriver(db.driverName()) );
    QVERIFY(db.isValid());
    QVERIFY(!db.isOpen());
  }

  Connection::remove(*connection);
}

void SQLiteDatabaseTest::openTest()
{
  SQLiteConnectionParameters parameters;
  const auto connection = SQLiteDatabase::addConnection();
  QVERIFY(connection);
  QTemporaryDir dir;
  QVERIFY(dir.isValid());
  QString filePath;

  parameters.setOpenMode(SQLiteOpenMode::ReadWrite);
  /*
   * Open Read/Write
   * File not yet exists
   */
  filePath = dir.path() + "/dbA.sqlite";
  parameters.setDatabaseFile(filePath);
  QVERIFY(!QFileInfo::exists(filePath));
  QVERIFY(!connection->database().isOpen());
  SQLiteDatabase db1(*connection, parameters);
  QVERIFY(db1.open());
  QVERIFY(QFileInfo::exists(filePath));
  QVERIFY(db1.isSQLiteDatabaseOpen());
  {
    QSqlDatabase db = connection->database();
    QVERIFY(db.isOpen());
    QVERIFY(!db.isOpenError());
    QVERIFY(databaseUsesExtendedErrorCodes(db));
    QVERIFY(!databaseIsOpenReadOnly(db));
    QVERIFY(createSimpleTable(db));
    db.close();
  }
  /*
   * Open Read/Write
   * File exists and is a SQLite database
   */
  QVERIFY(QFileInfo::exists(filePath));
  QVERIFY(!connection->database().isOpen());
  QVERIFY(db1.open());
  QVERIFY(QFileInfo::exists(filePath));
  QVERIFY(db1.isSQLiteDatabaseOpen());
  {
    QSqlDatabase db = connection->database();
    QVERIFY(db.isOpen());
    QVERIFY(!db.isOpenError());
    QVERIFY(databaseUsesExtendedErrorCodes(db));
    QVERIFY(!databaseIsOpenReadOnly(db));
    db.close();
  }
  /*
   * Open Read/Write
   * File exists and is a not SQLite database
   */
  filePath = dir.path() + "/document.txt";
  parameters.setDatabaseFile(filePath);
  QVERIFY(writeTextFile(filePath, "ABCD"));
  QVERIFY(QFileInfo::exists(filePath));
  QVERIFY(!connection->database().isOpen());
  SQLiteDatabase db2(*connection, parameters);
  QVERIFY(!db2.open());
  QVERIFY(!connection->database().isOpen());
  // Check that the file was not touched
  QVERIFY(QFileInfo::exists(filePath));
  QCOMPARE(readTextFile(filePath), QString("ABCD"));

  parameters.setOpenMode(SQLiteOpenMode::ReadOnly);
  /*
   * Open Read only
   * File not exists
   */
  filePath = dir.path() + "/dbB.sqlite";
  parameters.setDatabaseFile(filePath);
  QVERIFY(!QFileInfo::exists(filePath));
  QVERIFY(!connection->database().isOpen());
  SQLiteDatabase db3(*connection, parameters);
  QVERIFY(!db3.open());
  QVERIFY(!connection->database().isOpen());
  QVERIFY(!QFileInfo::exists(filePath));
  /*
   * Open Read only
   * File exists and is a SQLite database
   */
  filePath = dir.path() + "/dbA.sqlite";
  parameters.setDatabaseFile(filePath);
  QVERIFY(QFileInfo::exists(filePath));
  QVERIFY(!connection->database().isOpen());
  SQLiteDatabase db4(*connection, parameters);
  QVERIFY(db4.open());
  QVERIFY(connection->database().isOpen());
  QVERIFY(QFileInfo::exists(filePath));
  QVERIFY(db4.isSQLiteDatabaseOpen());
  {
    QSqlDatabase db = connection->database();
    QVERIFY(db.isOpen());
    QVERIFY(!db.isOpenError());
    QVERIFY(databaseUsesExtendedErrorCodes(db));
    QVERIFY(databaseIsOpenReadOnly(db));
    db.close();
  }
  /*
   * Open Read only
   * File exists and is a not SQLite database
   */
  filePath = dir.path() + "/document.txt";
  parameters.setDatabaseFile(filePath);
  QVERIFY(QFileInfo::exists(filePath));
  QVERIFY(!connection->database().isOpen());
  SQLiteDatabase db5(*connection, parameters);
  QVERIFY(!db5.open());
  QVERIFY(!connection->database().isOpen());
  // Check that the file was not touched
  QVERIFY(QFileInfo::exists(filePath));
  QCOMPARE(readTextFile(filePath), QString("ABCD"));

  Connection::remove(*connection);
}

void SQLiteDatabaseTest::isSQLiteDatabaseOpenTest()
{
  SQLiteConnectionParameters parameters;
  const auto connection = SQLiteDatabase::addConnection();
  QVERIFY(connection);
  QTemporaryDir dir;
  QVERIFY(dir.isValid());
  const QString sqliteFilePath = dir.path() + "/db.sqlite";
  const QString documentFilePath = dir.path() + "/document.txt";

  QVERIFY(writeTextFile(documentFilePath, "ABCD"));

  {
    QSqlDatabase db = connection->database();
    QVERIFY(db.isValid());

    parameters.setDatabaseFile(sqliteFilePath);
    SQLiteDatabase sqliteDb1(*connection, parameters);
    QVERIFY(!sqliteDb1.isSQLiteDatabaseOpen());
    db.setDatabaseName(sqliteFilePath);
    QVERIFY(db.open());
    QVERIFY(sqliteDb1.isSQLiteDatabaseOpen());
    db.close();

    parameters.setDatabaseFile(documentFilePath);
    SQLiteDatabase sqliteDb2(*connection, parameters);
    QVERIFY(!sqliteDb2.isSQLiteDatabaseOpen());
    db.setDatabaseName(documentFilePath);
    QVERIFY(db.open());
    QVERIFY(!sqliteDb2.isSQLiteDatabaseOpen());
    db.close();
  }

  Connection::remove(*connection);
}

void SQLiteDatabaseTest::createNewTest()
{
  SQLiteConnectionParameters parameters;
  const auto connection = SQLiteDatabase::addConnection();
  QVERIFY(connection);
  QTemporaryDir dir;
  QVERIFY(dir.isValid());
  QString filePath;

  /*
   * File does not exist
   */
  filePath = dir.path() + "/dbA.sqlite";
  parameters.setDatabaseFile(filePath);
  QVERIFY(!QFileInfo::exists(filePath));
  QVERIFY(!connection->database().isOpen());
  SQLiteDatabase db1(*connection, parameters);
  QVERIFY(db1.createNew());
  QVERIFY(QFileInfo::exists(filePath));
  QVERIFY(db1.isSQLiteDatabaseOpen());
  {
    QSqlDatabase db = connection->database();
    QVERIFY(db.isOpen());
    QVERIFY(!db.isOpenError());
    QVERIFY(databaseUsesExtendedErrorCodes(db));
    QVERIFY(!databaseIsOpenReadOnly(db));
    QVERIFY(createSimpleTable(db));
    db.close();
  }
  /*
   * Path is a directory
   * (also checks that SQLiteDatabase closes before try to open)
   */
  filePath = dir.path();
  parameters.setDatabaseFile(filePath);
  QVERIFY(!connection->database().isOpen());
  SQLiteDatabase db2(*connection, parameters);
  QVERIFY(!db2.createNew());
  QVERIFY(!db2.isSQLiteDatabaseOpen());
  QVERIFY(!connection->database().isOpen());
  /*
   * A file already exists and is a SQLite database
   */
  filePath = dir.path() + "/dbA.sqlite";
  parameters.setDatabaseFile(filePath);
  QVERIFY(QFileInfo::exists(filePath));
  QVERIFY(!connection->database().isOpen());
  SQLiteDatabase db3(*connection, parameters);
  QVERIFY(!db3.createNew());
  QVERIFY(QFileInfo::exists(filePath));
  QVERIFY(!db3.isSQLiteDatabaseOpen());
  QVERIFY(!connection->database().isOpen());
  /*
   * A file already exists and is not a SQLite database
   */
  filePath = dir.path() + "/document.txt";
  QVERIFY(writeTextFile(filePath, "ABCD"));
  parameters.setDatabaseFile(filePath);
  QVERIFY(QFileInfo::exists(filePath));
  QVERIFY(!connection->database().isOpen());
  SQLiteDatabase db4(*connection, parameters);
  QVERIFY(!db4.createNew());
  QVERIFY(QFileInfo::exists(filePath));
  QVERIFY(!db4.isSQLiteDatabaseOpen());
  QVERIFY(!connection->database().isOpen());
  // Check that the file was not touched
  QCOMPARE(readTextFile(filePath), QString("ABCD"));
  /*
   * Create a new database again
   */
  filePath = dir.path() + "/dbB.sqlite";
  parameters.setDatabaseFile(filePath);
  QVERIFY(!QFileInfo::exists(filePath));
  QVERIFY(!connection->database().isOpen());
  SQLiteDatabase db5(*connection, parameters);
  QVERIFY(db5.createNew());
  QVERIFY(QFileInfo::exists(filePath));
  QVERIFY(db5.isSQLiteDatabaseOpen());
  {
    QSqlDatabase db = connection->database();
    QVERIFY(db.isOpen());
    QVERIFY(!db.isOpenError());
    QVERIFY(databaseUsesExtendedErrorCodes(db));
    QVERIFY(!databaseIsOpenReadOnly(db));
    QVERIFY(createSimpleTable(db));
    db.close();
  }

  /*
   * File does not exist
   * Read only open mode
   */
  filePath = dir.path() + "/dbC.sqlite";
  parameters.setDatabaseFile(filePath);
  parameters.setOpenMode(SQLiteOpenMode::ReadOnly);
  QVERIFY(!QFileInfo::exists(filePath));
  QVERIFY(!connection->database().isOpen());
  SQLiteDatabase db6(*connection, parameters);
  QVERIFY(db6.createNew());
  QVERIFY(QFileInfo::exists(filePath));
  QVERIFY(db6.isSQLiteDatabaseOpen());
  {
    QSqlDatabase db = connection->database();
    QVERIFY(db.isOpen());
    QVERIFY(!db.isOpenError());
    QVERIFY(databaseUsesExtendedErrorCodes(db));
    QVERIFY(databaseIsOpenReadOnly(db));
    db.close();
  }

  Connection::remove(*connection);
}

void SQLiteDatabaseTest::openExistingTest()
{
  SQLiteConnectionParameters parameters;
  const auto connection = SQLiteDatabase::addConnection();
  QVERIFY(connection);
  QTemporaryDir dir;
  QVERIFY(dir.isValid());
  QString filePath;

  /*
   * Open a valid existing SQLite database, read only
   */
  filePath = dir.path() + "/validRoDb.sqlite";
  parameters.setDatabaseFile(filePath);
  parameters.setOpenMode(SQLiteOpenMode::ReadOnly);
  QVERIFY(!connection->database().isOpen());
  SQLiteDatabase db1(*connection, parameters);
  QVERIFY(db1.createNew());
  Connection::close(*connection);
  QVERIFY(QFileInfo::exists(filePath));
  QVERIFY(db1.openExisting());
  QVERIFY(db1.isSQLiteDatabaseOpen());
  {
    QSqlDatabase db = connection->database();
    QVERIFY(db.isOpen());
    QVERIFY(!db.isOpenError());
    QVERIFY(databaseUsesExtendedErrorCodes(db));
    QVERIFY(databaseIsOpenReadOnly(db));
    db.close();
  }

  parameters.setOpenMode(SQLiteOpenMode::ReadWrite);
  /*
   * Open a valid existing SQLite database
   */
  filePath = dir.path() + "/validDb.sqlite";
  parameters.setDatabaseFile(filePath);
  QVERIFY(!connection->database().isOpen());
  SQLiteDatabase db2(*connection, parameters);
  QVERIFY(db2.createNew());
  Connection::close(*connection);
  QVERIFY(QFileInfo::exists(filePath));
  QVERIFY(db2.openExisting());
  QVERIFY(db2.isSQLiteDatabaseOpen());
  {
    QSqlDatabase db = connection->database();
    QVERIFY(db.isOpen());
    QVERIFY(!db.isOpenError());
    QVERIFY(databaseUsesExtendedErrorCodes(db));
    QVERIFY(!databaseIsOpenReadOnly(db));
    QVERIFY(createSimpleTable(db));
    db.close();
  }
  /*
   * Try to open a non existing file
   * (also checks that SQLiteDatabase closes before try to open)
   */
  filePath = dir.path() + "nonExisting.sqlite";
  parameters.setDatabaseFile(filePath);
  QVERIFY(!connection->database().isOpen());
  SQLiteDatabase db3(*connection, parameters);
  QVERIFY(!QFileInfo::exists(filePath));
  QVERIFY(!db3.openExisting());
  QVERIFY(!QFileInfo::exists(filePath));
  QVERIFY(!connection->database().isOpen());
  /*
   * Try to open a file that is not a SQLite database
   */
  filePath = dir.path() + "/document.txt";
  QVERIFY(writeTextFile(filePath, "ABCD"));
  parameters.setDatabaseFile(filePath);
  QVERIFY(!connection->database().isOpen());
  SQLiteDatabase db4(*connection, parameters);
  QVERIFY(QFileInfo::exists(filePath));
  QVERIFY(!db4.openExisting());
  QVERIFY(QFileInfo::exists(filePath));
  QVERIFY(!connection->database().isOpen());
  // Check that the file was not touched
  QCOMPARE(readTextFile(filePath), QString("ABCD"));

  Connection::remove(*connection);
}

void SQLiteDatabaseTest::checkForeignKeySupportEnabled()
{
  SQLiteConnectionParameters parameters;
  const auto connection = SQLiteDatabase::addConnection();
  QVERIFY(connection);
  QTemporaryDir dir;
  QVERIFY(dir.isValid());
  const QString dbPath = QDir::cleanPath(dir.path() + "/test.sqlite");

  parameters.setDatabaseFile(dbPath);
  SQLiteDatabase sqliteDb(*connection, parameters);
  QVERIFY(sqliteDb.createNew());

  {
    QSqlDatabase db = connection->database();
    QVERIFY(db.isOpen());
    QSqlQuery query(db);

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

  Connection::close(*connection);
  QVERIFY(sqliteDb.openExisting());

  {
    QSqlDatabase db = connection->database();
    QVERIFY(db.isOpen());
    QSqlQuery query(db);

    QVERIFY(query.exec("INSERT INTO Person (Id_PK) VALUES (5)"));
    QVERIFY(query.exec("INSERT INTO Address (Id_PK, Person_Id_FK) VALUES (15,5)"));
    QVERIFY(!query.exec("INSERT INTO Address (Id_PK, Person_Id_FK) VALUES (16,6)"));
  }

  Connection::remove(*connection);
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

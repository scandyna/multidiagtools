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
#include "SQLiteDatabase.h"
#include "Mdt/ErrorCode.h"
#include "Mdt/Sql/Error.h"
#include <QLatin1String>
#include <QLatin1Char>
#include <QStringList>
#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>

namespace Mdt{ namespace Sql{

SQLiteDatabase::SQLiteDatabase(const Connection & connection, const SQLiteConnectionParameters & parameters, QObject *parent)
 : QObject(parent),
   mConnection(connection),
   mParameters(parameters)
{
  Q_ASSERT(mConnection.database().isValid());
  Q_ASSERT(hasSQLiteDriverLoaded(mConnection.database()));
}

bool SQLiteDatabase::createNew()
{
  QFileInfo fi(mParameters.databaseFile());

  if(fi.isDir()){
    const auto msg = tr("Create a new SQLite database failed because file path refers to a directory.\nFile path: '%1'")
                     .arg( fi.absoluteFilePath() );
    const auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    setLastError(error);
    return false;
  }
  if(fi.exists()){
    const auto msg = tr("Create a new SQLite database failed because file named '%1' allready exists.\nDirectory: '%2'")
                     .arg( fi.fileName(), fi.absoluteDir().path() );
    const auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    setLastError(error);
    return false;
  }
  if(mParameters.openMode() == SQLiteOpenMode::ReadOnly){
    mParameters.setOpenMode(SQLiteOpenMode::ReadWrite);
    const auto result = open();
    mParameters.setOpenMode(SQLiteOpenMode::ReadOnly);
    mConnection.database().close();
    if(!result){
      return false;
    }
  }

  return open();
}

bool SQLiteDatabase::openExisting()
{
  QFileInfo fi(mParameters.databaseFile());

  if(!fi.exists()){
    const auto msg = tr("Open SQLite database failed because file named '%1' does not exist.\nDirectory: '%2'")
                     .arg( fi.fileName(), fi.absoluteDir().path() );
    const auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    setLastError(error);
    return false;
  }
  if(fi.isDir()){
    const auto msg = tr("Open SQLite database failed because file path refers to a directory.\nFile path: '%1'")
                     .arg( fi.absoluteFilePath() );
    const auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    setLastError(error);
    return false;
  }

  return open();
}

bool SQLiteDatabase::open()
{
  QSqlDatabase db = mConnection.database();

  db.close();
  setConnectOptions(mParameters.openMode());
  db.setDatabaseName(mParameters.databaseFile());
  if(!db.open()){
    const QFileInfo fi(mParameters.databaseFile());
    const auto msg = tr("Open SQLite database file '%1' failed.\nDirectory: '%2'")
                     .arg( fi.fileName(), fi.absoluteDir().path() );
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    error.stackError( mdtErrorFromQSqlDatabaseQ(db, this) );
    setLastError(error);
    return false;
  }
  if(!isSQLiteDatabaseOpen()){
    db.close();
    const QFileInfo fi(mParameters.databaseFile());
    const auto msg = tr("Open SQLite database failed because file named '%1' is not a SQLite database (or it is encrypted).\nDirectory: '%2'")
                     .arg( fi.fileName(), fi.absoluteDir().path() );
    const auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    setLastError(error);
    return false;
  }
  if(!enableForeignKeySupport(db)){
    return false;
  }

  return true;
}

bool SQLiteDatabase::isSQLiteDatabaseOpen()
{
  QSqlDatabase db = mConnection.database();

  if(!db.isOpen()){
    return false;
  }
  if(getSchemaVersion(db)){
    return true;
  }

  return false;
}

Mdt::Expected<Connection> SQLiteDatabase::addConnection(const QString & connectionNamePrefix)
{
  const auto connectionName = Connection::generateConnectionName( QSqlDatabase::connectionNames(), connectionNamePrefix );
  QSqlDatabase db;

  if( QSqlDatabase::isDriverAvailable(QLatin1String("MDTQSQLITE")) ){
    db = QSqlDatabase::addDatabase(QLatin1String("MDTQSQLITE"), connectionName);
  }else{
    db = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"), connectionName);
  }
  if(!db.isValid()){
    const auto msg = tr("Could not load a SQLite driver. Searched in:\n%1")
                     .arg( QCoreApplication::libraryPaths().join(QLatin1Char('\n')) );
    const auto error = mdtErrorNewT(Mdt::ErrorCode::DriverNotFound, msg, Mdt::Error::Critical, "SQLiteDatabase");
    return error;
  }

  return Connection(connectionName);
}

bool SQLiteDatabase::isSQLiteDriver(const QString & driverName) noexcept
{
  return (driverName == QLatin1String("QSQLITE")) || (driverName == QLatin1String("MDTQSQLITE"));
}

bool SQLiteDatabase::hasSQLiteDriverLoaded(const QSqlDatabase& db) noexcept
{
  return db.isValid() && isSQLiteDriver(db.driverName());
}

Mdt::Expected<qlonglong> SQLiteDatabase::getSchemaVersion(const QSqlDatabase & db)
{
  Q_ASSERT(hasSQLiteDriverLoaded(db));
  Q_ASSERT(db.isOpen());

  QSqlQuery query(db);
  if(!query.exec(QLatin1String("PRAGMA schema_version"))){
    const auto msg = tr("Get schema version failed.");
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    error.stackError( mdtErrorFromQSqlQueryQ(query, this) );
    setLastError(error);
    return error;
  }
  if(!query.next()){
    const auto msg = tr("Get schema version failed: query returned no result.");
    const auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    setLastError(error);
    return error;
  }
  Q_ASSERT(query.record().count() > 0);

  return query.value(0).toLongLong();
}

bool SQLiteDatabase::enableForeignKeySupport(const QSqlDatabase & db)
{
  Q_ASSERT(hasSQLiteDriverLoaded(db));
  Q_ASSERT(db.isOpen());

  QSqlQuery query(db);
  if(!query.exec(QLatin1String("PRAGMA foreign_keys = ON"))){
    const auto msg = tr("Enabling foreign keys support failed.");
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    error.stackError( mdtErrorFromQSqlQueryQ(query, this) );
    setLastError(error);
    return false;
  }

  return true;
}

void SQLiteDatabase::setConnectOptions(SQLiteOpenMode openMode)
{
  QSqlDatabase db = mConnection.database();

  Q_ASSERT(hasSQLiteDriverLoaded(db));
  Q_ASSERT(!db.isOpen());

  if(openMode == SQLiteOpenMode::ReadOnly){
    db.setConnectOptions(QLatin1String("QSQLITE_USE_EXTENDED_RESULT_CODES;QSQLITE_OPEN_READONLY"));
  }else{
    db.setConnectOptions(QLatin1String("QSQLITE_USE_EXTENDED_RESULT_CODES"));
  }
}


void SQLiteDatabase::setLastError(const Mdt::Error & error)
{
  Q_ASSERT(!error.isNull());

  mLastError = error;
}

}} // namespace Mdt{ namespace Sql{

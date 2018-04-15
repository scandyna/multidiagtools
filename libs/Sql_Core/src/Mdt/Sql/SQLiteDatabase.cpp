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
#include "SQLiteDatabase.h"
#include "Mdt/Sql/Error.h"
#include <QFileInfo>
#include <QDir>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>

namespace Mdt{ namespace Sql{

SQLiteDatabase::SQLiteDatabase(const QString & connectionName)
{
  mDatabase = QSqlDatabase::database(connectionName, false);
  if( !mDatabase.isValid() || (mDatabase.driverName() != QLatin1String("QSQLITE")) ){
    mDatabase = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"), connectionName);
  }
}

bool SQLiteDatabase::createNew(const QString& dbFilePath)
{
  QFileInfo fi(dbFilePath);

  mDatabase.close();
  if(fi.isDir()){
    const auto msg = tr("Create a new SQLite database failed because file path refers to a directory.\nFile path: '%1'")
                     .arg( fi.absoluteFilePath() );
    const auto error = mdtErrorNew(msg, Mdt::Error::Critical, "SQLiteDatabase");
    setLastError(error);
    return false;
  }
  if(fi.exists()){
    const auto msg = tr("Create a new SQLite database failed because file named '%1' allready exists.\nDirectory: '%2'")
                     .arg( fi.fileName(), fi.absoluteDir().path() );
    const auto error = mdtErrorNew(msg, Mdt::Error::Critical, "SQLiteDatabase");
    setLastError(error);
    return false;
  }
  mDatabase.setDatabaseName(fi.absoluteFilePath());
  mDatabase.setConnectOptions();
  if(!mDatabase.open()){
    const auto msg = tr("Create a new SQLite database file named '%1' failed.\nDirectory: '%2'")
                     .arg( fi.fileName(), fi.absoluteDir().path() );
    auto error = mdtErrorNew(msg, Mdt::Error::Critical, "SQLiteDatabase");
    error.stackError( mdtErrorFromQSqlDatabase(mDatabase, "SQLiteDatabase") );
    setLastError(error);
    return false;
  }

  return true;
}

bool SQLiteDatabase::openExisting(const QString & dbFilePath, SQLiteDatabase::OpenMode openMode)
{
  QFileInfo fi(dbFilePath);

  mDatabase.close();
  if(!fi.exists()){
    const auto msg = tr("Open SQLite database failed because file named '%1' does not exist.\nDirectory: '%2'")
                     .arg( fi.fileName(), fi.absoluteDir().path() );
    const auto error = mdtErrorNew(msg, Mdt::Error::Critical, "SQLiteDatabase");
    setLastError(error);
    return false;
  }
  if(fi.isDir()){
    const auto msg = tr("Open SQLite database failed because file path refers to a directory.\nFile path: '%1'")
                     .arg( fi.absoluteFilePath() );
    const auto error = mdtErrorNew(msg, Mdt::Error::Critical, "SQLiteDatabase");
    setLastError(error);
    return false;
  }
  mDatabase.setDatabaseName(fi.absoluteFilePath());
  if(openMode == ReadOnly){
    mDatabase.setConnectOptions(QLatin1String("QSQLITE_OPEN_READONLY"));
  }else{
    mDatabase.setConnectOptions();
  }
  if(!mDatabase.open()){
    const auto msg = tr("Open SQLite database file named '%1' failed.\nDirectory: '%2'")
                     .arg( fi.fileName(), fi.absoluteDir().path() );
    auto error = mdtErrorNew(msg, Mdt::Error::Critical, "SQLiteDatabase");
    error.stackError( mdtErrorFromQSqlDatabase(mDatabase, "SQLiteDatabase") );
    setLastError(error);
    return false;
  }
  const auto schemaVersion = getSchemaVersion();
  if(!schemaVersion){
    mDatabase.close();
    const auto msg = tr("Open SQLite database failed because file named '%1' is not a SQLite database (or it is encrypted).\nDirectory: '%2'")
                     .arg( fi.fileName(), fi.absoluteDir().path() );
    const auto error = mdtErrorNew(msg, Mdt::Error::Critical, "SQLiteDatabase");
    setLastError(error);
    return false;
  }

  return true;
}

Expected<qlonglong> SQLiteDatabase::getSchemaVersion()
{
  Q_ASSERT(mDatabase.isOpen());

  QSqlQuery query(mDatabase);
  if(!query.exec(QLatin1String("PRAGMA schema_version"))){
    const auto msg = tr("Get schema version failed.");
    auto error = mdtErrorNew(msg, Mdt::Error::Critical, "SQLiteDatabase");
    error.stackError( mdtErrorFromQSqlQuery(query, "SQLiteDatabase") );
    setLastError(error);
    return mLastError;
  }
  if(!query.next()){
    const auto msg = tr("Get schema version failed: query returned no result.");
    const auto error = mdtErrorNew(msg, Mdt::Error::Critical, "SQLiteDatabase");
    setLastError(error);
    return mLastError;
  }
  Q_ASSERT(query.record().count() > 0);

  return query.value(0).toLongLong();
}

void SQLiteDatabase::setLastError(const Mdt::Error & error)
{
  mLastError = error;
//   mLastError.commit();
}

}} // namespace Mdt{ namespace Sql{

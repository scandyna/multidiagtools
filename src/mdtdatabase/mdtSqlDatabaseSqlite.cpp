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
#include "mdtSqlDatabaseSqlite.h"
#include "mdtSqlDriverType.h"
#include "mdtSqlForeignKeySetting.h"
#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QFile>
#include <QObject>

mdtSqlDatabaseSqlite::mdtSqlDatabaseSqlite()
{
}

mdtSqlDatabaseSqlite::mdtSqlDatabaseSqlite(const QSqlDatabase & db)
 : pvDatabase(db)
{
  Q_ASSERT(mdtSqlDriverType::typeFromName(db.driverName()) == mdtSqlDriverType::SQLite);
}

bool mdtSqlDatabaseSqlite::isValid() const
{
  return (pvDatabase.isValid() && mdtSqlDriverType::typeFromName(pvDatabase.driverName()) == mdtSqlDriverType::SQLite);
}

void mdtSqlDatabaseSqlite::clear()
{
  /// \todo close
  pvDatabase = QSqlDatabase();
}

void mdtSqlDatabaseSqlite::setDatabase(const QSqlDatabase & db)
{
  Q_ASSERT(mdtSqlDriverType::typeFromName(db.driverName()) == mdtSqlDriverType::SQLite);
  pvDatabase = db;
}

bool mdtSqlDatabaseSqlite::openDatabase(const QFileInfo & fileInfo)
{
  Q_ASSERT(isValid());
  Q_ASSERT(!isOpen());
  Q_ASSERT(!fileInfo.filePath().isEmpty());

  // Check if file exists
  if(!fileInfo.exists()){
    pvLastError.setError(tr("File '") + fileInfo.fileName() + tr("' does not exist.") + "\n"\
                           + tr("Directory: '") + fileInfo.absoluteDir().path() + tr("'."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseSqlite");
    pvLastError.commit();
    return false;
  }
  // Open database
  pvDatabase.setDatabaseName(fileInfo.absoluteFilePath());

  return openDatabasePv();
}

bool mdtSqlDatabaseSqlite::openDatabase()
{
  Q_ASSERT(isValid());
  Q_ASSERT(!isOpen());
  Q_ASSERT(!pvDatabase.databaseName().isEmpty());

  return openDatabasePv();
}

bool mdtSqlDatabaseSqlite::createDatabase(const QFileInfo & fileInfo, mdtSqlDatabaseSqlite::CreateMode createMode)
{
  Q_ASSERT(isValid());
  Q_ASSERT(!isOpen());
  Q_ASSERT(!fileInfo.filePath().isEmpty());

  // Check if directory exists
  if(!fileInfo.absoluteDir().exists()){
    pvLastError = mdtError(tr("Directory '") + fileInfo.absoluteDir().path() + tr("' does not exists.") , mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseSqlite");
    pvLastError.commit();
    return false;
  }
  // Check if file exists
  if(fileInfo.exists()){
    switch(createMode){
      case KeepExisting:
        break;
      case FailIfExists:
        pvLastError = mdtError(tr("Database '") + fileInfo.fileName() + tr("' allready exists") , mdtError::Error);
        MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseSqlite");
        pvLastError.commit();
        return false;
      case OverwriteExisting:
        {
          QFile file(fileInfo.absoluteFilePath());
          if(!file.resize(0)){
            pvLastError = mdtError(tr("File truncate failed for file '") + fileInfo.absoluteFilePath() + "'", mdtError::Error);
            MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseSqlite");
            pvLastError.setSystemError(file.error(), file.errorString());
            pvLastError.commit();
            return false;
          }
        }
        break;
    }
  }
  // Open database
  pvDatabase.setDatabaseName(fileInfo.absoluteFilePath());

  return openDatabasePv();
}

bool mdtSqlDatabaseSqlite::deleteDatabase(const QString & filePath)
{
  Q_ASSERT(!isOpen());

  QFile file(filePath);

  if(!file.remove()){
    pvLastError = mdtError(tr("Cannot remove file '") + filePath + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseSqlite");
    pvLastError.setSystemError(file.error(), file.errorString());
    pvLastError.commit();
    return false;
  }
  clearDatabaseName();

  return true;
}

QStringList mdtSqlDatabaseSqlite::getConnectionNames()
{
  QStringList connectionNames;

  for(const auto & cnnName : QSqlDatabase::connectionNames()){
    QSqlDatabase db = QSqlDatabase::database(cnnName, false);
    if(mdtSqlDriverType::typeFromName(db.driverName()) == mdtSqlDriverType::SQLite){
      connectionNames.append(cnnName);
    }
  }

  return connectionNames;
}

bool mdtSqlDatabaseSqlite::openDatabasePv()
{
  if(!pvDatabase.open()){
    QFileInfo fileInfo(pvDatabase.databaseName());
    pvLastError = mdtError(tr("Cannot open database '") + fileInfo.fileName() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseSqlite");
    pvLastError.setInformativeText(tr("Check that you have write access to directory '") + fileInfo.absoluteDir().path() + tr("'."));
    pvLastError.setSystemError(pvDatabase.lastError().number(), pvDatabase.lastError().text());
    pvLastError.commit();
    return false;
  }
  Q_ASSERT(isOpen());
  mdtSqlForeignKeySetting fkSetting(pvDatabase, mdtSqlForeignKeySetting::Permanent);
  if(!fkSetting.enable()){
    pvLastError = fkSetting.lastError();
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseSqlite");
    pvLastError.commit();
    close();
    return false;
  }
  QSqlQuery query(pvDatabase);
  // At default, synchronous write is set to FULL. This is safe, but really slow..
  if(!query.exec("PRAGMA synchronous = NORMAL")){
    pvLastError.setError(tr("Cannot set PRAGMA synchronous to NORMAL (its probably set to FULL now). Database: '") + pvDatabase.databaseName() + "'", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlDatabaseSqlite");
    pvLastError.setSystemError(pvDatabase.lastError().number(), pvDatabase.lastError().text());
    pvLastError.commit();
    close();
    return false;
  }
  return true;
}

QString mdtSqlDatabaseSqlite::tr(const char* sourceText, const char* disambiguation, int n)
{
  return QObject::tr(sourceText, disambiguation, n);
}

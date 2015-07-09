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
#include "mdtSqlForeignKeySetting.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QObject>

mdtSqlForeignKeySetting::mdtSqlForeignKeySetting(QSqlDatabase db, bool keep)
 : pvDatabase(db)
{
  Q_ASSERT(pvDatabase.isOpen());

  if(keep){
    pvState = Keep;
  }else{
    pvState = Original;
  }
}

mdtSqlForeignKeySetting::~mdtSqlForeignKeySetting()
{
  switch(pvState){
    case Enabled:
      disable();
      break;
    case Disabled:
      enable();
      break;
    case Original:
    case Keep:
      break;
  }
}

bool mdtSqlForeignKeySetting::enable()
{
  QString driverName = pvDatabase.driverName();

  // Check state
  if(pvState == Enabled){
    return true;
  }
  // Call enable function regarding database engine
  if(driverName == "QSQLITE"){
    if(!enableSqlite()){
      return false;
    }
  }else{
    // Unsupported database engine
    QString msg = QObject::tr("Cannot enable foreign keys constraint. Driver '") + driverName + QObject::tr("' refers to a unsupported database engine.");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlForeignKeySetting");
    pvLastError.commit();
    return false;
  }
  // Update state
  switch(pvState){
    case Original:
    case Disabled:
      pvState = Enabled;
      break;
    case Enabled:
    case Keep:
      break;
  }

  return true;
}

bool mdtSqlForeignKeySetting::disable()
{
  QString driverName = pvDatabase.driverName();

  // Check state
  if(pvState == Disabled){
    return true;
  }
  // Call disable function regarding database engine
  if(driverName == "QSQLITE"){
    if(!disableSqlite()){
      return false;
    }
  }else{
    QString msg = QObject::tr("Cannot disable foreign keys constraint. Driver '") + driverName + QObject::tr("' refers to a unsupported database engine.");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlForeignKeySetting");
    pvLastError.commit();
    return false;
  }
  // Update state
  switch(pvState){
    case Original:
    case Enabled:
      pvState = Disabled;
      break;
    case Disabled:
    case Keep:
      break;
  }

  return true;
}

bool mdtSqlForeignKeySetting::enableSqlite()
{
  QSqlQuery query(pvDatabase);

  if(!query.exec("PRAGMA foreign_keys = ON")){
    QSqlError sqlError = query.lastError();
    QString msg = QObject::tr("Cannot enable foreign keys constraint on database '") + pvDatabase.databaseName() + QObject::tr("'.");
    pvLastError.setError(msg, mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlForeignKeySetting");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtSqlForeignKeySetting::disableSqlite()
{
  QSqlQuery query(pvDatabase);

  if(!query.exec("PRAGMA foreign_keys = OFF")){
    QSqlError sqlError = query.lastError();
    QString msg = QObject::tr("Cannot disable foreign keys constraint on database '") + pvDatabase.databaseName() + QObject::tr("'.");
    pvLastError.setError(msg, mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlForeignKeySetting");
    pvLastError.commit();
    return false;
  }

  return true;
}

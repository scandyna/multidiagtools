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
#include "ErrorDriver.h"
#include "AbstractErrorDriver.h"
#include "AbstractErrorDriverPlugin.h"
#include "PluginInstance.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QLatin1String>
#include <QGlobalStatic>
#include <QMutex>
#include <QMutexLocker>

namespace Mdt{ namespace Sql{

Q_GLOBAL_STATIC(PluginInstance, currentPlugin)
Q_GLOBAL_STATIC(QMutex, currentPluginMutex)

ErrorDriver::ErrorDriver(const QSqlDriver*const sqlDriver, QObject *parent)
 : QObject(parent)
{
  if(sqlDriver == nullptr){
    init( QString() );
  }else{
    init( driverNameFromDbmsType(sqlDriver->dbmsType()) );
  }
}

ErrorDriver::ErrorDriver(const QSqlDatabase & db, QObject *parent)
 : QObject(parent)
{
  init( db.driverName() );
}

ErrorDriver::ErrorDriver(const QString & sqlDriverName, QObject *parent)
 : QObject(parent)
{
  init(sqlDriverName);
}

ErrorDriver::~ErrorDriver()
{
}

Mdt::ErrorCode ErrorDriver::errorCode(const QSqlError& sqlError) const
{
  if(isValid()){
    return mImpl->errorCode(sqlError);
  }
  return Mdt::ErrorCode::UnknownError;
}

Mdt::Error::Level ErrorDriver::errorLevel(const QSqlError& sqlError) const
{
  if(isValid()){
    return mImpl->errorLevel(sqlError);
  }
  return Mdt::Error::Critical;
}

void ErrorDriver::init(const QString & sqlDriverName)
{
  if(sqlDriverName.trimmed().isEmpty()){
    const QString msg = tr("No SQL driver provided, all errors will be unknown.");
    const auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    setInitError(error);
    return;
  }

  QMutexLocker lock(currentPluginMutex);
  QObject *driver = currentPlugin->create<AbstractErrorDriverPlugin>(QLatin1String("Mdt.Sql.AbstractErrorDriver"), sqlDriverName);
  if(driver == nullptr){
    const QString msg = tr("Could not find a error driver for SQL driver '%1'.")
                        .arg(sqlDriverName);
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    error.stackError( currentPlugin->lastError() );
    setInitError(error);
    return;

  }
  mImpl.reset( qobject_cast<AbstractErrorDriver*>(driver) );
  if(!mImpl){
    const QString msg = tr("Could find a error driver for SQL driver '%1', but it is not a real subclass of Mdt::Sql::AbstractErrorDriver.")
                        .arg(sqlDriverName);
    const auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    setInitError(error);
    return;
  }
}

void ErrorDriver::setInitError(const Mdt::Error & error)
{
  mInitError = error;
}

QString ErrorDriver::driverNameFromDbmsType(QSqlDriver::DbmsType dbmsType)
{
  switch(dbmsType){
    case QSqlDriver::MSSqlServer:
      return QLatin1String("QODBC");
    case QSqlDriver::MySqlServer:
      return QLatin1String("QMYSQL");
    case QSqlDriver::PostgreSQL:
      return QLatin1String("QPSQL");
    case QSqlDriver::Oracle:
      return QLatin1String("QOCI");
    case QSqlDriver::Sybase:
      return QLatin1String("QTDS");
    case QSqlDriver::SQLite:
      return QLatin1String("QSQLITE");
    case QSqlDriver::Interbase:
      return QLatin1String("QIBASE");
    case QSqlDriver::DB2:
      return QLatin1String("QDB2");
    case QSqlDriver::UnknownDbms:
      return QString();
  }
  return QString();
}

}} // namespace Mdt{ namespace Sql{

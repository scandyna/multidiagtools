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
#include "Driver.h"
#include "DriverImplementationInterface.h"
#include "DriverSQLite.h"
// #include "DriverMySql.h"
// #include "DriverPostgreSQL.h"
#include <QObject>

namespace Mdt{ namespace Sql{ namespace Schema{

Driver::Driver(const Connection & connection)
{
  const QSqlDatabase db = connection.database();

  Q_ASSERT(db.isValid());
  Q_ASSERT(db.driver() != nullptr);

  auto type = typeFromDbmsType(db.driver()->dbmsType());
  switch(type){
    case DriverType::SQLite:
      mImpl = std::make_unique<DriverSQLite>(connection);
      break;
    case DriverType::MySQL:
//       mImpl = std::make_unique<DriverMySql>(db);
      break;
    case DriverType::PostgreSQL:
//       mImpl = std::make_unique<DriverPostgreSQL>(db);
      break;
    case DriverType::MariaDB:
    case DriverType::Unknown:
      break;
  }
  if(!mImpl){
    const QString msg = tr("Database engine with Qt driver name '%1' is not supported. Database connection name: '%2'.") \
                        .arg(db.driverName(), db.connectionName());
    mLastError = mdtErrorNew(msg, Mdt::Error::Critical, "Mdt::Sql::Schema::Driver");
    mLastError.commit();
  }
}

Driver::~Driver()
{
}

DriverType Driver::type() const
{
  if(!mImpl){
    return DriverType::Unknown;
  }
  return mImpl->type();
}

FieldTypeList Driver::getAvailableFieldTypeList() const
{
  Q_ASSERT(isValid());
  return mImpl->getAvailableFieldTypeList();
}

bool Driver::createTable(const Table & table)
{
  Q_ASSERT(isValid());

  if(!mImpl->createTable(table)){
    mLastError = mImpl->lastError();
    return false;
  }
  return true;
}

bool Driver::dropTable(const Table & table)
{
  Q_ASSERT(isValid());

  if(!mImpl->dropTable(table)){
    mLastError = mImpl->lastError();
    return false;
  }
  return true;
}

Expected<PrimaryKeyContainer> Driver::getTablePrimaryKeyFromDatabase(const QString & tableName) const
{
  Q_ASSERT(isValid());

  auto ret = mImpl->getTablePrimaryKeyFromDatabase(tableName);
  if(!ret){
    mLastError = ret.error();
  }
  return ret;
}

// Expected<PrimaryKeyContainer> Driver::getTablePrimaryKeyFromDatabase(const Table& table) const
// {
//   Q_ASSERT(isValid());
// 
//   return getTablePrimaryKeyFromDatabase(table.tableName());
// }

Expected<ForeignKeyList> Driver::getTableForeignKeyListFromDatabase(const QString& tableName) const
{
  Q_ASSERT(isValid());

  auto ret = mImpl->getTableForeignKeyListFromDatabase(tableName);
  if(!ret){
    mLastError = ret.error();
  }
  return ret;
}

bool Driver::createView(const View & view)
{
  Q_ASSERT(isValid());

  if(!mImpl->createView(view)){
    mLastError = mImpl->lastError();
    return false;
  }
  return true;
}

bool Driver::dropView(const View & view)
{
  Q_ASSERT(isValid());

  if(!mImpl->dropView(view)){
    mLastError = mImpl->lastError();
    return false;
  }
  return true;
}

bool Driver::populateTable(const TablePopulation & tp)
{
  Q_ASSERT(isValid());

  if(!mImpl->populateTable(tp)){
    mLastError = mImpl->lastError();
    return false;
  }
  return true;
}

bool Driver::createSchema(const Schema& schema)
{
  Q_ASSERT(isValid());

  if(!mImpl->createSchema(schema)){
    mLastError = mImpl->lastError();
    return false;
  }
  return true;
}

bool Driver::dropSchema(const Schema& schema)
{
  Q_ASSERT(isValid());

  if(!mImpl->dropSchema(schema)){
    mLastError = mImpl->lastError();
    return false;
  }
  return true;
}

QString Driver::tr(const char* sourceText)
{
  return QObject::tr(sourceText);
}

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

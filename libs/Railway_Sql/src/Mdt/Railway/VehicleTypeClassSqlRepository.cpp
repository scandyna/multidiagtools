/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "VehicleTypeClassSqlRepository.h"
#include "Mdt/Railway/Entity/VehicleTypeClass.h"
#include "Mdt/Entity/SqlSelectStatement.h"
#include "Mdt/Entity/SqlData.h"
#include "Mdt/Sql/Error.h"
#include "Mdt/Sql/InsertQuery.h"
#include "Mdt/Sql/FieldName.h"
#include <QString>
#include <QSqlQuery>

#include <QDebug>

namespace Mdt{ namespace Railway{

// VehicleTypeClassSqlRepository::VehicleTypeClassSqlRepository(const QSqlDatabase& dbConnection)
//  : mDbConnection(dbConnection)
// {
//   Q_ASSERT(mDbConnection.isValid());
// }

void VehicleTypeClassSqlRepository::setDbConnection(const QSqlDatabase& dbConnection)
{
  Q_ASSERT(dbConnection.isValid());

  mDbConnection = dbConnection;
}

bool VehicleTypeClassSqlRepository::fetchRecords(int count)
{
  const auto sql = Mdt::Entity::SqlSelectStatement::selectAllFromEntity<Entity::VehicleTypeClassEntity>();

  qDebug() << "SQL: " << sql;
  
  QSqlQuery query(mDbConnection);
  if(!query.exec(sql)){
    const auto msg = tr("Fetching records for vehicle type class failed.\nSQL: '%1'")
                     .arg(sql);
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    error.stackError( mdtErrorFromQSqlQueryQ(query, this) );
    setLastError(error);
    return false;
  }
  for(int i = 0; i < count; ++i){
    if(!query.next()){
      return true;
    }
    qDebug() << "Fetching " << query.record();
    appendRecordToCache( Mdt::Entity::SqlData::entityDataFromQSqlRecord<VehicleTypeClassData>(query.record()) );
  }

  return true;
}

bool VehicleTypeClassSqlRepository::insertRecordToStorage(const VehicleTypeClassData& record)
{
  using Mdt::Sql::FieldName;

  Mdt::Sql::InsertQuery query(mDbConnection);

  query.setTableName(record.def().entityName());
  query.addValue( FieldName(record.def().name().fieldName()), record.name() );
  query.addValue( FieldName(record.def().alias().fieldName()), record.alias() );
  if(!query.exec()){
    setLastError(query.lastError());
    return false;
  }
  
  return true;
}

}} // namespace Mdt{ namespace Railway{

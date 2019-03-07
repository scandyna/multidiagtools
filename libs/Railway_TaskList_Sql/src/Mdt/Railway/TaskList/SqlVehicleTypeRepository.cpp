/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#include "SqlVehicleTypeRepository.h"
#include "Mdt/Railway/TaskList/VehicleTypeDataStruct.h"
#include "Mdt/Railway/TaskList/VehicleType_p.h"
#include "Mdt/Sql/InsertQuery.h"
#include "Mdt/Sql/Reflection/InsertStatement.h"

namespace Mdt{ namespace Railway{ namespace TaskList{

void SqlVehicleTypeRepository::setDatabase(const QSqlDatabase & db)
{
  Q_ASSERT(db.isValid());

  mDatabase = db;
}

Mdt::Expected<VehicleTypeId> SqlVehicleTypeRepository::add(const VehicleType & vehicle)
{
  const auto statement = Mdt::Sql::Reflection::insertStatementFromReflected<VehicleTypePrimaryKey>( privateConstVehicleTypeDataStruct(vehicle) );
  Mdt::Sql::InsertQuery query(mDatabase);

  if(!query.execStatement(statement)){
    return query.lastError(); /// \todo Maybe a better error message should be generated here
  }

  return VehicleTypeId::fromQVariant(query.lastInsertId());
}

Mdt::Expected<VehicleType> SqlVehicleTypeRepository::getById(VehicleTypeId id) const
{
  Q_ASSERT(!id.isNull());

}

bool SqlVehicleTypeRepository::getAllAsync() const
{
}

}}} // namespace Mdt{ namespace Railway{ namespace TaskList{

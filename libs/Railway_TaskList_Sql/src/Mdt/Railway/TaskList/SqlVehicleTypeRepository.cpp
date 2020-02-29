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
#include "Mdt/Sql/SelectQuery.h"
// #include "Mdt/Sql/Reflection/InsertStatement.h"
// #include "Mdt/QueryExpression/ReflectionSelectStatement.h"

using namespace Mdt::QueryExpression;

namespace Mdt{ namespace Railway{ namespace TaskList{

SqlVehicleTypeRepository::SqlVehicleTypeRepository(const QSqlDatabase & db, QObject *parent)
 : VehicleTypeRepository(parent),
   mTable(db)
{
  Q_ASSERT(db.isValid());
}

// void SqlVehicleTypeRepository::setDatabase(const QSqlDatabase & db)
// {
//   Q_ASSERT(db.isValid());
// 
//   mDatabase = db;
// }

Mdt::Expected<VehicleTypeId> SqlVehicleTypeRepository::add(const VehicleType & vehicle)
{
  const auto id = mTable.add( privateConstVehicleTypeDataStruct(vehicle) );
  if(!id){
    /// \todo Generate a more domain specific error message here
    return id.error();
  }

  return VehicleTypeId(*id);
}

Mdt::Expected<VehicleType> SqlVehicleTypeRepository::getById(VehicleTypeId id) const
{
  Q_ASSERT(!id.isNull());

  const auto vehicleTypeData = mTable.get( id.value() );
  if(!vehicleTypeData){
    /// \todo Generate a more domain specific error message here
    return vehicleTypeData.error();
  }

  return vehicleTypeFromDataStruct(*vehicleTypeData);
}

Mdt::ExpectedResult SqlVehicleTypeRepository::update(const VehicleType & vehicle)
{
  const auto & data = privateConstVehicleTypeDataStruct(vehicle);
  Q_ASSERT(data.id > 0);

  const auto result = mTable.update(data);
  if(!result){
    /// \todo Generate a more domain specific error message here
  }

  return result;
}

Mdt::ExpectedResult SqlVehicleTypeRepository::remove(VehicleTypeId id)
{
  Q_ASSERT(!id.isNull());

  const auto result = mTable.remove( id.value() );
  if(!result){
    /// \todo Generate a more domain specific error message here
  }

  return result;
}

Mdt::ExpectedResult SqlVehicleTypeRepository::removeAll()
{
  const auto result = mTable.removeAll();
  if(!result){
    /// \todo Generate a more domain specific error message here
  }

  return result;
}

Mdt::ExpectedResult SqlVehicleTypeRepository::getAllAsync() const
{
}

}}} // namespace Mdt{ namespace Railway{ namespace TaskList{

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
#include "ChooseVehicleTypeCache.h"
#include "Entity/VehicleTypeClass.h"

using namespace Mdt::Entity;

namespace Mdt{ namespace Railway{

ChooseVehicleTypeCache::ChooseVehicleTypeCache(QObject* parent)
 : AbstractAsyncReadOnlyCache(parent)
{
  QueryEntity<Entity::VehicleTypeClassEntity> vehicleTypeClass;

  const auto vehicleTypeId = mSelectStatement.makeSelectField( mSelectStatement.def().id() );
  const auto vehicleTypeClassId = vehicleTypeClass.makeSelectField( vehicleTypeClass.def().id() );
//   const auto vehicleTypeClassIdFk = mSelectStatement.makeSelectField( mSelectStatement.def().vehicleTypeClassId() );
  const auto vehicleTypeName = vehicleTypeClass.makeSelectField( vehicleTypeClass.def().name() );
  const auto manufacturerSerie = mSelectStatement.makeSelectField( mSelectStatement.def().manufacturerSerie() );

  mSelectStatement.addField( vehicleTypeName );
  mSelectStatement.addField( manufacturerSerie );
  mSelectStatement.addField( vehicleTypeId );
  mSelectStatement.addField( vehicleTypeClassId );
//   mSelectStatement.joinEntity( vehicleTypeClass, vehicleTypeClassIdFk == vehicleTypeClassId );
  mSelectStatement.joinEntity<Mdt::Railway::Entity::VehicleTypeClassVehicleTypeRelation>();

//   mSelectStatement.addField( mSelectStatement.def().id() );
//   mSelectStatement.addField( mSelectStatement.def().vehicleTypeClassId() );
//   mSelectStatement.addField( mSelectStatement.def().manufacturerSerie() );
}

void ChooseVehicleTypeCache::setQueryFactory(const std::shared_ptr<ChooseVehicleTypeCache::SelectQueryFactory> & factory)
{
  Q_ASSERT(factory.get() != nullptr);

  mSelectQueryFactory = factory;
  mSelectQuery = std::move( mSelectQueryFactory->createSelectQuery() );
  connect( mSelectQuery.get(), &SelectQuery::newRecordAvailable, this, &ChooseVehicleTypeCache::fromBackendAppendRecord );
  connect( mSelectQuery.get(), &SelectQuery::errorOccured, this, &ChooseVehicleTypeCache::setLastError );
}

QString ChooseVehicleTypeCache::horizontalHeaderName(int column) const
{
  switch(column){
    case 0:
      return QStringLiteral("Name");
    case 1:
      return QStringLiteral("Serie");
    case 2:
      return QStringLiteral("Id");
    case 3:
      return QStringLiteral("Vehicle type class Id");
    default:
      break;
  }
  return QString();
}

VehicleTypeId ChooseVehicleTypeCache::id(int row) const noexcept
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());

  return VehicleTypeId::fromQVariant( data(row, idColumn()) );
}

bool ChooseVehicleTypeCache::fetchRecords(int count)
{
  mSelectQuery->submitStatement(mSelectStatement, count);

  return true;
}

}} // namespace Mdt{ namespace Railway{

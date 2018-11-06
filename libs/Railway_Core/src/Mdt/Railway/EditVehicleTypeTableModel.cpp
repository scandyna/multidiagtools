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
#include "EditVehicleTypeTableModel.h"
#include "Entity/VehicleTypeClass.h"

using Mdt::Entity::QueryEntity;

namespace Mdt{ namespace Railway{

EditVehicleTypeTableModel::EditVehicleTypeTableModel(QObject* parent)
 : BaseClass(parent)
{
//   QueryEntity<Entity::VehicleTypeClassEntity> vehicleTypeClass;
// 
//   const auto vehicleTypeId = mSelectStatement.makeSelectField( mSelectStatement.def().id() );
//   const auto vehicleTypeClassId = vehicleTypeClass.makeSelectField( vehicleTypeClass.def().id() );
//   const auto vehicleTypeName = vehicleTypeClass.makeSelectField( vehicleTypeClass.def().name() );
//   const auto manufacturerSerie = mSelectStatement.makeSelectField( mSelectStatement.def().manufacturerSerie() );
// 
//   mSelectStatement.addField( vehicleTypeName );
//   mSelectStatement.addField( manufacturerSerie );
//   mSelectStatement.addField( vehicleTypeId );
//   mSelectStatement.addField( vehicleTypeClassId );
//   mSelectStatement.joinEntity<Mdt::Railway::Entity::VehicleTypeClassVehicleTypeRelation>();
}

void EditVehicleTypeTableModel::setQueryFactory(const std::shared_ptr< SelectQueryFactory >& factory)
{
  Q_ASSERT(factory.get() != nullptr);

  mSelectQueryFactory = factory;
  mSelectQuery = std::move( mSelectQueryFactory->createSelectQuery() );
  connect( mSelectQuery.get(), &SelectQuery::newRecordAvailable, this, &EditVehicleTypeTableModel::fromBackendAppendRecord );
//   connect( mSelectQuery.get(), &SelectQuery::errorOccured, this, &ChooseVehicleTypeCache::setLastError );
}

int EditVehicleTypeTableModel::columnCount(const QModelIndex& /*parent*/) const
{
  return mSelectStatement.fieldCount();
}

QVariant EditVehicleTypeTableModel::horizontalHeaderDisplayRoleData(int column) const
{
  if(column == mSelectStatement.vehicleTypeNameColumn()){
    return tr("Type");
  }
  if(column == mSelectStatement.manufacturerSerieColumn()){
    return tr("Serie");
  }
  if(column == mSelectStatement.vehicleTypeIdColumn()){
    return tr("V. Id");
  }
  if(column == mSelectStatement.vehicleTypeClassIdColumn()){
    return tr("VtC. Id");
  }

  return BaseClass::horizontalHeaderDisplayRoleData(column);
}

// int EditVehicleTypeTableModel::idColumn() const
// {
//   return 2;
// }

bool EditVehicleTypeTableModel::fetchRecords(int count)
{
  mSelectQuery->submitStatement(mSelectStatement, count);

  return true;
}

}} // namespace Mdt{ namespace Railway{

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
#include "SqlSchema.h"
#include "Mdt/Railway/Entity/VehicleTypeClass.h"
#include "Mdt/Railway/Entity/VehicleType.h"
#include "Mdt/Entity/SqlTable.h"
#include "Mdt/Sql/Schema/Driver.h"
#include "Mdt/Sql/Schema/ForeignKeySettings.h"

#include <QDebug>

using Mdt::Entity::SqlTable;
using Mdt::Sql::Schema::ForeignKeyAction;

namespace Mdt{ namespace Railway{

SqlSchema::SqlSchema()
{
}

bool SqlSchema::createSchema(const QSqlDatabase& dbConnection)
{
  Q_ASSERT(dbConnection.isValid());

  Mdt::Sql::Schema::Driver driver(dbConnection);
  if(!driver.isValid()){
    qDebug() << "DB backend probably not supported";
    return false;
  }

  Mdt::Sql::Schema::ForeignKeySettings commonForeignKeySettings;
  commonForeignKeySettings.setIndexed(true);
  commonForeignKeySettings.setOnDeleteAction(ForeignKeyAction::Restrict);
  commonForeignKeySettings.setOnUpdateAction(ForeignKeyAction::Cascade);

  auto vehicleTypeClassTable = SqlTable::fromEntity<Entity::VehicleTypeClassEntity>();
  SqlTable::addUniqueConstraintToTable<Entity::VehicleTypeClassNameUniqueConstraint>(vehicleTypeClassTable);
  if( !driver.createTable(vehicleTypeClassTable) ){
    qDebug() << "Create VehicleTypeClass table failed";
    return false;
  }

  auto vehicleTypeTable = SqlTable::fromEntity<Entity::VehicleTypeEntity>();
  SqlTable::addForeignKeyFromRelationToTable<Entity::VehicleTypeClassVehicleTypeRelation>(vehicleTypeTable, commonForeignKeySettings);
  SqlTable::addUniqueConstraintToTable<Entity::VehicleTypeUniqueConstraint>(vehicleTypeTable);
  if( !driver.createTable(vehicleTypeTable) ){
    qDebug() << "Create VehicleType table failed";
    return false;
  }

//   if( !driver.createTable( Mdt::Entity::SqlTable::fromEntity<Entity::VehicleTypeClassEntity>() ) ){
//     qDebug() << "Create VehicleTypeClass table failed";
//     return false;
//   }
//   if( !driver.createTable( Mdt::Entity::SqlTable::fromEntity<Entity::VehicleTypeEntity>() ) ){
//     qDebug() << "Create VehicleType table failed";
//     return false;
//   }

  return true;
}

}} // namespace Mdt{ namespace Railway{

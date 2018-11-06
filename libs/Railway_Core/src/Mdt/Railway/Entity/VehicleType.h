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
#ifndef MDT_RAILWAY_ENTITY_VEHICLE_TYPE_H
#define MDT_RAILWAY_ENTITY_VEHICLE_TYPE_H

#include "VehicleTypeClass.h"
#include "Mdt/Entity/Def.h"
#include "Mdt/Entity/UniqueConstraint.h"
#include "Mdt/Entity/Relation.h"
#include "MdtRailway_CoreExport.h"
#include <QtGlobal>
#include <QString>

namespace Mdt{ namespace Railway{ namespace Entity{

  struct MDT_RAILWAY_CORE_EXPORT VehicleTypeDataStruct
  {
    qulonglong id = 0;
    qulonglong vehicleTypeClassId = 0;
    QString manufacturerSerie;
  };

}}} // namespace Mdt{ namespace Railway{ namespace Entity{

MDT_ENTITY_DEF(
  (Mdt, Railway, Entity, VehicleTypeDataStruct),
  VehicleType,
  (id, FieldFlag::IsPrimaryKey),
  (vehicleTypeClassId, FieldFlag::IsRequired),
  (manufacturerSerie, FieldFlag::IsRequired, FieldMaxLength(10))
)

namespace Mdt{ namespace Railway{ namespace Entity{

  using VehicleTypeUniqueConstraint = Mdt::Entity::UniqueConstraint<VehicleTypeEntity, VehicleTypeDef::vehicleTypeClassIdField, VehicleTypeDef::manufacturerSerieField>;

  using VehicleTypeClassVehicleTypeRelation = Mdt::Entity::Relation<VehicleTypeClassEntity, VehicleTypeEntity, VehicleTypeDef::vehicleTypeClassIdField>;

}}} // namespace Mdt{ namespace Railway{ namespace Entity{

#endif // #ifndef MDT_RAILWAY_ENTITY_VEHICLE_TYPE_H

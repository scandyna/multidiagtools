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

#include "Mdt/Entity/Def.h"
#include "Mdt/Entity/DataTemplate.h"
#include <QtGlobal>
#include <QString>

namespace Mdt{ namespace Railway{ namespace Entity{

  struct VehicleTypeDataStruct
  {
    qulonglong id;
    QString name;
  };

}}} // namespace Mdt{ namespace Railway{ namespace Entity{

MDT_ENTITY_DEF(
  (Mdt, Railway, Entity, VehicleTypeDataStruct),
  VehicleType,
  (id, FieldFlag::IsPrimaryKey),
  (name, FieldFlag::IsRequired, FieldMaxLength(50))
)

namespace Mdt{ namespace Railway{ namespace Entity{

  class VehicleTypeData : public Mdt::Entity::DataTemplate<VehicleTypeDataStruct, VehicleTypeDef>
  {
  };

}}} // namespace Mdt{ namespace Railway{ namespace Entity{

#endif // #ifndef MDT_RAILWAY_ENTITY_VEHICLE_TYPE_H

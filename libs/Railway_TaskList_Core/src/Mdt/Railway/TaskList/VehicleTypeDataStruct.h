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
#ifndef MDT_RAILWAY_TASK_LIST_VEHICLE_TYPE_DATA_STRUCT_H
#define MDT_RAILWAY_TASK_LIST_VEHICLE_TYPE_DATA_STRUCT_H

#include "Mdt/Reflection/ReflectStruct.h"
#include "Mdt/Reflection/AutoIncrementIdPrimaryKey.h"
#include "MdtRailway_TaskList_CoreExport.h"
#include <QString>

namespace Mdt{ namespace Railway{ namespace TaskList{

  struct MDT_RAILWAY_TASKLIST_CORE_EXPORT VehicleTypeDataStruct
  {
    VehicleTypeDataStruct() = default;

    VehicleTypeDataStruct(const QString & name, const QString & alias, const QString & serie)
     : name(name),
       alias(alias),
       manufacturerSerie(serie)
    {
    }

    qlonglong id = 0;
    QString name;
    QString alias;
    QString manufacturerSerie;
  };

}}} // namespace Mdt{ namespace Railway{ namespace TaskList{

MDT_REFLECT_STRUCT(
  (Mdt,Railway,TaskList,VehicleTypeDataStruct),
  VehicleType,
  (id),
  (name),
  (alias),
  (manufacturerSerie)
)

namespace Mdt{ namespace Railway{ namespace TaskList{

  using VehicleTypePrimaryKey = Mdt::Reflection::AutoIncrementIdPrimaryKey<VehicleTypeDef::id>;

}}} // namespace Mdt{ namespace Railway{ namespace TaskList{

#endif // MDT_RAILWAY_TASK_LIST_VEHICLE_TYPE_DATA_STRUCT_H

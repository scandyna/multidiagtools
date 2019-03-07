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
#ifndef MDT_RAILWAY_TASK_LIST_VEHICLE_TYPE_P_H
#define MDT_RAILWAY_TASK_LIST_VEHICLE_TYPE_P_H

#include "VehicleType.h"
#include "VehicleTypeDataStruct.h"
#include "MdtRailway_TaskList_CoreExport.h"

namespace Mdt{ namespace Railway{ namespace TaskList{

  inline
  const VehicleTypeDataStruct & privateConstVehicleTypeDataStruct(const VehicleType & vehicleType)
  {
    return vehicleType.mDataStruct;
  }

  inline
  VehicleTypeDataStruct & privateVehicleTypeDataStruct(VehicleType & vehicleType)
  {
    return vehicleType.mDataStruct;
  }

}}} // namespace Mdt{ namespace Railway{ namespace TaskList{

#endif // MDT_RAILWAY_TASK_LIST_VEHICLE_TYPE_P_H
